#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "token.hpp"
#include "lexer.cpp"   

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::IDENTIFIER:   return "IDENTIFIER";
        case TokenType::NUMBER:       return "NUMBER";
        case TokenType::WRITE:       return "WRITE";
        case TokenType::ASSIGN:       return "ASSIGN";
        case TokenType::PLUS:         return "PLUS";
        case TokenType::MINUS:        return "MINUS";
        case TokenType::MULTIPLY:     return "MULTIPLY";
        case TokenType::DIVIDE:       return "DIVIDE";
        case TokenType::LPAREN:       return "LPAREN";
        case TokenType::RPAREN:       return "RPAREN";
        case TokenType::END_OF_FILE:  return "EOF";
        default:                      return "UNKNOWN";
    }
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "Usage: ./myalgo <fichier.algo>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Erreur: impossible d'ouvrir le fichier." << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string sourceCode = buffer.str();

    // إنشاء Lexer
    Lexer lexer(sourceCode);
    std::vector<Token> tokens = lexer.tokenize();

    for (const auto& token : tokens) {
        std::cout << tokenTypeToString(token.type);

        if (!token.value.empty())
            std::cout << " (" << token.value << ")";

        std::cout << std::endl;
    }

    return 0;
}
