// executor.cpp - Implementation of MiniSQL Query Execution Engine.
#include "executor.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cctype>

Executor::Executor(Storage& storage, Catalog& catalog)
    : storage(storage), catalog(catalog) {}

void Executor::execute(const Stmt& stmt) {
    if (stmt.type == StmtType::CREATE_TABLE) {
        executeCreate(*stmt.createStmt);
    } else if (stmt.type == StmtType::INSERT) {
        executeInsert(*stmt.insertStmt);
    } else if (stmt.type == StmtType::SELECT) {
        executeSelect(*stmt.selectStmt);
    } else if (stmt.type == StmtType::DELETE) {
        executeDelete(*stmt.deleteStmt);
    } else if (stmt.type == StmtType::UPDATE) {
        executeUpdate(*stmt.updateStmt);
    }
}

void Executor::executeCreate(const CreateTableStmt& stmt) {
    storage.createTable(stmt.tableName, stmt.columns);
    std::cout << "Table '" << stmt.tableName << "' created successfully.\n";
}

void Executor::executeInsert(const InsertStmt& stmt) {
    storage.insertRow(stmt.tableName, stmt.values);
    std::cout << "1 row inserted into '" << stmt.tableName << "'.\n";
}

void Executor::executeDelete(const DeleteStmt& stmt) {
    Table& table = storage.getTable(stmt.tableName);
    size_t initialCount = table.rows.size();

    std::vector<std::vector<std::string>> remainingRows;
    for (const auto& row : table.rows) {
        bool matches = !stmt.hasWhere || evaluateWhere(stmt.whereClause, row, table.columns);
        if (!matches) {
            remainingRows.push_back(row);
        }
    }
    size_t deletedCount = initialCount - remainingRows.size();
    table.rows = std::move(remainingRows);
    if (deletedCount == 1) {
        std::cout << "1 row deleted from '" << stmt.tableName << "'.\n";
    } else {
        std::cout << deletedCount << " rows deleted from '" << stmt.tableName << "'.\n";
    }
}

void Executor::executeUpdate(const UpdateStmt& stmt) {
    Table& table = storage.getTable(stmt.tableName);
    auto colIt = std::find(table.columns.begin(), table.columns.end(), stmt.columnName);
    size_t colIdx = std::distance(table.columns.begin(), colIt);

    size_t updatedCount = 0;
    for (auto& row : table.rows) {
        if (!stmt.hasWhere || evaluateWhere(stmt.whereClause, row, table.columns)) {
            row[colIdx] = stmt.newValue;
            updatedCount++;
        }
    }
    if (updatedCount == 1) {
        std::cout << "1 row updated in '" << stmt.tableName << "'.\n";
    } else {
        std::cout << updatedCount << " rows updated in '" << stmt.tableName << "'.\n";
    }
}

void Executor::executeSelect(const SelectStmt& stmt) {
    const Table& table = storage.getTable(stmt.tableName);

    // 1. Determine columns to project
    std::vector<std::string> projCols;
    std::vector<size_t> projIndices;

    if (stmt.selectAll) {
        projCols = table.columns;
        for (size_t i = 0; i < table.columns.size(); ++i) {
            projIndices.push_back(i);
        }
    } else {
        projCols = stmt.columns;
        for (const auto& c : stmt.columns) {
            auto it = std::find(table.columns.begin(), table.columns.end(), c);
            projIndices.push_back(std::distance(table.columns.begin(), it));
        }
    }

    // 2. Filter rows using WHERE clause
    std::vector<std::vector<std::string>> filteredRows;
    for (const auto& row : table.rows) {
        if (!stmt.hasWhere || evaluateWhere(stmt.whereClause, row, table.columns)) {
            filteredRows.push_back(row);
        }
    }

    // 3. Sort rows if ORDER BY clause is present
    if (stmt.hasOrderBy) {
        auto orderIt = std::find(table.columns.begin(), table.columns.end(), stmt.orderByColumn);
        size_t orderIdx = std::distance(table.columns.begin(), orderIt);

        std::stable_sort(filteredRows.begin(), filteredRows.end(),
            [orderIdx, this](const std::vector<std::string>& a, const std::vector<std::string>& b) {
                double valA = 0, valB = 0;
                bool numA = isNumeric(a[orderIdx], valA);
                bool numB = isNumeric(b[orderIdx], valB);
                if (numA && numB) {
                    return valA < valB;
                }
                return a[orderIdx] < b[orderIdx];
            });
    }

    // 4. Project requested columns
    std::vector<std::vector<std::string>> resultRows;
    for (const auto& row : filteredRows) {
        std::vector<std::string> projRow;
        for (size_t idx : projIndices) {
            projRow.push_back(row[idx]);
        }
        resultRows.push_back(projRow);
    }

    // 5. Format and display tabular output
    std::vector<size_t> widths(projCols.size());
    for (size_t i = 0; i < projCols.size(); ++i) {
        widths[i] = projCols[i].length();
    }
    for (const auto& r : resultRows) {
        for (size_t i = 0; i < r.size(); ++i) {
            widths[i] = std::max(widths[i], r[i].length());
        }
    }

    auto printBorder = [&]() {
        std::cout << "+";
        for (size_t w : widths) {
            std::cout << std::string(w + 2, '-') << "+";
        }
        std::cout << "\n";
    };

    auto printRow = [&](const std::vector<std::string>& r) {
        std::cout << "|";
        for (size_t i = 0; i < r.size(); ++i) {
            std::cout << " " << r[i];
            size_t pad = widths[i] - r[i].length();
            std::cout << std::string(pad, ' ') << " |";
        }
        std::cout << "\n";
    };

    printBorder();
    printRow(projCols);
    printBorder();
    for (const auto& r : resultRows) {
        printRow(r);
    }
    printBorder();

    size_t count = resultRows.size();
    if (count == 1) {
        std::cout << "(1 row)\n\n";
    } else {
        std::cout << "(" << count << " rows)\n\n";
    }
}

