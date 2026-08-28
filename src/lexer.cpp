// lexer.cpp
#include "lexer.h"
#include <cctype>
#include <unordered_map>

Lexer::Lexer(const std::string& src) : source(src) {}

bool Lexer::isAtEnd() { return pos >= (int)source.size(); }
char Lexer::peek() { return isAtEnd() ? '\0' : source[pos]; }
char Lexer::advance() { return source[pos++]; }

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (!isAtEnd()) {
        char c = peek();

        if (c == ' ' || c == '\t' || c == '\r') { advance(); continue; }
        if (c == '\n') { line++; advance(); continue; }

        if (isalpha(c)) { tokens.push_back(makeIdentifierOrKeyword()); continue; }
        if (isdigit(c)) { tokens.push_back(makeNumber()); continue; }
        if (c == '\'') { tokens.push_back(makeString()); continue; }

        advance(); // consume the symbol below
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

Token Lexer::makeIdentifierOrKeyword() {
    int start = pos;
    while (!isAtEnd() && (isalnum(peek()) || peek() == '_')) advance();
    std::string text = source.substr(start, pos - start);

    static std::unordered_map<std::string, TokenType> keywords = {
        {"SELECT", TokenType::SELECT}, {"FROM", TokenType::FROM},
        {"WHERE", TokenType::WHERE}, {"ORDER", TokenType::ORDER},
        {"BY", TokenType::BY}, {"AND", TokenType::AND}, {"OR", TokenType::OR},
        {"CREATE", TokenType::CREATE}, {"TABLE", TokenType::TABLE},
        {"INSERT", TokenType::INSERT}, {"INTO", TokenType::INTO},
        {"VALUES", TokenType::VALUES}
    };

    std::string upper = text;
    for (auto& ch : upper) ch = toupper(ch);

    if (keywords.count(upper)) return {keywords[upper], text, line};
    return {TokenType::IDENTIFIER, text, line};
}

Token Lexer::makeNumber() {
    int start = pos;
    while (!isAtEnd() && (isdigit(peek()) || peek() == '.')) advance();
    return {TokenType::NUMBER, source.substr(start, pos - start), line};
}

Token Lexer::makeString() {
    advance(); // skip opening '
    int start = pos;
    while (!isAtEnd() && peek() != '\'') advance();
    std::string text = source.substr(start, pos - start);
    advance(); // skip closing '
    return {TokenType::STRING, text, line};
}