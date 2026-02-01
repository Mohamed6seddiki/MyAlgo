#include "token.hpp"
#include <cctype>
#include <vector>
#include <string>
#include <iostream>
#include <map>

class Lexer {
private:
    std::string source;
    size_t position;
    char currentChar;
    
    // Keyword mapping
    std::map<std::string, TokenType> keywords;

    void initKeywords() {
        // Program structure
        keywords["algorithm"] = TokenType::ALGORITHM;
        keywords["var"] = TokenType::VAR;
        keywords["begin"] = TokenType::BEGIN;
        keywords["end"] = TokenType::END;
        
        // Data types
        keywords["integer"] = TokenType::INTEGER;
        keywords["real"] = TokenType::REAL;
        keywords["boolean"] = TokenType::BOOLEAN;
        
        // I/O
        keywords["read"] = TokenType::READ;
        keywords["write"] = TokenType::WRITE;
        
        // Control flow
        keywords["if"] = TokenType::IF;
        keywords["then"] = TokenType::THEN;
        keywords["endif"] = TokenType::ENDIF;
        keywords["for"] = TokenType::FOR;
        keywords["from"] = TokenType::FROM;
        keywords["to"] = TokenType::TO;
        keywords["do"] = TokenType::DO;
        keywords["endfor"] = TokenType::ENDFOR;
        keywords["while"] = TokenType::WHILE;
        keywords["endwhile"] = TokenType::ENDWHILE;
        keywords["repeat"] = TokenType::REPEAT;
        keywords["until"] = TokenType::UNTIL;
        keywords["else"] = TokenType::ELSE;
        
        // Boolean literals
        keywords["true"] = TokenType::TRUE;
        keywords["false"] = TokenType::FALSE;
        
        // Logical operators
        keywords["or"] = TokenType::OR;
        keywords["and"] = TokenType::AND;
        keywords["not"] = TokenType::NOT;
        
        // Arithmetic operators
        keywords["DIV"] = TokenType::DIV;
        keywords["MOD"] = TokenType::MOD;
    }

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
        
        // Handle decimal point for real numbers
        if (currentChar == '.') {
            value += currentChar;
            advance();
            while (currentChar != '\0' && std::isdigit(currentChar)) {
                value += currentChar;
                advance();
            }
        }
        
        return Token(TokenType::NUMBER, value);
    }

    Token stringLiteral() {
        std::string value;
        advance(); // Skip the opening quote
        
        while (currentChar != '\0' && currentChar != '"') {
            if (currentChar == '\\') {
                // Handle escape sequences
                advance();
                if (currentChar == '\0') break;
                
                switch (currentChar) {
                    case 'n': value += '\n'; break;
                    case 't': value += '\t'; break;
                    case 'r': value += '\r'; break;
                    case '\\': value += '\\'; break;
                    case '"': value += '"'; break;
                    default: value += currentChar; break;
                }
            } else {
                value += currentChar;
            }
            advance();
        }
        
        if (currentChar == '"') {
            advance(); // Skip the closing quote
            return Token(TokenType::STRING, value);
        } else {
            std::cerr << "Error: Unterminated string literal" << std::endl;
            return Token(TokenType::STRING, value);
        }
    }

    Token identifier() {
        std::string value;
        while (currentChar != '\0' && (std::isalnum(currentChar) || currentChar == '_')) {
            value += currentChar;
            advance();
        }

        // Check if it's a keyword
        if (keywords.find(value) != keywords.end()) {
            return Token(keywords[value], value);
        }

        return Token(TokenType::IDENTIFIER, value);
    }

public:
    Lexer(const std::string& text)
        : source(text), position(0) {
        currentChar = source.empty() ? '\0' : source[0];
        initKeywords();
    }

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;

        while (currentChar != '\0') {
            
            // Skip whitespace
            if (std::isspace(currentChar)) {
                skipWhitespace();
                continue;
            }

            // String literal
            if (currentChar == '"') {
                tokens.push_back(stringLiteral());
                continue;
            }

            // Identifier or Keyword
            if (std::isalpha(currentChar) || currentChar == '_') {
                tokens.push_back(identifier());
                continue;
            }

            // Number
            if (std::isdigit(currentChar)) {
                tokens.push_back(number());
                continue;
            }

            // Colon : or Assignment :=
            if (currentChar == ':') {
                advance();
                if (currentChar == '=') {
                    advance();
                    tokens.emplace_back(TokenType::ASSIGN, ":=");
                } else {
                    tokens.emplace_back(TokenType::COLON, ":");
                }
                continue;
            }

            // Less <, Not equal <>, Less equal <=
            if (currentChar == '<') {
                advance();
                if (currentChar == '>') {
                    advance();
                    tokens.emplace_back(TokenType::NOT_EQUAL, "<>");
                } else if (currentChar == '=') {
                    advance();
                    tokens.emplace_back(TokenType::LESS_EQUAL, "<=");
                } else {
                    tokens.emplace_back(TokenType::LESS, "<");
                }
                continue;
            }

            // Greater >, Greater equal >=
            if (currentChar == '>') {
                advance();
                if (currentChar == '=') {
                    advance();
                    tokens.emplace_back(TokenType::GREATER_EQUAL, ">=");
                } else {
                    tokens.emplace_back(TokenType::GREATER, ">");
                }
                continue;
            }

            // Equal =
            if (currentChar == '=') {
                tokens.emplace_back(TokenType::EQUAL, "=");
                advance();
                continue;
            }

            // Plus +
            if (currentChar == '+') {
                tokens.emplace_back(TokenType::PLUS, "+");
                advance();
                continue;
            }

            // Minus -
            if (currentChar == '-') {
                tokens.emplace_back(TokenType::MINUS, "-");
                advance();
                continue;
            }

            // Multiply *
            if (currentChar == '*') {
                tokens.emplace_back(TokenType::MULTIPLY, "*");
                advance();
                continue;
            }

            // Divide / or comment //
            if (currentChar == '/') {
                advance();
                // Check for comment
                if (currentChar == '/') {
                    // Skip single-line comment
                    while (currentChar != '\0' && currentChar != '\n') {
                        advance();
                    }
                    continue;
                } else {
                    // It's a division operator
                    tokens.emplace_back(TokenType::DIVIDE, "/");
                }
                continue;
            }

            // Left parenthesis (
            if (currentChar == '(') {
                tokens.emplace_back(TokenType::LPAREN, "(");
                advance();
                continue;
            }

            // Right parenthesis )
            if (currentChar == ')') {
                tokens.emplace_back(TokenType::RPAREN, ")");
                advance();
                continue;
            }

            // Comma ,
            if (currentChar == ',') {
                tokens.emplace_back(TokenType::COMMA, ",");
                advance();
                continue;
            }

            // Semicolon ;
            if (currentChar == ';') {
                tokens.emplace_back(TokenType::SEMICOLON, ";");
                advance();
                continue;
            }

            // Period .
            if (currentChar == '.') {
                tokens.emplace_back(TokenType::PERIOD, ".");
                advance();
                continue;
            }

            // Skip extended ASCII/Unicode characters quietly
            if (static_cast<unsigned char>(currentChar) > 127) {
                // Just skip these characters instead of printing error
                advance();
                continue;
            }

            // Unknown character (basic ASCII only)
            std::cerr << "Unknown character: '" << currentChar << "' (ASCII " << (int)currentChar << ")" << std::endl;
            advance();
        }

        tokens.emplace_back(TokenType::END_OF_FILE);
        return tokens;
    }
};