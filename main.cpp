// main.cpp - MyAlgo Compiler
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <map>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <cctype>

#include "token.hpp"
#include "AST.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "SemanticAnalyzer.hpp"
#include "CodeGenerator.hpp"

using namespace std;

string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::STRING: return "STRING";
        case TokenType::ALGORITHM: return "ALGORITHM";
        case TokenType::VAR: return "VAR";
        case TokenType::BEGIN: return "BEGIN";
        case TokenType::END: return "END";
        case TokenType::FUNCTION: return "FUNCTION";
        case TokenType::PROCEDURE: return "PROCEDURE";
        case TokenType::BEGINFUNCTION: return "BEGINFUNCTION";
        case TokenType::BEGINPROCEDURE: return "BEGINPROCEDURE";
        case TokenType::ENDFUNCTION: return "ENDFUNCTION";
        case TokenType::ENDPROCEDURE: return "ENDPROCEDURE";
        case TokenType::RETURN: return "RETURN";
        case TokenType::INTEGER: return "INTEGER";
        case TokenType::REAL: return "REAL";
        case TokenType::BOOLEAN: return "BOOLEAN";
        case TokenType::STRING_TYPE: return "STRING_TYPE";
        case TokenType::ARRAY: return "ARRAY";
        case TokenType::OF: return "OF";
        case TokenType::READ: return "READ";
        case TokenType::WRITE: return "WRITE";
        case TokenType::IF: return "IF";
        case TokenType::THEN: return "THEN";
        case TokenType::ENDIF: return "ENDIF";
        case TokenType::FOR: return "FOR";
        case TokenType::FROM: return "FROM";
        case TokenType::TO: return "TO";
        case TokenType::DO: return "DO";
        case TokenType::ENDFOR: return "ENDFOR";
        case TokenType::WHILE: return "WHILE";
        case TokenType::ENDWHILE: return "ENDWHILE";
        case TokenType::REPEAT: return "REPEAT";
        case TokenType::UNTIL: return "UNTIL";
        case TokenType::ELSE: return "ELSE";
        case TokenType::TRUE: return "TRUE";
        case TokenType::FALSE: return "FALSE";
        case TokenType::OR: return "OR";
        case TokenType::AND: return "AND";
        case TokenType::NOT: return "NOT";
        case TokenType::DIV: return "DIV";
        case TokenType::MOD: return "MOD";
        case TokenType::ASSIGN: return "ASSIGN";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::MULTIPLY: return "MULTIPLY";
        case TokenType::DIVIDE: return "DIVIDE";
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::NOT_EQUAL: return "NOT_EQUAL";
        case TokenType::LESS: return "LESS";
        case TokenType::GREATER: return "GREATER";
        case TokenType::LESS_EQUAL: return "LESS_EQUAL";
        case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";
        case TokenType::LBRACKET: return "LBRACKET";
        case TokenType::RBRACKET: return "RBRACKET";
        case TokenType::COMMA: return "COMMA";
        case TokenType::COLON: return "COLON";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::PERIOD: return "PERIOD";
        case TokenType::END_OF_FILE: return "EOF";
        default: return "UNKNOWN";
    }
}

int main(int argc, char* argv[]) {
    cout << "=== MyAlgo Compiler ===" << endl;
    
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <filename.algo>" << endl;
        cerr << "Example: " << argv[0] << " test.algo" << endl;
        return 1;
    }

    ifstream file(argv[1]);
    if (!file) {
        cerr << "Error: Cannot open file '" << argv[1] << "'" << endl;
        return 1;
    }

    stringstream buffer;
    buffer << file.rdbuf();
    string sourceCode = buffer.str();
    
    cout << "Source code loaded (" << sourceCode.length() << " characters)" << endl;

    try {
        // Lexical analysis
        cout << "\n1. Running lexical analysis..." << endl;
        Lexer lexer(sourceCode);
        vector<Token> tokens = lexer.tokenize();
        cout << "   Generated " << tokens.size() << " tokens" << endl;

        // Optional: Print tokens for debugging
        // cout << "\nTokens:" << endl;
        // for (const auto& token : tokens) {
        //     cout << "   " << tokenTypeToString(token.type) << " : '" << token.value << "'" << endl;
        // }

        // Syntactic analysis
        cout << "\n2. Running syntactic analysis..." << endl;
        Parser parser(tokens);
        shared_ptr<ASTNode> ast = parser.parse();
        
        if (!ast) {
            cerr << "Parsing failed!" << endl;
            return 1;
        }
        cout << "   Parsing successful!" << endl;

        // Optional: Print AST for debugging
        // cout << "\nAST Structure:" << endl;
        // parser.printAST(ast);

        // Semantic analysis
        cout << "\n3. Running semantic analysis..." << endl;
        SemanticAnalyzer semanticAnalyzer;
        bool semanticOK = semanticAnalyzer.analyze(ast);
        semanticAnalyzer.printErrors();
        
        if (!semanticOK) {
            cerr << "Semantic analysis failed!" << endl;
            return 1;
        }
        cout << "   Semantic analysis successful!" << endl;

        // Optional: Print symbol tables
        // semanticAnalyzer.printSymbolTables();

        // Code generation
        cout << "\n4. Running code generation..." << endl;
        string inputFilename = argv[1];
        string outputFilename;
        
        size_t dotPos = inputFilename.find_last_of(".");
        if (dotPos != string::npos) {
            outputFilename = inputFilename.substr(0, dotPos) + ".cpp";
        } else {
            outputFilename = inputFilename + ".cpp";
        }
        
        CodeGenerator codeGenerator(ast, outputFilename);
        bool generationOK = codeGenerator.generate();
        
        if (!generationOK) {
            cerr << "Code generation failed!" << endl;
            return 1;
        }
        
        cout << "\n=== COMPILATION SUCCESS ===" << endl;
        cout << "Output file: " << outputFilename << endl;
        cout << "==========================" << endl;

        // Compile and run the generated C++ code
        cout << "\n=== Compiling and running generated code ===" << endl;
        
        string cppFile = outputFilename;
        string baseName = outputFilename.substr(0, outputFilename.find_last_of("."));
        string exeFile = baseName + ".exe";
        
        // Compile the C++ code
        string compileCmd = "g++ -std=c++11 \"" + cppFile + "\" -o \"" + exeFile + "\"";
        cout << "Compiling: " << compileCmd << endl;
        
        int compileResult = system(compileCmd.c_str());
        if (compileResult != 0) {
            cerr << "Failed to compile C++ code! Make sure g++ is in your PATH." << endl;
            cerr << "You can compile it manually with: " << compileCmd << endl;
            return 1;
        }
        
        // Run the executable
        cout << "\n=== Program Output ===" << endl;
        string runCmd;
        #ifdef _WIN32
            runCmd = "\"" + exeFile + "\"";
        #else
            runCmd = "./\"" + baseName + "\"";
        #endif
        
        int runResult = system(runCmd.c_str());
        
        if (runResult != 0) {
            cerr << "Program exited with error code: " << runResult << endl;
        }
        
        cout << "\n========================" << endl;
        
    } catch (const runtime_error& e) {
        cerr << "\n=== COMPILATION ERROR ===" << endl;
        cerr << "Error: " << e.what() << endl;
        cerr << "==========================" << endl;
        return 1;
    }

    return 0;
}