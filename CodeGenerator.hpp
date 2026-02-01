// CodeGenerator.hpp
#ifndef CODEGENERATOR_HPP
#define CODEGENERATOR_HPP

#include "AST.hpp"
#include "SymbolTable.hpp"
#include "SemanticAnalyzer.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <vector>
#include <map>

class CodeGenerator {
private:
    std::shared_ptr<ASTNode> ast;
    std::ofstream outputFile;
    std::string outputFilename;
    int indentLevel;
    bool generateMain;
    
    std::string indent() const {
        return std::string(indentLevel * 4, ' ');
    }
    
    std::string cppType(Type type) {
        switch (type) {
            case Type::INTEGER: return "int";
            case Type::REAL: return "double";
            case Type::BOOLEAN: return "bool";
            default: return "int"; // Default to int
        }
    }
    
    std::string cppBool(bool value) {
        return value ? "true" : "false";
    }
    
    std::string sanitizeIdentifier(const std::string& name) {
        // In C++, we can use the same identifiers as in Pascal
        // But we need to avoid C++ keywords
        static std::map<std::string, bool> cppKeywords = {
            {"alignas", true}, {"alignof", true}, {"and", true}, {"and_eq", true},
            {"asm", true}, {"auto", true}, {"bitand", true}, {"bitor", true},
            {"bool", true}, {"break", true}, {"case", true}, {"catch", true},
            {"char", true}, {"char8_t", true}, {"char16_t", true}, {"char32_t", true},
            {"class", true}, {"compl", true}, {"concept", true}, {"const", true},
            {"consteval", true}, {"constexpr", true}, {"const_cast", true},
            {"continue", true}, {"co_await", true}, {"co_return", true}, {"co_yield", true},
            {"decltype", true}, {"default", true}, {"delete", true}, {"do", true},
            {"double", true}, {"dynamic_cast", true}, {"else", true}, {"enum", true},
            {"explicit", true}, {"export", true}, {"extern", true}, {"false", true},
            {"float", true}, {"for", true}, {"friend", true}, {"goto", true},
            {"if", true}, {"inline", true}, {"int", true}, {"long", true},
            {"mutable", true}, {"namespace", true}, {"new", true}, {"noexcept", true},
            {"not", true}, {"not_eq", true}, {"nullptr", true}, {"operator", true},
            {"or", true}, {"or_eq", true}, {"private", true}, {"protected", true},
            {"public", true}, {"register", true}, {"reinterpret_cast", true},
            {"requires", true}, {"return", true}, {"short", true}, {"signed", true},
            {"sizeof", true}, {"static", true}, {"static_assert", true},
            {"static_cast", true}, {"struct", true}, {"switch", true}, {"template", true},
            {"this", true}, {"thread_local", true}, {"throw", true}, {"true", true},
            {"try", true}, {"typedef", true}, {"typeid", true}, {"typename", true},
            {"union", true}, {"unsigned", true}, {"using", true}, {"virtual", true},
            {"void", true}, {"volatile", true}, {"wchar_t", true}, {"while", true},
            {"xor", true}, {"xor_eq", true}
        };
        
        if (cppKeywords.find(name) != cppKeywords.end()) {
            return name + "_"; // Append underscore to avoid keyword conflict
        }
        return name;
    }
    
    void generateIncludes() {
        outputFile << "#include <iostream>\n";
        outputFile << "#include <cmath>\n"; // For math operations
        outputFile << "#include <string>\n"; // For string support
        outputFile << "using namespace std;\n\n";
    }
    
