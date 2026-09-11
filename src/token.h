// token.h
#pragma once
#include <string>

enum class TokenType {
    // Keywords
    SELECT, FROM, WHERE, ORDER, BY,
    CREATE, TABLE, INSERT, INTO, VALUES,
    AND, OR,

    // Values
    IDENTIFIER, NUMBER, STRING,

    // Comparison operators
    EQUALS, GREATER, LESS, GREATER_EQ, LESS_EQ,

    // Punctuation
    COMMA, STAR, LPAREN, RPAREN, SEMICOLON,

    // Special
    END_OF_FILE, UNKNOWN
};

struct Token {
    TokenType type;
    std::string text;  // actual matched text, e.g. "students", ">="
    int line;           // for error messages
};

std::string tokenTypeToString(TokenType type);