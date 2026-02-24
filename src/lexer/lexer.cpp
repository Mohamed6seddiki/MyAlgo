#include "lexer.hpp"
#include <cctype>
#include <iostream>

void Lexer::initKeywords() {
    keywords["algorithm"] = TokenType::ALGORITHM;
    keywords["structure"] = TokenType::STRUCTURE;
    keywords["endstructure"] = TokenType::ENDSTRUCTURE;
    keywords["var"] = TokenType::VAR;
    keywords["begin"] = TokenType::BEGIN;
    keywords["end"] = TokenType::END;
    
    // Functions & Procedures
    keywords["function"] = TokenType::FUNCTION;
    keywords["procedure"] = TokenType::PROCEDURE;
    keywords["beginfunction"] = TokenType::BEGINFUNCTION;    // NEW
    keywords["beginprocedure"] = TokenType::BEGINPROCEDURE;  // NEW
    keywords["endfunction"] = TokenType::ENDFUNCTION;
    keywords["endprocedure"] = TokenType::ENDPROCEDURE;
    keywords["return"] = TokenType::RETURN;
    
    keywords["integer"] = TokenType::INTEGER;
    keywords["real"] = TokenType::REAL;
    keywords["boolean"] = TokenType::BOOLEAN;
    keywords["string"] = TokenType::STRING_TYPE;
    keywords["array"] = TokenType::ARRAY;
    keywords["of"] = TokenType::OF;
    keywords["read"] = TokenType::READ;
    keywords["write"] = TokenType::WRITE;
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
    keywords["true"] = TokenType::TRUE;
    keywords["false"] = TokenType::FALSE;
    keywords["or"] = TokenType::OR;
    keywords["and"] = TokenType::AND;
    keywords["not"] = TokenType::NOT;
    keywords["div"] = TokenType::DIV;
    keywords["mod"] = TokenType::MOD;
}

void Lexer::advance() {
    position++;
    if (position < source.length())
        currentChar = source[position];
    else
        currentChar = '\0';
}

void Lexer::skipWhitespace() {
    while (currentChar != '\0' && std::isspace(currentChar)) {
        advance();
    }
}

Token Lexer::number() {
    std::string value;
    while (currentChar != '\0' && std::isdigit(currentChar)) {
        value += currentChar;
        advance();
    }
    
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

Token Lexer::stringLiteral() {
    std::string value;
    advance(); // Skip opening quote
    
    while (currentChar != '\0' && currentChar != '"') {
        value += currentChar;
        advance();
    }
    
    if (currentChar == '"') {
        advance(); // Skip closing quote
        return Token(TokenType::STRING, value);
    }
    
    return Token(TokenType::STRING, value);
}

Token Lexer::identifier() {
    std::string value;
    while (currentChar != '\0' && (std::isalnum(currentChar) || currentChar == '_')) {
        value += currentChar;
        advance();
    }

    // Check if it's a keyword (case-insensitive)
    std::string lowerValue;
    for (char c : value) {
        lowerValue += std::tolower(c);
    }
    
    auto it = keywords.find(lowerValue);
    if (it != keywords.end()) {
        return Token(it->second, value);
    }

    return Token(TokenType::IDENTIFIER, value);
}

Lexer::Lexer(const std::string& text) : source(text), position(0) {
    currentChar = source.empty() ? '\0' : source[0];
    initKeywords();
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (currentChar != '\0') {
        if (std::isspace(currentChar)) {
            skipWhitespace();
            continue;
        }

        if (currentChar == '"') {
            tokens.push_back(stringLiteral());
            continue;
        }

        if (std::isalpha(currentChar) || currentChar == '_') {
            tokens.push_back(identifier());
            continue;
        }

        if (std::isdigit(currentChar)) {
            tokens.push_back(number());
            continue;
        }

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

        if (currentChar == '=') {
            tokens.emplace_back(TokenType::EQUAL, "=");
            advance();
            continue;
        }

        if (currentChar == '+') {
            tokens.emplace_back(TokenType::PLUS, "+");
            advance();
            continue;
        }

        if (currentChar == '-') {
            tokens.emplace_back(TokenType::MINUS, "-");
            advance();
            continue;
        }

        if (currentChar == '*') {
            tokens.emplace_back(TokenType::MULTIPLY, "*");
            advance();
            continue;
        }

        if (currentChar == '/') {
            advance();
            if (currentChar == '/') {
                // Single line comment
                while (currentChar != '\0' && currentChar != '\n') {
                    advance();
                }
                continue;
            } else {
                tokens.emplace_back(TokenType::DIVIDE, "/");
            }
            continue;
        }

        if (currentChar == '(') {
            tokens.emplace_back(TokenType::LPAREN, "(");
            advance();
            continue;
        }

        if (currentChar == ')') {
            tokens.emplace_back(TokenType::RPAREN, ")");
            advance();
            continue;
        }

        if (currentChar == '[') {
            tokens.emplace_back(TokenType::LBRACKET, "[");
            advance();
            continue;
        }

        if (currentChar == ']') {
            tokens.emplace_back(TokenType::RBRACKET, "]");
            advance();
            continue;
        }

        if (currentChar == ',') {
            tokens.emplace_back(TokenType::COMMA, ",");
            advance();
            continue;
        }

        if (currentChar == ';') {
            tokens.emplace_back(TokenType::SEMICOLON, ";");
            advance();
            continue;
        }

        if (currentChar == '.') {
            tokens.emplace_back(TokenType::PERIOD, ".");
            advance();
            continue;
        }

        // Handle comments starting with # 
        if (currentChar == '#') {
            while (currentChar != '\0' && currentChar != '\n') {
                advance();
            }
            continue;
        }

        // Skip unknown characters
        advance();
    }

    tokens.emplace_back(TokenType::END_OF_FILE);
    return tokens;
}