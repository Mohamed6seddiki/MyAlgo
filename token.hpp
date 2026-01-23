#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

enum class TokenType {

    END_OF_FILE,

    // Identifiers & literals
    IDENTIFIER,
    NUMBER,

    // Keywords
    WRITE,       // ECRIRE

    // Operators
    ASSIGN,       // <-
    PLUS,         // +
    MINUS,        // -
    MULTIPLY,     // *
    DIVIDE,       // /

    // Symbols
    LPAREN,       // (
    RPAREN        // )
};

struct Token {
    TokenType type;
    std::string value;   

    Token(TokenType t, const std::string& v = "")
        : type(t), value(v) {}
};

#endif

//x <- 10

/*
Token(IDENTIFIER, "x")
Token(ASSIGN, "<-")
Token(NUMBER, "10")

*/