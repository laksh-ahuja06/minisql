#pragma once
#include "token.h"
#include "ast.h"
#include <vector>
#include <memory>

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::vector<Stmt> parseProgram();

private:
    std::vector<Token> tokens;
    int pos = 0;

    Token peek();
    Token advance();
    bool check(TokenType type);
    Token expect(TokenType type, const std::string& errMsg);
    bool isAtEnd();

    Stmt parseStatement();
    Stmt parseCreateTable();
    Stmt parseInsert();
    Stmt parseSelect();

    std::shared_ptr<Expr> parseOr();
    std::shared_ptr<Expr> parseAnd();
    std::shared_ptr<Expr> parseComparison();
    std::shared_ptr<Expr> parseOperand();
};