bool Executor::isNumeric(const std::string& str, double& val) {
    if (str.empty()) return false;
    try {
        size_t idx = 0;
        val = std::stod(str, &idx);
        return idx == str.size();
    } catch (...) {
        return false;
    }
}

std::string Executor::evalOperand(const std::shared_ptr<Expr>& expr,
                                  const std::vector<std::string>& row,
                                  const std::vector<std::string>& colNames,
                                  bool& isNumber) const {
    if (!expr) {
        isNumber = false;
        return "";
    }
    if (expr->type == ExprType::COLUMN_REF) {
        auto it = std::find(colNames.begin(), colNames.end(), expr->columnName);
        size_t idx = std::distance(colNames.begin(), it);
        std::string val = row[idx];
        double numVal = 0;
        isNumber = isNumeric(val, numVal);
        return val;
    } else if (expr->type == ExprType::LITERAL_NUM) {
        isNumber = true;
        return expr->literalValue;
    } else if (expr->type == ExprType::LITERAL_STR) {
        isNumber = false;
        return expr->literalValue;
    }
    isNumber = false;
    return "";
}

bool Executor::evaluateWhere(const std::shared_ptr<Expr>& expr,
                              const std::vector<std::string>& row,
                              const std::vector<std::string>& colNames) const {
    if (!expr) return true;

    if (expr->type == ExprType::BINARY) {
        if (expr->op == "AND") {
            return evaluateWhere(expr->left, row, colNames) &&
                   evaluateWhere(expr->right, row, colNames);
        }
        if (expr->op == "OR") {
            return evaluateWhere(expr->left, row, colNames) ||
                   evaluateWhere(expr->right, row, colNames);
        }

        // Comparison binary operators (=, >, <, >=, <=)
        bool numLeft = false, numRight = false;
        std::string valLeft = evalOperand(expr->left, row, colNames, numLeft);
        std::string valRight = evalOperand(expr->right, row, colNames, numRight);

        if (numLeft && numRight) {
            double dLeft = std::stod(valLeft);
            double dRight = std::stod(valRight);
            if (expr->op == "=")  return dLeft == dRight;
            if (expr->op == "!=") return dLeft != dRight;
            if (expr->op == ">")  return dLeft > dRight;
            if (expr->op == "<")  return dLeft < dRight;
            if (expr->op == ">=") return dLeft >= dRight;
            if (expr->op == "<=") return dLeft <= dRight;
        } else {
            if (expr->op == "=")  return valLeft == valRight;
            if (expr->op == "!=") return valLeft != valRight;
            if (expr->op == ">")  return valLeft > valRight;
            if (expr->op == "<")  return valLeft < valRight;
            if (expr->op == ">=") return valLeft >= valRight;
            if (expr->op == "<=") return valLeft <= valRight;
        }
    }
    return true;
}