    void generateExpression(const std::shared_ptr<ExpressionNode>& expr) {
        if (auto num = std::dynamic_pointer_cast<NumberNode>(expr)) {
            // Handle real numbers (with decimal point)
            if (num->value.find('.') != std::string::npos) {
                outputFile << num->value;
            } else {
                outputFile << num->value;
            }
        }
        else if (auto str = std::dynamic_pointer_cast<StringNode>(expr)) {
            outputFile << "\"" << str->value << "\"";
        }
        else if (auto boolean = std::dynamic_pointer_cast<BooleanNode>(expr)) {
            outputFile << cppBool(boolean->value);
        }
        else if (auto var = std::dynamic_pointer_cast<VariableNode>(expr)) {
            outputFile << sanitizeIdentifier(var->name);
        }
        else if (auto binOp = std::dynamic_pointer_cast<BinaryOpNode>(expr)) {
            // Handle special operators
            std::string op = binOp->op;
            if (op == "DIV") {
                outputFile << "(";
                generateExpression(binOp->left);
                outputFile << " / ";
                generateExpression(binOp->right);
                outputFile << ")"; // Integer division in C++ is just /
            }
            else if (op == "MOD") {
                outputFile << "(";
                generateExpression(binOp->left);
                outputFile << " % ";
                generateExpression(binOp->right);
                outputFile << ")";
            }
            else if (op == "=") {  // ADD THIS CASE
            outputFile << "(";
            generateExpression(binOp->left);
            outputFile << " == ";  // Change = to == for comparison
            generateExpression(binOp->right);
            outputFile << ")";
            }
            else if (op == "<>") {
                outputFile << "(";
                generateExpression(binOp->left);
                outputFile << " != ";
                generateExpression(binOp->right);
                outputFile << ")";
            }
            else if (op == "and") {
                outputFile << "(";
                generateExpression(binOp->left);
                outputFile << " && ";
                generateExpression(binOp->right);
                outputFile << ")";
            }
            else if (op == "or") {
                outputFile << "(";
                generateExpression(binOp->left);
                outputFile << " || ";
                generateExpression(binOp->right);
                outputFile << ")";
            }
            else {
                outputFile << "(";
                generateExpression(binOp->left);
                outputFile << " " << op << " ";
                generateExpression(binOp->right);
                outputFile << ")";
            }
        }
        else if (auto unaryOp = std::dynamic_pointer_cast<UnaryOpNode>(expr)) {
            if (unaryOp->op == "not") {
                outputFile << "!(";
                generateExpression(unaryOp->operand);
                outputFile << ")";
            }
            else {
                outputFile << unaryOp->op;
                generateExpression(unaryOp->operand);
            }
        }
        else if (auto call = std::dynamic_pointer_cast<CallNode>(expr)) {
            if (call->functionName == "write") {
                // write becomes cout
                outputFile << "cout << ";
                if (!call->arguments.empty()) {
                    generateExpression(call->arguments[0]);
                }
            }
            else {
                outputFile << sanitizeIdentifier(call->functionName) << "(";
                for (size_t i = 0; i < call->arguments.size(); i++) {
                    if (i > 0) outputFile << ", ";
                    generateExpression(call->arguments[i]);
                }
                outputFile << ")";
            }
        }
    }
    
    void generateStatement(const std::shared_ptr<StatementNode>& stmt) {
        if (auto assign = std::dynamic_pointer_cast<AssignmentNode>(stmt)) {
            outputFile << indent() << sanitizeIdentifier(assign->variableName) << " = ";
            generateExpression(assign->expression);
            outputFile << ";\n";
        }
        else if (auto ifStmt = std::dynamic_pointer_cast<IfStatementNode>(stmt)) {
            outputFile << indent() << "if (";
            generateExpression(ifStmt->condition);
            outputFile << ") {\n";
            
            indentLevel++;
            generateBlock(ifStmt->thenBlock);
            indentLevel--;
            
            if (ifStmt->elseBlock && !ifStmt->elseBlock->statements.empty()) {
                outputFile << indent() << "} else {\n";
                indentLevel++;
                generateBlock(ifStmt->elseBlock);
                indentLevel--;
            }
            
            outputFile << indent() << "}\n";
        }
        else if (auto forStmt = std::dynamic_pointer_cast<ForStatementNode>(stmt)) {
            outputFile << indent() << "for (int " << sanitizeIdentifier(forStmt->variableName) << " = ";
            generateExpression(forStmt->startExpr);
            outputFile << "; " << sanitizeIdentifier(forStmt->variableName) << " <= ";
            generateExpression(forStmt->endExpr);
            outputFile << "; " << sanitizeIdentifier(forStmt->variableName) << "++) {\n";
            
            indentLevel++;
            generateBlock(forStmt->body);
            indentLevel--;
            
            outputFile << indent() << "}\n";
        }
        else if (auto whileStmt = std::dynamic_pointer_cast<WhileStatementNode>(stmt)) {
            outputFile << indent() << "while (";
            generateExpression(whileStmt->condition);
            outputFile << ") {\n";
            
            indentLevel++;
            generateBlock(whileStmt->body);
            indentLevel--;
            
            outputFile << indent() << "}\n";
        }
        else if (auto repeatStmt = std::dynamic_pointer_cast<RepeatStatementNode>(stmt)) {
            outputFile << indent() << "do {\n";
            
            indentLevel++;
            generateBlock(repeatStmt->body);
            indentLevel--;
            
            outputFile << indent() << "} while (!(";
            generateExpression(repeatStmt->condition);
            outputFile << "));\n";
        }
        else if (auto readStmt = std::dynamic_pointer_cast<ReadStatementNode>(stmt)) {
            outputFile << indent() << "cin >> " << sanitizeIdentifier(readStmt->variableName) << ";\n";
        }
        else if (auto writeStmt = std::dynamic_pointer_cast<WriteStatementNode>(stmt)) {
            outputFile << indent() << "cout << ";
            generateExpression(writeStmt->expression);
            outputFile << " << endl;\n";
        }
    }
    
