// parser.h - Recursive descent parser class declaration for MiniSQL.
#pragma once
#include "token.h"
#include "ast.h"
#include <vector>
#include <memory>

/**
 * @brief Recursive descent parser that converts a stream of Tokens into an Abstract Syntax Tree (AST).
 */
class Parser {
public:
    /**
     * @brief Constructs a Parser with a token vector.
     * @param tokens Stream of tokens from Lexer.
     */
    Parser(const std::vector<Token>& tokens);

    /**
     * @brief Parses the token stream into a list of SQL statements.
     * @return std::vector<Stmt> List of parsed AST statement nodes.
     * @throws std::runtime_error if a syntax error occurs.
     */
    std::vector<Stmt> parseProgram();

private:
    std::vector<Token> tokens;  ///< Input token stream
    int pos = 0;                 ///< Current index in tokens

    /// Returns the token at current position without advancing.
    Token peek();

    /// Returns current token and advances position cursor by 1.
    Token advance();

    /// Returns true if the current token matches the expected type.
    bool check(TokenType type);

    /// Asserts current token matches type and advances; throws error if mismatched.
    Token expect(TokenType type, const std::string& errMsg);

    /// Returns true if current token is END_OF_FILE.
    bool isAtEnd();

    /// Parses a single statement (CREATE TABLE, INSERT, or SELECT).
    Stmt parseStatement();

    /// Parses a CREATE TABLE statement.
    Stmt parseCreateTable();

    /// Parses an INSERT INTO statement.
    Stmt parseInsert();

    /// Parses a SELECT statement.
    Stmt parseSelect();

    // Expression parsing hierarchy (lowest to highest precedence):
    /// Parses OR expressions.
    std::shared_ptr<Expr> parseOr();

    /// Parses AND expressions.
    std::shared_ptr<Expr> parseAnd();

    /// Parses comparison expressions (=, >, <, >=, <=).
    std::shared_ptr<Expr> parseComparison();

    /// Parses leaf operands (identifiers, numbers, string literals).
    std::shared_ptr<Expr> parseOperand();
};
