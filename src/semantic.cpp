// semantic.cpp - Implementation of semantic analysis checks for MiniSQL.
#include "semantic.h"
#include <stdexcept>

// Recursively inspects expression nodes to ensure referenced column names exist in the catalog.
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

// Validates top-level statements against schema catalog and updates catalog for DDL statements.
void checkStatement(const Stmt& stmt, Catalog& catalog) {
    if (stmt.type == StmtType::CREATE_TABLE) {
        auto& c = stmt.createStmt;
        // Verify table does not already exist
        if (catalog.tableExists(c->tableName)) {
            throw std::runtime_error("Table '" + c->tableName + "' already exists");
        }
        // Register newly created table schema
        catalog.registerTable(c->tableName, c->columns);

    } else if (stmt.type == StmtType::INSERT) {
        auto& ins = stmt.insertStmt;
        // Verify target table exists
        if (!catalog.tableExists(ins->tableName)) {
            throw std::runtime_error("Unknown table '" + ins->tableName + "'");
        }
        // Verify inserted value count matches target table column count
        auto& cols = catalog.getColumns(ins->tableName);
        if (ins->values.size() != cols.size()) {
            throw std::runtime_error("Column count mismatch for table '" + ins->tableName +
                                      "': expected " + std::to_string(cols.size()) +
                                      ", got " + std::to_string(ins->values.size()));
        }

    } else if (stmt.type == StmtType::SELECT) {
        auto& sel = stmt.selectStmt;
        // Verify source table exists
        if (!catalog.tableExists(sel->tableName)) {
            throw std::runtime_error("Unknown table '" + sel->tableName + "'");
        }
        // Verify all projected column names exist (unless SELECT *)
        if (!sel->selectAll) {
            for (auto& col : sel->columns) {
                if (!catalog.columnExists(sel->tableName, col)) {
                    throw std::runtime_error("Unknown column '" + col +
                                              "' in table '" + sel->tableName + "'");
                }
            }
        }
        // Validate column references in WHERE clause
        if (sel->hasWhere) {
            checkExpr(sel->whereClause, sel->tableName, catalog);
        }
        // Validate ORDER BY column reference
        if (sel->hasOrderBy) {
            if (!catalog.columnExists(sel->tableName, sel->orderByColumn)) {
                throw std::runtime_error("Unknown column '" + sel->orderByColumn +
                                          "' in ORDER BY for table '" + sel->tableName + "'");
            }
        }

    } else if (stmt.type == StmtType::DELETE) {
        auto& del = stmt.deleteStmt;
        // Verify target table exists
        if (!catalog.tableExists(del->tableName)) {
            throw std::runtime_error("Unknown table '" + del->tableName + "'");
        }
        // Validate column references in WHERE clause
        if (del->hasWhere) {
            checkExpr(del->whereClause, del->tableName, catalog);
        }

    } else if (stmt.type == StmtType::UPDATE) {
        auto& upd = stmt.updateStmt;
        // Verify target table exists
        if (!catalog.tableExists(upd->tableName)) {
            throw std::runtime_error("Unknown table '" + upd->tableName + "'");
        }
        // Verify target column exists in table
        if (!catalog.columnExists(upd->tableName, upd->columnName)) {
            throw std::runtime_error("Unknown column '" + upd->columnName +
                                      "' in table '" + upd->tableName + "'");
        }
        // Validate column references in WHERE clause
        if (upd->hasWhere) {
            checkExpr(upd->whereClause, upd->tableName, catalog);
        }
    }
}
