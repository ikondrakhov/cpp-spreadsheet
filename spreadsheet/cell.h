#pragma once

#include "common.h"
#include "formula.h"

#include <unordered_set>
#include <optional>

class Cell : public CellInterface {
public:
    Cell(std::string text, const SheetInterface& sheet);
    ~Cell();

    void Set(std::string text, const SheetInterface& sheet);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;

    void AddReferedCell(Position p);
    const std::vector<Position>& GetReferedCells() const;
    void InvalidateCache();
    
    
    std::vector<Position> GetReferencedCells() const override;

private:
    
    class Impl;
    class EmptyImpl;
    class TextImpl;
    class FormulaImpl;
    std::unique_ptr<Impl> impl_;
    mutable std::optional<Value> cache_;
    std::vector<Position> refered_cells_;

    class Impl {
    public:
        using Value = std::variant<std::string, double, FormulaError>;
        virtual Value GetValue() const = 0;
        virtual std::string GetText() const = 0;
        virtual std::vector<Position> GetReferencedCells() const {
            return {};
        }
    };

    class EmptyImpl: public Impl {
    public:
        virtual void Set(std::string text);
        virtual Value GetValue() const;
        virtual std::string GetText() const;
    };

    class TextImpl: public Impl {
    public:
        TextImpl(std::string text);

        virtual Value GetValue() const;
        virtual std::string GetText() const;
    private:
        std::string _value;
    };

    class FormulaImpl: public Impl {
    public:
        FormulaImpl(std::string text, const SheetInterface& sheet);

        virtual Value GetValue() const;

        virtual std::string GetText() const;
        
        virtual std::vector<Position> GetReferencedCells() const override;
    private:
        Value value_;
        std::string text_;
        std::unique_ptr<FormulaInterface> formula_;
    };

};