    void generateBlock(const std::shared_ptr<BlockNode>& block) {
        if (!block) return;
        
        for (const auto& stmt : block->statements) {
            if (stmt) {
                generateStatement(stmt);
            }
        }
    }
    
    void generateVariableDeclarations(const std::vector<std::shared_ptr<VarDeclNode>>& variables) {
        for (const auto& varDecl : variables) {
            // Determine C++ type from Pascal type
            std::string cppTypeStr;
            if (varDecl->typeName == "integer") cppTypeStr = "int";
            else if (varDecl->typeName == "real") cppTypeStr = "double";
            else if (varDecl->typeName == "boolean") cppTypeStr = "bool";
            else cppTypeStr = "int"; // Default
            
            outputFile << indent() << cppTypeStr << " ";
            for (size_t i = 0; i < varDecl->names.size(); i++) {
                if (i > 0) outputFile << ", ";
                outputFile << sanitizeIdentifier(varDecl->names[i]);
            }
            outputFile << ";\n";
        }
    }
    
    void generateAlgorithm(const std::shared_ptr<AlgorithmNode>& algo) {
        outputFile << "// Generated C++ code from Pascal-like algorithm: " << algo->name << "\n";
        outputFile << "// Original algorithm: " << algo->name << "\n\n";
        
        generateIncludes();
        
        if (generateMain) {
            outputFile << "int main() {\n";
            indentLevel++;
        }
        
        // Generate variable declarations
        generateVariableDeclarations(algo->variables);
        outputFile << "\n";
        
        // Generate statements
        generateBlock(algo->body);
        
        if (generateMain) {
            outputFile << "\n" << indent() << "return 0;\n";
            indentLevel--;
            outputFile << "}\n";
        }
    }
    
public:
    CodeGenerator(const std::shared_ptr<ASTNode>& ast, 
                  const std::string& outputFilename = "output.cpp",
                  bool generateMain = true)
        : ast(ast), outputFilename(outputFilename), 
          indentLevel(0), generateMain(generateMain) {}
    
    bool generate() {
        outputFile.open(outputFilename);
        if (!outputFile.is_open()) {
            std::cerr << "Error: Could not open output file " << outputFilename << std::endl;
            return false;
        }
        
        try {
            if (auto algo = std::dynamic_pointer_cast<AlgorithmNode>(ast)) {
                generateAlgorithm(algo);
            } else {
                std::cerr << "Error: AST root is not an AlgorithmNode" << std::endl;
                return false;
            }
            
            outputFile.close();
            std::cout << "C++ code generated successfully: " << outputFilename << std::endl;
            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Error during code generation: " << e.what() << std::endl;
            outputFile.close();
            return false;
        }
    }
    
    std::string getOutputFilename() const {
        return outputFilename;
    }
};

#endif