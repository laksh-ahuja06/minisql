#pragma once
#include <string>
#include <vector>
#include <memory>

enum class ExprType { LITERAL_NUM, LITERAL_STR, COLUMN_REF, BINARY };

struct Expr {
    ExprType type;
    std::string literalValue;
    std::string columnName;
    std::string op;
    std::shared_ptr<Expr> left;
    std::shared_ptr<Expr> right;
};

struct CreateTableStmt {
    std::string tableName;
    std::vector<std::string> columns;
};

struct InsertStmt {
    std::string tableName;
    std::vector<std::string> values;
};

struct SelectStmt {
    std::vector<std::string> columns;
    bool selectAll = false;
    std::string tableName;

    bool hasWhere = false;
    std::shared_ptr<Expr> whereClause;

    bool hasOrderBy = false;
    std::string orderByColumn;
};

enum class StmtType { CREATE_TABLE, INSERT, SELECT };

struct Stmt {
    StmtType type;
    std::shared_ptr<CreateTableStmt> createStmt;
    std::shared_ptr<InsertStmt> insertStmt;
    std::shared_ptr<SelectStmt> selectStmt;
};