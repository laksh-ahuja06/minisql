#include "semantic.h"
#include <stdexcept>

void checkExpr(const std::shared_ptr<Expr>& expr, const std::string& tableName, Catalog& catalog) {
    if (!expr) return;

    if (expr->type == ExprType::COLUMN_REF) {
        if (!catalog.columnExists(tableName, expr->columnName)) {
            throw std::runtime_error("Unknown column '" + expr->columnName +
                                      "' in table '" + tableName + "'");
        }
    } else if (expr->type == ExprType::BINARY) {
        checkExpr(expr->left, tableName, catalog);
        checkExpr(expr->right, tableName, catalog);
    }
}

void checkStatement(const Stmt& stmt, Catalog& catalog) {
    if (stmt.type == StmtType::CREATE_TABLE) {
        auto& c = stmt.createStmt;
        if (catalog.tableExists(c->tableName)) {
            throw std::runtime_error("Table '" + c->tableName + "' already exists");
        }
        catalog.registerTable(c->tableName, c->columns);

    } else if (stmt.type == StmtType::INSERT) {
        auto& ins = stmt.insertStmt;
        if (!catalog.tableExists(ins->tableName)) {
            throw std::runtime_error("Unknown table '" + ins->tableName + "'");
        }
        auto& cols = catalog.getColumns(ins->tableName);
        if (ins->values.size() != cols.size()) {
            throw std::runtime_error("Column count mismatch for table '" + ins->tableName +
                                      "': expected " + std::to_string(cols.size()) +
                                      ", got " + std::to_string(ins->values.size()));
        }

    } else if (stmt.type == StmtType::SELECT) {
        auto& sel = stmt.selectStmt;
        if (!catalog.tableExists(sel->tableName)) {
            throw std::runtime_error("Unknown table '" + sel->tableName + "'");
        }
        if (!sel->selectAll) {
            for (auto& col : sel->columns) {
                if (!catalog.columnExists(sel->tableName, col)) {
                    throw std::runtime_error("Unknown column '" + col +
                                              "' in table '" + sel->tableName + "'");
                }
            }
        }
        if (sel->hasWhere) {
            checkExpr(sel->whereClause, sel->tableName, catalog);
        }
        if (sel->hasOrderBy) {
            if (!catalog.columnExists(sel->tableName, sel->orderByColumn)) {
                throw std::runtime_error("Unknown column '" + sel->orderByColumn +
                                          "' in ORDER BY for table '" + sel->tableName + "'");
            }
        }
    }
}
