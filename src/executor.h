// executor.h - Query execution engine declaration for MiniSQL statements.
#pragma once
#include "ast.h"
#include "storage.h"
#include "catalog.h"
#include <memory>
#include <string>
#include <vector>

/**
 * @brief Execution Engine that evaluates MiniSQL AST statements against in-memory storage.
 */
class Executor {
public:
    /**
     * @brief Constructs Executor with references to storage and schema catalog.
     * @param storage Mutable reference to storage engine.
     * @param catalog Mutable reference to schema catalog.
     */
    Executor(Storage& storage, Catalog& catalog);

    /**
     * @brief Executes a single AST statement (CREATE TABLE, INSERT, or SELECT).
     * @param stmt The statement AST to execute.
     */
    void execute(const Stmt& stmt);

private:
    Storage& storage;
    Catalog& catalog;

    void executeCreate(const CreateTableStmt& stmt);
    void executeInsert(const InsertStmt& stmt);
    void executeSelect(const SelectStmt& stmt);

    bool evaluateWhere(const std::shared_ptr<Expr>& expr,
                       const std::vector<std::string>& row,
                       const std::vector<std::string>& colNames) const;

    std::string evalOperand(const std::shared_ptr<Expr>& expr,
                            const std::vector<std::string>& row,
                            const std::vector<std::string>& colNames,
                            bool& isNumber) const;

    static bool isNumeric(const std::string& str, double& val);
};
