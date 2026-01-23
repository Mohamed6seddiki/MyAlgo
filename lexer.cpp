#include "token.hpp"
#include <cctype>
#include <vector>
#include <string>
#include <iostream>

class Lexer {
private:
    std::string source;
    size_t position;
    char currentChar;

    void advance() {
        position++;
        if (position < source.length())
            currentChar = source[position];
        else
            currentChar = '\0';
    }

    void skipWhitespace() {
        while (currentChar != '\0' && std::isspace(currentChar)) {
            advance();
        }
    }

    Token number() {
        std::string value;
        while (currentChar != '\0' && std::isdigit(currentChar)) {
            value += currentChar;
            advance();
        }
        return Token(TokenType::NUMBER, value);
    }

    Token identifier() {
        std::string value;
        while (currentChar != '\0' && (std::isalnum(currentChar) || currentChar == '_')) {
            value += currentChar;
            advance();
        }

        if (value == "WRITE")
            return Token(TokenType::WRITE);

        return Token(TokenType::IDENTIFIER, value);
    }

public:
    Lexer(const std::string& text)
        : source(text), position(0) {
        currentChar = source.empty() ? '\0' : source[0];
    }

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;

        while (currentChar != '\0') {
            
            if (std::isspace(currentChar)) {
                skipWhitespace();
                continue;
            }

            // Identifier or Keyword
            if (std::isalpha(currentChar)) {
                tokens.push_back(identifier());
                continue;
            }

            // Number
            if (std::isdigit(currentChar)) {
                tokens.push_back(number());
                continue;
            }

            // Operators
            if (currentChar == '+' ) {
                tokens.emplace_back(TokenType::PLUS);
                advance();
                continue;
            }

            if (currentChar == '-' ) {
                tokens.emplace_back(TokenType::MINUS);
                advance();
                continue;
            }

            if (currentChar == '*' ) {
                tokens.emplace_back(TokenType::MULTIPLY);
                advance();
                continue;
            }

            if (currentChar == '/' ) {
                tokens.emplace_back(TokenType::DIVIDE);
                advance();
                continue;
            }

            // Assign <-
            if (currentChar == '<') {
                advance();
                if (currentChar == '-') {
                    advance();
                    tokens.emplace_back(TokenType::ASSIGN, "<-");
                    continue;
                }
            }

            // Parentheses
            if (currentChar == '(') {
                tokens.emplace_back(TokenType::LPAREN);
                advance();
                continue;
            }

            if (currentChar == ')') {
                tokens.emplace_back(TokenType::RPAREN);
                advance();
                continue;
            }

            // erro
            std::cerr << "Caractère inconnu: " << currentChar << std::endl;
            advance();
        }

        tokens.emplace_back(TokenType::END_OF_FILE);
        return tokens;
    }
};


/*
ex:
x <- 10
y <- x + 5
ECRIRE y



resulte:
IDENTIFIER(x)
ASSIGN(<-)
NUMBER(10)
IDENTIFIER(y)
ASSIGN(<-)
IDENTIFIER(x)
PLUS
NUMBER(5)
ECRIRE
IDENTIFIER(y)
EOF


*/