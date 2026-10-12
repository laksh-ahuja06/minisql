// token.h - Defines token types and Token structure for the MiniSQL lexer.
#pragma once
#include <string>

/**
 * @brief Represents all supported lexical token types in MiniSQL.
 */
enum class TokenType {
    // Keywords
    SELECT, FROM, WHERE, ORDER, BY,
    CREATE, TABLE, INSERT, INTO, VALUES,
    DELETE, UPDATE, SET,
    AND, OR,

    // Literal values and identifiers
    IDENTIFIER, NUMBER, STRING,

    // Comparison operators (=, >, <, >=, <=)
    EQUALS, GREATER, LESS, GREATER_EQ, LESS_EQ,

    // Punctuation characters
    COMMA, STAR, LPAREN, RPAREN, SEMICOLON,

    // Stream control / error tokens
    END_OF_FILE, UNKNOWN
};

/**
 * @brief Represents a single lexical token produced by the Lexer.
 */
struct Token {
    TokenType type;     ///< The category of the token (keyword, operator, value, etc.)
    std::string text;   ///< Actual matched lexeme text from source SQL (e.g. "students", ">=")
    int line;           ///< 1-based line number in source for error context
};

/**
 * @brief Converts a TokenType enum to its human-readable string representation.
 * @param type The TokenType enum value.
 * @return String name of the token type.
 */
std::string tokenTypeToString(TokenType type);