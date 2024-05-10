#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>
#include <set>

using namespace std::literals;

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << "#ARITHM!";
}

FormulaError::FormulaError(Category category): category_(category) {}

FormulaError::Category FormulaError::GetCategory() const {
    return category_;
}

bool FormulaError::operator==(FormulaError rhs) const {
    return category_ == rhs.category_;
}

std::string_view FormulaError::ToString() const {
    switch(category_) {
        case FormulaError::Category::Ref:
            return "#REF!";
        case FormulaError::Category::Value:
            return "#VALUE!";
        case FormulaError::Category::Arithmetic:
            return "#ARITHM!";
    }
    return "";
}

namespace {
class Formula : public FormulaInterface {
public:
// Реализуйте следующие методы:
    explicit Formula(std::string expression) try : ast_(ParseFormulaAST(expression)) 
        {}
        catch (...) {
            throw FormulaException("Error parsing formula");
        }
    Value Evaluate(const SheetInterface& sheet) const override {
        try {
            return ast_.Execute([&](const Position& p) -> const CellInterface* {
                auto cell = sheet.GetCell(p);
                return cell;
            });
        } catch (const FormulaError& e) {
            return e;
        } catch (const InvalidPositionException&) {
            return FormulaError(FormulaError::Category::Ref);
        }
    }
    std::string GetExpression() const override  {
        std::ostringstream formula; 
        ast_.PrintFormula(formula);
        return formula.str();
    }
    
    
    std::vector<Position> GetReferencedCells() const {
        const auto list = ast_.GetCells();
        std::set<Position> cells = {std::begin(list), std::end(list)};
        return {std::begin(cells), std::end(cells)};
    }

private:
    FormulaAST ast_;
};
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    return std::make_unique<Formula>(std::move(expression));
}