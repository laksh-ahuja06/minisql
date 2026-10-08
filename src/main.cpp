// main.cpp - Entry point for MiniSQL compiler pipeline (lexer -> parser -> semantic analyzer).
#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "catalog.h"
#include "storage.h"
#include "executor.h"
#include <iostream>
#include <fstream>
#include <sstream>

// Recursively prints formatted representation of expression AST nodes.
void printExpr(const std::shared_ptr<Expr>& e, int indent) {
    std::string pad(indent, ' ');
    if (!e) return;
    if (e->type == ExprType::BINARY) {
        std::cout << pad << "BinaryExpr(op=\"" << e->op << "\")\n";
        printExpr(e->left, indent + 2);
        printExpr(e->right, indent + 2);
    } else if (e->type == ExprType::COLUMN_REF) {
        std::cout << pad << "ColumnRef(" << e->columnName << ")\n";
    } else if (e->type == ExprType::LITERAL_NUM) {
        std::cout << pad << "LiteralNum(" << e->literalValue << ")\n";
    } else if (e->type == ExprType::LITERAL_STR) {
        std::cout << pad << "LiteralStr('" << e->literalValue << "')\n";
    }
}

// Formats and prints parsed SQL statement AST structure to standard output.
void printStmt(const Stmt& s) {
    if (s.type == StmtType::CREATE_TABLE) {
        std::cout << "CreateTableStmt\n  table: " << s.createStmt->tableName << "\n  columns: ";
        for (auto& c : s.createStmt->columns) std::cout << c << " ";
        std::cout << "\n";
    } else if (s.type == StmtType::INSERT) {
        std::cout << "InsertStmt\n  table: " << s.insertStmt->tableName << "\n  values: ";
        for (auto& v : s.insertStmt->values) std::cout << v << " ";
        std::cout << "\n";
    } else if (s.type == StmtType::SELECT) {
        auto& sel = s.selectStmt;
        std::cout << "SelectStmt\n  columns: ";
        if (sel->selectAll) std::cout << "*";
        else for (auto& c : sel->columns) std::cout << c << " ";
        std::cout << "\n  table: " << sel->tableName << "\n";
        if (sel->hasWhere) { std::cout << "  where:\n"; printExpr(sel->whereClause, 4); }
        if (sel->hasOrderBy) std::cout << "  orderBy: " << sel->orderByColumn << "\n";
    }
    std::cout << "----\n";
}

// Executes full MiniSQL pipeline (Lexing -> Parsing -> Semantic Validation -> Execution) on SQL string input.
void runQuery(const std::string& sql) {
    Catalog catalog;
    Storage storage;
    Executor executor(storage, catalog);

    try {
        // 1. Lexical Analysis
        Lexer lexer(sql);
        auto tokens = lexer.tokenize();

        // 2. Syntax Analysis (Parsing)
        Parser parser(tokens);
        auto stmts = parser.parseProgram();

        // 3. Semantic Analysis & Execution
        for (auto& s : stmts) {
            checkStatement(s, catalog);
            executor.execute(s);
        }
    } catch (const std::exception& e) {
        std::cout << "ERROR: " << e.what() << "\n";
    }
}

// Program main entrypoint supporting command-line SQL file execution or fallback to demo queries.
int main(int argc, char* argv[]) {
    if (argc > 1) {
        // Execute SQL file passed via command line argument
        std::ifstream file(argv[1]);
        if (!file) {
            std::cout << "Could not open file: " << argv[1] << "\n";
            return 1;
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        runQuery(buffer.str());
    } else {
        // Fallback execution of default sample SQL queries
        std::string defaultSql =
            "CREATE TABLE students (name, age, grade);"
            "INSERT INTO students VALUES ('Alice', 22, 'A');"
            "SELECT name, age FROM students WHERE age > 20 AND grade = 'A' ORDER BY age;"
            "SELECT * FROM students;";
        runQuery(defaultSql);
    }
    return 0;
}