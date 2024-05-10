#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>
#include <map>
#include <queue>

using namespace std::literals;

Sheet::~Sheet() {}

void Sheet::SetCell(Position pos, std::string text) {
    if(pos.col >= Position::MAX_COLS || pos.row >= Position::MAX_ROWS
      || pos.col < 0 || pos.row < 0) {
        throw InvalidPositionException("Invalid position");
    }
    
    //invalidate cache
    if(cells_.find(pos) != cells_.end() && cells_[pos] != nullptr) {
        std::queue<Position> next_positions;
        std::unordered_set<std::string> visited_cells;
        cells_[pos]->InvalidateCache();
        for(auto p: cells_[pos]->GetReferedCells()) {
            next_positions.push(p);
            cells_[p]->InvalidateCache();
            visited_cells.insert(p.ToString());
        }
        
        while (!next_positions.empty()) {
            Position current_pos = next_positions.front();
            next_positions.pop();
            for(auto p: cells_[current_pos]->GetReferedCells()) {
                if(visited_cells.find(p.ToString()) == visited_cells.end()) {
                    next_positions.push(p);
                }
                cells_[p]->InvalidateCache();
                visited_cells.insert(p.ToString());
            }
        }
    }
    
    //create new cell and replace with old
    std::unique_ptr<Cell> cell = std::make_unique<Cell>(text, *this);
    std::unique_ptr<Cell> tmp_cell = std::move(cells_[pos]);
    cells_[pos] = std::move(cell);
    for(Position referenced_cell_pos: cells_[pos]->GetReferencedCells()) {
        if(cells_.find(referenced_cell_pos) == cells_.end()) {
            cells_[referenced_cell_pos] = std::make_unique<Cell>("", *this);
        }
        cells_[referenced_cell_pos]->AddReferedCell(pos);
    }
    
    //if contains cyclic dependencies return old value
    try {
        CheckCyclicDependencies(pos);
    } catch (CircularDependencyException& e) {
        cells_[pos] = std::move(tmp_cell);
        throw e;
    }
    
    if(pos.col >= size_.cols) {
        size_.cols = pos.col + 1;
    }
    if(pos.row >= size_.rows) {
        size_.rows = pos.row + 1;
    }
}

const CellInterface* Sheet::GetCell(Position pos) const {
    if(pos.col >= Position::MAX_COLS || pos.row >= Position::MAX_ROWS
      || pos.col < 0 || pos.row < 0) {
        throw InvalidPositionException("Invalid position");
    }
    if(cells_.find(pos) == cells_.end()) {
        return nullptr;
    }
    return cells_.at(pos).get();
}
CellInterface* Sheet::GetCell(Position pos) {
    if(pos.col >= Position::MAX_COLS || pos.row >= Position::MAX_ROWS
      || pos.col < 0 || pos.row < 0) {
        throw InvalidPositionException("Invalid position");
    }
    if(cells_.find(pos) == cells_.end()) {
        return nullptr;
    }
    return cells_.at(pos).get();
}

void Sheet::ClearCell(Position pos) {
    if(pos.col >= Position::MAX_COLS || pos.row >= Position::MAX_ROWS
      || pos.col < 0 || pos.row < 0) {
        throw InvalidPositionException("Invalid position");
    }
    cells_.erase(pos);
    if(cells_.size() == 0) {
        size_ = {0, 0};
    } else {
        size_ = {cells_.rbegin()->first.row + 1, cells_.rbegin()->first.col + 1};
    }
}

Size Sheet::GetPrintableSize() const {
    return size_;
}

void Sheet::PrintValues(std::ostream& output) const {
    for(int row_n = 0; row_n < size_.rows; row_n++) {
        for(int col_n = 0; col_n < size_.cols; col_n++) {
            if(cells_.find({row_n, col_n}) != cells_.end()) {
                auto value = cells_.at({row_n, col_n})->GetValue();
                if(std::holds_alternative<double>(value)) {
                    output << std::get<double>(value);
                } else if (std::holds_alternative<std::string>(value)) {
                    output << std::get<std::string>(value);
                } else {
                    output << std::get<FormulaError>(value);
                }
            }
            if(col_n == size_.cols - 1) {
                output << '\n';
            } else {
                output << '\t';
            }
        }
    }
}
void Sheet::PrintTexts(std::ostream& output) const {
    for(int row_n = 0; row_n < size_.rows; row_n++) {
        for(int col_n = 0; col_n < size_.cols; col_n++) {
            if(cells_.find({row_n, col_n}) != cells_.end()) {
                output << cells_.at({row_n, col_n})->GetText();
            }
            if(col_n == size_.cols - 1) {
                output << '\n';
            } else {
                output << '\t';
            }
        }
    }
}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}

const Cell* Sheet::GetConcreteCell(Position pos) const {
    if(pos.col >= Position::MAX_COLS || pos.row >= Position::MAX_ROWS
      || pos.col < 0 || pos.row < 0) {
        throw InvalidPositionException("Invalid position");
    }
    if(cells_.find(pos) == cells_.end()) {
        return nullptr;
    }
    return cells_.at(pos).get();
}

Cell* Sheet::GetConcreteCell(Position pos) {
    if(pos.col >= Position::MAX_COLS || pos.row >= Position::MAX_ROWS
      || pos.col < 0 || pos.row < 0) {
        throw InvalidPositionException("Invalid position");
    }
    if(cells_.find(pos) == cells_.end()) {
        return nullptr;
    }
    return cells_.at(pos).get();
}


void Sheet::CheckCyclicDependencies(Position cell_position) {
    std::queue<Position> cells_queue;
    for(auto cell_pos: GetConcreteCell(cell_position)->GetReferencedCells()) {
        cells_queue.push(cell_pos);
    }
    std::unordered_set<std::string> visited_cells;
    
    while(!cells_queue.empty()) {
        Position p = cells_queue.front();
        cells_queue.pop();
        
        if(visited_cells.find(p.ToString()) == visited_cells.end()) {
            for(auto cell_pos: GetConcreteCell(p)->GetReferencedCells()) {
                cells_queue.push(cell_pos);
            }
        }
        visited_cells.insert(p.ToString());
        
        if(p == cell_position) {
            throw CircularDependencyException("Circular dependency exception");
        }
    }
}