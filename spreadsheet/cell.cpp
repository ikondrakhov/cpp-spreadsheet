#include "cell.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>
#include <queue>

// Реализуйте следующие методы
Cell::Cell(std::string value, const SheetInterface& sheet){
    Set(value, sheet);
}

Cell::~Cell() {}

void Cell::Set(std::string text, const SheetInterface& sheet) {
    if(text[0] == '=' && text.size() != 1) {
        impl_ = std::make_unique<FormulaImpl>(text.substr(1), sheet);
    } else {
        impl_ = std::make_unique<TextImpl>(text);
    }
}

void Cell::Clear() {
    impl_ = std::make_unique<EmptyImpl>();
}

Cell::Value Cell::GetValue() const {
    if(!cache_.has_value()) {
        cache_ = impl_->GetValue();
    }
    return cache_.value();
}
std::string Cell::GetText() const {
    return impl_->GetText();
}

void Cell::InvalidateCache() {
    cache_.reset();
}

void Cell::AddReferedCell(Position p) {
    refered_cells_.push_back(p);
}

std::vector<Position> Cell::GetReferencedCells() const {
    return impl_->GetReferencedCells();
};

void Cell::EmptyImpl::Set(std::string text) {};

Cell::Value Cell::EmptyImpl::GetValue() const {
    return "";
}

std::string Cell::EmptyImpl::GetText() const {
    return "";
}

Cell::TextImpl::TextImpl(std::string text) {
    _value = text;
}

Cell::Value Cell::TextImpl::GetValue() const {
    if(_value[0] == '\'') {
        return _value.substr(1);
    } 
    return _value;
}

std::string Cell::TextImpl::GetText() const {
    return _value;
}

Cell::FormulaImpl::FormulaImpl(std::string text, const SheetInterface& sheet) {
    text_ = text;
    formula_ = ParseFormula(text);
    auto v = formula_->Evaluate(sheet);
    if(std::holds_alternative<double>(v)) {
        value_ = std::get<double>(v);
    } else {
        value_ = std::get<FormulaError>(v);
    }
}

Cell::Value Cell::FormulaImpl::GetValue() const {
    return value_;
}

std::string Cell::FormulaImpl::GetText() const {
    return "=" + formula_->GetExpression();
}


std::vector<Position> Cell::FormulaImpl::GetReferencedCells() const {
    return formula_->GetReferencedCells();
};


const std::vector<Position>& Cell::GetReferedCells() const {
    return refered_cells_;
}