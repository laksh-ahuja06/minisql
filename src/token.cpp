// token.cpp - Implementation of token utility helper functions.
#include "token.h"

// Maps TokenType enum values to their string names for debugging and AST logging.
std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::SELECT: return "SELECT";
        case TokenType::FROM: return "FROM";
        case TokenType::WHERE: return "WHERE";
        case TokenType::ORDER: return "ORDER";
        case TokenType::BY: return "BY";
        case TokenType::CREATE: return "CREATE";
        case TokenType::TABLE: return "TABLE";
        case TokenType::INSERT: return "INSERT";
        case TokenType::INTO: return "INTO";
        case TokenType::VALUES: return "VALUES";
        case TokenType::DELETE: return "DELETE";
        case TokenType::UPDATE: return "UPDATE";
        case TokenType::SET: return "SET";
        case TokenType::AND: return "AND";
        case TokenType::OR: return "OR";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::STRING: return "STRING";
        case TokenType::EQUALS: return "EQUALS";
        case TokenType::GREATER: return "GREATER";
        case TokenType::LESS: return "LESS";
        case TokenType::GREATER_EQ: return "GREATER_EQ";
        case TokenType::LESS_EQ: return "LESS_EQ";
        case TokenType::COMMA: return "COMMA";
        case TokenType::STAR: return "STAR";
        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        case TokenType::UNKNOWN: return "UNKNOWN";
        default: return "UNKNOWN";
    }
}
