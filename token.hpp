#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

enum class TokenType {
    END_OF_FILE,

    // Identifiers & literals
    IDENTIFIER,
    NUMBER,

    // Keywords - Program structure
    ALGORITHM,    // algorithm
    VAR,          // var
    BEGIN,        // begin
    END,          // end

    // Keywords - Data types
    INTEGER,      // integer
    REAL,         // real
    BOOLEAN,      // boolean
    STRING,         // String 
    
    // Keywords - I/O
    READ,         // read
    WRITE,        // write

    // Keywords - Control flow
    IF,           // if
    THEN,         // then
    ENDIF,        // endif
    FOR,          // for
    FROM,         // from
    TO,           // to
    DO,           // do
    ENDFOR,       // endfor
    WHILE,        // while
    ENDWHILE,     // endwhile
    REPEAT,       // repeat
    UNTIL,        // until
    ELSE,         // else

    // Keywords - Boolean literals
    TRUE,         // true
    FALSE,        // false

    // Keywords - Logical operators
    OR,           // or
    AND,          // and
    NOT,          // not

    // Keywords - Arithmetic operators
    DIV,          // DIV (integer division)
    MOD,          // MOD (modulo)

    // Operators - Assignment
    ASSIGN,       // :=

    // Operators - Arithmetic
    PLUS,         // +
    MINUS,        // -
    MULTIPLY,     // *
    DIVIDE,       // /

    // Operators - Relational
    EQUAL,        // =
    NOT_EQUAL,    // <>
    LESS,         // <
    GREATER,      // >
    LESS_EQUAL,   // <=
    GREATER_EQUAL,// >=

    // Symbols
    LPAREN,       // (
    RPAREN,       // )
    COMMA,        // ,
    COLON,        // :
    SEMICOLON,    // ;
    PERIOD        // .
};

struct Token {
    TokenType type;
    std::string value;   

    Token(TokenType t, const std::string& v = "")
        : type(t), value(v) {}
};

#endif