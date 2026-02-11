#ifndef LEXER_HPP
#define LEXER_HPP

#include "token.hpp"
#include <vector>
#include <string>
#include <map>

class Lexer {
private:
    std::string source;
    size_t position;
    char currentChar;
    std::map<std::string, TokenType> keywords;

    void initKeywords();
    void advance();
    void skipWhitespace();
    Token number();
    Token stringLiteral();
    Token identifier();

public:
    Lexer(const std::string& text);
    std::vector<Token> tokenize();
};

#endif