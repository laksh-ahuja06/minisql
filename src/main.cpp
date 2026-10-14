// main.cpp - Entry point for MiniSQL compiler pipeline (lexer -> parser -> semantic analyzer -> executor).
#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "catalog.h"
#include "storage.h"
#include "executor.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstring>

// RUN the file :
// make
// ./sqlcompiler --verbose tests/sample1.sql

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
    } else if (s.type == StmtType::DELETE) {
        std::cout << "DeleteStmt\n  table: " << s.deleteStmt->tableName << "\n";
        if (s.deleteStmt->hasWhere) { std::cout << "  where:\n"; printExpr(s.deleteStmt->whereClause, 4); }
    } else if (s.type == StmtType::UPDATE) {
        std::cout << "UpdateStmt\n  table: " << s.updateStmt->tableName
                  << "\n  col: " << s.updateStmt->columnName
                  << " = " << s.updateStmt->newValue << "\n";
        if (s.updateStmt->hasWhere) { std::cout << "  where:\n"; printExpr(s.updateStmt->whereClause, 4); }
    }
    std::cout << "----\n";
}

// Prints a section banner used to separate pipeline phases in verbose mode.
void printPhase(const std::string& title) {
    std::cout << "\n========== " << title << " ==========\n";
}

// Prints the token stream produced by the lexer as a table of line, type and text.
void printTokens(const std::vector<Token>& tokens) {
    std::cout << std::left << std::setw(6) << "LINE" << std::setw(14) << "TYPE" << "TEXT\n";
    for (auto& t : tokens) {
        std::cout << std::left << std::setw(6) << t.line
                  << std::setw(14) << tokenTypeToString(t.type) << t.text << "\n";
    }
    std::cout << "(" << tokens.size() << " tokens)\n";
}

// Executes full MiniSQL pipeline (Lexing -> Parsing -> Semantic Validation -> Execution) on SQL string input.
// In verbose mode, the output of every phase is printed before execution.
void runQuery(const std::string& sql, bool verbose) {
    Catalog catalog;
    Storage storage;
    Executor executor(storage, catalog);

    try {
        // 1. Lexical Analysis
        Lexer lexer(sql);
        auto tokens = lexer.tokenize();
        if (verbose) {
            printPhase("PHASE 1: LEXICAL ANALYSIS (tokens)");
            printTokens(tokens);
        }

        // 2. Syntax Analysis (Parsing)
        Parser parser(tokens);
        auto stmts = parser.parseProgram();
        if (verbose) {
            printPhase("PHASE 2: SYNTAX ANALYSIS (AST)");
            std::cout << stmts.size() << " statement(s) parsed successfully.\n----\n";
            for (auto& s : stmts) printStmt(s);
            printPhase("PHASE 3 & 4: SEMANTIC ANALYSIS + EXECUTION");
        }

        // 3. Semantic Analysis & Execution
        for (size_t i = 0; i < stmts.size(); ++i) {
            checkStatement(stmts[i], catalog);
            if (verbose) std::cout << "[Statement " << i + 1 << "] semantic check: OK\n";
            executor.execute(stmts[i]);
        }
    } catch (const std::exception& e) {
        std::cout << "ERROR: " << e.what() << "\n";
    }
}

// Program main entrypoint supporting command-line SQL file execution or fallback to demo queries.
int main(int argc, char* argv[]) {
    // Optional --verbose / -v flag prints every compiler phase (tokens, AST, semantic checks)
    bool verbose = false;
    const char* path = nullptr;
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--verbose") == 0 || std::strcmp(argv[i], "-v") == 0) verbose = true;
        else path = argv[i];
    }

    if (path) {
        // Execute SQL file passed via command line argument
        std::ifstream file(path);
        if (!file) {
            std::cout << "Could not open file: " << path << "\n";
            return 1;
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        runQuery(buffer.str(), verbose);
    } else {
        // Fallback execution of default sample SQL queries
        std::string defaultSql =
            "CREATE TABLE students (name, age, grade);"
            "INSERT INTO students VALUES ('Alice', 22, 'A');"
            "SELECT name, age FROM students WHERE age > 20 AND grade = 'A' ORDER BY age;"
            "SELECT * FROM students;";
        runQuery(defaultSql, verbose);
    }
    return 0;
}
