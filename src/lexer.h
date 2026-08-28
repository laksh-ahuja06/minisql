// lexer.h
#pragma once
#include "token.h"
#include <vector>
#include <string>

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();

private:
    std::string source;
    int pos = 0;
    int line = 1;

    char peek();
    char advance();
    bool isAtEnd();

    Token makeIdentifierOrKeyword();
    Token makeNumber();
    Token makeString();
};