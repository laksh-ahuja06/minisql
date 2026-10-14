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

// Generic display tree: each AST node is converted into a label plus child nodes, then drawn
// with box-drawing connectors (like the `tree` command).
struct TreeNode {
    std::string label;
    std::vector<TreeNode> children;
};

// Converts a WHERE expression into a display tree.
TreeNode exprToTree(const std::shared_ptr<Expr>& e) {
    if (!e) return {"(empty)", {}};
    switch (e->type) {
        case ExprType::BINARY:      return {"BinaryExpr (" + e->op + ")", {exprToTree(e->left), exprToTree(e->right)}};
        case ExprType::COLUMN_REF:  return {"ColumnRef: " + e->columnName, {}};
        case ExprType::LITERAL_NUM: return {"LiteralNum: " + e->literalValue, {}};
        case ExprType::LITERAL_STR: return {"LiteralStr: '" + e->literalValue + "'", {}};
    }
    return {"(unknown)", {}};
}

// Builds a labelled node whose children are plain leaf strings (e.g. a column list).
TreeNode listNode(const std::string& label, const std::vector<std::string>& items) {
    TreeNode node{label, {}};
    for (auto& item : items) node.children.push_back({item, {}});
    return node;
}

// Converts a parsed statement into a display tree.
TreeNode stmtToTree(const Stmt& s) {
    if (s.type == StmtType::CREATE_TABLE) {
        return {"CreateTableStmt", {{"Table: " + s.createStmt->tableName, {}},
                                    listNode("Columns", s.createStmt->columns)}};
    }
    if (s.type == StmtType::INSERT) {
        return {"InsertStmt", {{"Table: " + s.insertStmt->tableName, {}},
                               listNode("Values", s.insertStmt->values)}};
    }
    if (s.type == StmtType::SELECT) {
        auto& sel = s.selectStmt;
        TreeNode node{"SelectStmt", {}};
        node.children.push_back(sel->selectAll ? TreeNode{"Columns: *", {}} : listNode("Columns", sel->columns));
        node.children.push_back({"From: " + sel->tableName, {}});
        if (sel->hasWhere) node.children.push_back({"Where", {exprToTree(sel->whereClause)}});
        if (sel->hasOrderBy) node.children.push_back({"OrderBy: " + sel->orderByColumn, {}});
        return node;
    }
    if (s.type == StmtType::DELETE) {
        TreeNode node{"DeleteStmt", {{"Table: " + s.deleteStmt->tableName, {}}}};
        if (s.deleteStmt->hasWhere) node.children.push_back({"Where", {exprToTree(s.deleteStmt->whereClause)}});
        return node;
    }
    if (s.type == StmtType::UPDATE) {
        auto& upd = s.updateStmt;
        TreeNode node{"UpdateStmt", {{"Table: " + upd->tableName, {}},
                                     {"Set", {{"Column: " + upd->columnName, {}}, {"Value: " + upd->newValue, {}}}}}};
        if (upd->hasWhere) node.children.push_back({"Where", {exprToTree(upd->whereClause)}});
        return node;
    }
    return {"(unknown statement)", {}};
}

// Recursively draws a tree's children; prefix holds the vertical guide lines of the ancestors.
void printTree(const TreeNode& node, const std::string& prefix) {
    for (size_t i = 0; i < node.children.size(); ++i) {
        bool last = (i + 1 == node.children.size());
        std::cout << prefix << (last ? "└── " : "├── ") << node.children[i].label << "\n";
        printTree(node.children[i], prefix + (last ? "    " : "│   "));
    }
}

// Pretty-prints the whole program (all parsed statements) as a single AST tree.
void printAST(const std::vector<Stmt>& stmts) {
    TreeNode program{"Program", {}};
    for (auto& s : stmts) program.children.push_back(stmtToTree(s));
    std::cout << program.label << "\n";
    printTree(program, "");
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
            std::cout << stmts.size() << " statement(s) parsed successfully.\n\n";
            printAST(stmts);
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
