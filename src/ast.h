// ast.h - Abstract Syntax Tree (AST) node definitions for MiniSQL queries.
#pragma once
#include <string>
#include <vector>
#include <memory>

/**
 * @brief Categorizes expression nodes evaluated in WHERE clauses.
 */
enum class ExprType {
    LITERAL_NUM,    ///< Numeric literal (e.g. 42, 3.14)
    LITERAL_STR,    ///< String literal (e.g. 'Alice')
    COLUMN_REF,     ///< Column name reference (e.g. age)
    BINARY          ///< Binary expression with operator (e.g. AND, OR, >=, =)
};

/**
 * @brief Represents an expression tree node (e.g. in WHERE filtering clauses).
 */
struct Expr {
    ExprType type;                  ///< The expression node category
    std::string literalValue;       ///< Populated for LITERAL_NUM or LITERAL_STR
    std::string columnName;         ///< Populated for COLUMN_REF
    std::string op;                 ///< Populated for BINARY (e.g., "AND", "OR", "=", ">=")
    std::shared_ptr<Expr> left;     ///< Left operand for BINARY expressions
    std::shared_ptr<Expr> right;    ///< Right operand for BINARY expressions
};

/**
 * @brief Represents a parsed CREATE TABLE statement.
 * Example: CREATE TABLE students (name, age, grade);
 */
struct CreateTableStmt {
    std::string tableName;              ///< Target table name
    std::vector<std::string> columns;   ///< List of column names defined for the table
};

/**
 * @brief Represents a parsed INSERT INTO statement.
 * Example: INSERT INTO students VALUES ('Alice', 22, 'A');
 */
struct InsertStmt {
    std::string tableName;              ///< Target table name to insert into
    std::vector<std::string> values;    ///< Literal values to insert
};

/**
 * @brief Represents a parsed SELECT query statement.
 * Example: SELECT name, age FROM students WHERE age > 20 ORDER BY age;
 */
struct SelectStmt {
    std::vector<std::string> columns;   ///< Requested projection columns
    bool selectAll = false;             ///< Set to true if 'SELECT *' was used
    std::string tableName;              ///< Source table name

    bool hasWhere = false;              ///< True if a WHERE clause is present
    std::shared_ptr<Expr> whereClause;  ///< Root node of the WHERE expression tree

    bool hasOrderBy = false;            ///< True if an ORDER BY clause is present
    std::string orderByColumn;          ///< Column name to sort results by
};

/**
 * @brief Categorizes top-level SQL statement types supported by MiniSQL.
 */
enum class StmtType {
    CREATE_TABLE,   ///< DDL table creation
    INSERT,         ///< DML row insertion
    SELECT          ///< DML data query
};

/**
 * @brief Wrapper struct representing a single parsed SQL statement.
 */
struct Stmt {
    StmtType type;                                  ///< The statement category
    std::shared_ptr<CreateTableStmt> createStmt;    ///< Populated if type == CREATE_TABLE
    std::shared_ptr<InsertStmt> insertStmt;          ///< Populated if type == INSERT
    std::shared_ptr<SelectStmt> selectStmt;          ///< Populated if type == SELECT
};