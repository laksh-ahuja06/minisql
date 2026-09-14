// lexer.cpp - Implementation of the Lexer class for tokenizing SQL queries.
#include "lexer.h"
#include <cctype>
#include <unordered_map>

Lexer::Lexer(const std::string& src) : source(src) {}

bool Lexer::isAtEnd() { return pos >= (int)source.size(); }
char Lexer::peek() { return isAtEnd() ? '\0' : source[pos]; }
char Lexer::advance() { return source[pos++]; }

// Main tokenization loop scanning the source string character by character.
std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (!isAtEnd()) {
        char c = peek();

        // Skip whitespace characters
        if (c == ' ' || c == '\t' || c == '\r') { advance(); continue; }
        if (c == '\n') { line++; advance(); continue; }

        // Ignore single-line comments starting with '--'
        if (c == '-' && pos + 1 < (int)source.size() && source[pos + 1] == '-') {
            while (!isAtEnd() && peek() != '\n') advance();
            continue;
        }

        // Match identifiers or keywords starting with a letter or underscore
        if (isalpha(c) || c == '_') { tokens.push_back(makeIdentifierOrKeyword()); continue; }
        
        // Match numeric literals starting with a digit
        if (isdigit(c)) { tokens.push_back(makeNumber()); continue; }
        
        // Match string literals enclosed in single quotes
        if (c == '\'') { tokens.push_back(makeString()); continue; }

        advance(); // consume symbol character
        switch (c) {
            case '=': tokens.push_back({TokenType::EQUALS, "=", line}); break;
            case '>':
                if (peek() == '=') { advance(); tokens.push_back({TokenType::GREATER_EQ, ">=", line}); }
                else tokens.push_back({TokenType::GREATER, ">", line});
                break;
            case '<':
                if (peek() == '=') { advance(); tokens.push_back({TokenType::LESS_EQ, "<=", line}); }
                else tokens.push_back({TokenType::LESS, "<", line});
                break;
            case ',': tokens.push_back({TokenType::COMMA, ",", line}); break;
            case '*': tokens.push_back({TokenType::STAR, "*", line}); break;
            case '(': tokens.push_back({TokenType::LPAREN, "(", line}); break;
            case ')': tokens.push_back({TokenType::RPAREN, ")", line}); break;
            case ';': tokens.push_back({TokenType::SEMICOLON, ";", line}); break;
            default: tokens.push_back({TokenType::UNKNOWN, std::string(1, c), line}); break;
        }
    }

    tokens.push_back({TokenType::END_OF_FILE, "", line});
    return tokens;
}

// Scans an alphanumeric sequence and checks if it matches a SQL keyword (case-insensitive).
Token Lexer::makeIdentifierOrKeyword() {
    int start = pos;
    while (!isAtEnd() && (isalnum(peek()) || peek() == '_')) advance();
    std::string text = source.substr(start, pos - start);

    // Map of recognized uppercase SQL keywords
    static std::unordered_map<std::string, TokenType> keywords = {
        {"SELECT", TokenType::SELECT}, {"FROM", TokenType::FROM},
        {"WHERE", TokenType::WHERE}, {"ORDER", TokenType::ORDER},
        {"BY", TokenType::BY}, {"AND", TokenType::AND},
        {"OR", TokenType::OR}, {"CREATE", TokenType::CREATE},
        {"TABLE", TokenType::TABLE}, {"INSERT", TokenType::INSERT},
        {"INTO", TokenType::INTO}, {"VALUES", TokenType::VALUES}
    };

    // Convert text to uppercase for keyword comparison
    std::string upper = text;
    for (auto& ch : upper) ch = toupper(ch);

    if (keywords.count(upper)) return {keywords[upper], text, line};
    return {TokenType::IDENTIFIER, text, line};
}

// Scans digits and optional single decimal point.
Token Lexer::makeNumber() {
    int start = pos;
    bool hasDot = false;
    while (!isAtEnd()) {
        char c = peek();
        if (isdigit(c)) {
            advance();
        } else if (c == '.' && !hasDot && pos + 1 < (int)source.size() && isdigit(source[pos + 1])) {
            hasDot = true;
            advance();
        } else {
            break;
        }
    }
    return {TokenType::NUMBER, source.substr(start, pos - start), line};
}

// Scans single-quoted string literal, handling escape sequences (\n, \t, \', \\).
Token Lexer::makeString() {
    advance(); // skip opening single quote '
    std::string text;

    while (!isAtEnd()) {
        char c = peek();
        if (c == '\'') {
            break; // Closing quote reached
        } else if (c == '\\' && pos + 1 < (int)source.size()) {
            advance(); // Consume backslash
            char next = advance();
            if (next == 'n') text += '\n';
            else if (next == 't') text += '\t';
            else if (next == '\'') text += '\'';
            else if (next == '\\') text += '\\';
            else text += next;
        } else {
            if (c == '\n') line++;
            text += c;
            advance();
        }
    }

    if (isAtEnd()) {
        return {TokenType::UNKNOWN, text, line};
    }

    advance(); // skip closing single quote '
    return {TokenType::STRING, text, line};
}