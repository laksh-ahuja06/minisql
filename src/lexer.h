// lexer.h - Lexical analyzer class declaration for MiniSQL.
#pragma once
#include "token.h"
#include <vector>
#include <string>

/**
 * @brief Lexical analyzer that breaks raw SQL input text into a sequence of Tokens.
 */
class Lexer {
public:
    /**
     * @brief Constructs a Lexer with the given raw SQL source string.
     * @param source Raw SQL string to tokenize.
     */
    Lexer(const std::string& source);

    /**
     * @brief Scans the input string and produces a list of Token objects.
     * @return std::vector<Token> Stream of tokens ending with TokenType::END_OF_FILE.
     */
    std::vector<Token> tokenize();

private:
    std::string source; ///< Raw source code string
    int pos = 0;        ///< Current character offset in source
    int line = 1;       ///< Current 1-based line number for tracking error context

    /// Returns the character at the current position without advancing.
    char peek();

    /// Returns the character at the current position and advances pos by 1.
    char advance();

    /// Returns true if pos has reached or passed the end of the source.
    bool isAtEnd();

    /// Helper to construct an identifier or keyword token starting at pos.
    Token makeIdentifierOrKeyword();

    /// Helper to construct a numeric literal token (integer or decimal).
    Token makeNumber();

    /// Helper to construct a single-quoted string literal token.
    Token makeString();
};