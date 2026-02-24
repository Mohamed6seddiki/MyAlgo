#ifndef SEMANTICANALYZER_HPP
#define SEMANTICANALYZER_HPP

#include "AST.hpp"
#include "SymbolTable.hpp"
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <stack>
#include <map>

class SemanticAnalyzer {
private:
    std::vector<std::string> errors;
    std::stack<SymbolTable> symbolTableStack;
    std::map<std::string, std::string> functionReturnTypes;  // function name -> return type name
    std::map<std::string, std::map<std::string, std::string>> structureDefinitions;
    
    void pushScope() {
        symbolTableStack.push(SymbolTable());
    }
    
    void popScope() {
        if (!symbolTableStack.empty()) {
            symbolTableStack.pop();
        }
    }
    
    SymbolTable& currentScope() {
        if (symbolTableStack.empty()) {
            symbolTableStack.push(SymbolTable());
        }
        return symbolTableStack.top();
    }
    
    void addError(const std::string& error) {
        errors.push_back(error);
    }
    
    Type getTypeFromString(const std::string& typeName) {
        if (typeName == "integer") return Type::INTEGER;
        if (typeName == "real") return Type::REAL;
        if (typeName == "boolean") return Type::BOOLEAN;
        if (typeName == "string") return Type::STRING;
        return Type::UNKNOWN;
    }

    bool isBuiltInTypeName(const std::string& typeName) {
        return typeName == "integer" || typeName == "real" ||
               typeName == "boolean" || typeName == "string";
    }

    bool declareVariableInCurrentScope(const std::string& variableName, const std::string& typeName,
                                       const std::string& declarationContext, bool isParameter = false) {
        Type baseType = getTypeFromString(typeName);
        bool isStructType = structureDefinitions.find(typeName) != structureDefinitions.end();

        if (baseType == Type::UNKNOWN && !isStructType) {
            addError("Unknown type in " + declarationContext + ": " + typeName);
            return false;
        }

        if (!currentScope().declare(variableName, baseType, isParameter)) {
            addError("Duplicate declaration in " + declarationContext + ": " + variableName);
            return false;
        }

        if (isStructType) {
            const auto& fields = structureDefinitions[typeName];
            for (const auto& field : fields) {
                Type fieldType = getTypeFromString(field.second);
                std::string flattenedFieldName = variableName + "." + field.first;
                if (!currentScope().declare(flattenedFieldName, fieldType, isParameter)) {
                    addError("Duplicate field symbol in " + declarationContext + ": " + flattenedFieldName);
                }
            }
        }

        return true;
    }
    
    Type analyzeExpression(const std::shared_ptr<ExpressionNode>& expr) {
        if (!expr) return Type::UNKNOWN;
        
        if (auto num = std::dynamic_pointer_cast<NumberNode>(expr)) {
            // Check if it's a real number
            if (num->value.find('.') != std::string::npos) {
                return Type::REAL;
            }
            return Type::INTEGER;
        }
        else if (auto boolean = std::dynamic_pointer_cast<BooleanNode>(expr)) {
            return Type::BOOLEAN;
        }
        else if (auto str = std::dynamic_pointer_cast<StringNode>(expr)) {
            return Type::STRING;
        }
        else if (auto var = std::dynamic_pointer_cast<VariableNode>(expr)) {
            // Search for variable in all scopes
            Type varType = Type::UNKNOWN;
            
            // Create a copy of the stack to search from top to bottom
            auto tempStack = symbolTableStack;
            std::stack<SymbolTable> searchStack;
            
            while (!tempStack.empty()) {
                searchStack.push(tempStack.top());
                tempStack.pop();
            }
            
            while (!searchStack.empty()) {
                SymbolTable& table = searchStack.top();
                if (table.isDeclared(var->name)) {
                    varType = table.getType(var->name);
                    break;
                }
                searchStack.pop();
            }
            
            if (varType == Type::UNKNOWN) {
                addError("Undeclared variable: " + var->name);
            }
            return varType;
        }
        else if (auto binOp = std::dynamic_pointer_cast<BinaryOpNode>(expr)) {
            Type leftType = analyzeExpression(binOp->left);
            Type rightType = analyzeExpression(binOp->right);
            
            // Arithmetic operators
            if (binOp->op == "+" || binOp->op == "-" || binOp->op == "*" || binOp->op == "/") {
                // String concatenation with +
                if (binOp->op == "+" && leftType == Type::STRING && rightType == Type::STRING) {
                    return Type::STRING;
                }
                // Numeric operations
                if (leftType == Type::INTEGER && rightType == Type::INTEGER) {
                    return Type::INTEGER;
                }
                else if ((leftType == Type::INTEGER || leftType == Type::REAL) &&
                         (rightType == Type::INTEGER || rightType == Type::REAL)) {
                    return Type::REAL;
                }
                else {
                    addError("Type mismatch in arithmetic operation");
                    return Type::ERROR;
                }
            }
            // Comparison operators
            else if (binOp->op == "<" || binOp->op == ">" || binOp->op == "<=" ||
                     binOp->op == ">=" || binOp->op == "=" || binOp->op == "<>") {
                if ((leftType == Type::INTEGER || leftType == Type::REAL) &&
                    (rightType == Type::INTEGER || rightType == Type::REAL)) {
                    return Type::BOOLEAN;
                }
                else if (leftType == Type::BOOLEAN && rightType == Type::BOOLEAN) {
                    return Type::BOOLEAN;
                }
                else if (leftType == Type::STRING && rightType == Type::STRING) {
                    return Type::BOOLEAN;
                }
                else {
                    addError("Type mismatch in comparison operation");
                    return Type::ERROR;
                }
            }
            // Logical operators
            else if (binOp->op == "and" || binOp->op == "or") {
                if (leftType == Type::BOOLEAN && rightType == Type::BOOLEAN) {
                    return Type::BOOLEAN;
                }
                else {
                    addError("Type mismatch in logical operation");
                    return Type::ERROR;
                }
            }
            else if (binOp->op == "DIV" || binOp->op == "MOD") {
                if (leftType == Type::INTEGER && rightType == Type::INTEGER) {
                    return Type::INTEGER;
                }
                else {
                    addError("DIV and MOD require integer operands");
                    return Type::ERROR;
                }
            }
        }
        else if (auto unaryOp = std::dynamic_pointer_cast<UnaryOpNode>(expr)) {
            Type operandType = analyzeExpression(unaryOp->operand);
            
            if (unaryOp->op == "not") {
                if (operandType == Type::BOOLEAN) {
                    return Type::BOOLEAN;
                }
                else {
                    addError("NOT operator requires boolean operand");
                    return Type::ERROR;
                }
            }
            else if (unaryOp->op == "-") {
                if (operandType == Type::INTEGER || operandType == Type::REAL) {
                    return operandType;
                }
                else {
                    addError("Unary minus requires numeric operand");
                    return Type::ERROR;
                }
            }
        }
        else if (auto call = std::dynamic_pointer_cast<CallNode>(expr)) {
            // Look up the function's return type
            auto it = functionReturnTypes.find(call->functionName);
            if (it != functionReturnTypes.end()) {
                return getTypeFromString(it->second);
            } else {
                addError("Unknown function: " + call->functionName);
                return Type::UNKNOWN;
            }
        }
        else if (auto arrayAccess = std::dynamic_pointer_cast<ArrayAccessNode>(expr)) {
            Type indexType = analyzeExpression(arrayAccess->index);
            if (indexType != Type::INTEGER) {
                addError("Array index must be integer: " + arrayAccess->arrayName);
            }

            if (arrayAccess->secondIndex) {
                Type secondIndexType = analyzeExpression(arrayAccess->secondIndex);
                if (secondIndexType != Type::INTEGER) {
                    addError("Matrix index must be integer: " + arrayAccess->arrayName);
                }
            }

            // Array access returns the element type
            Type arrayType = Type::UNKNOWN;
            bool found = false;
            
            // Search for array declaration
            auto tempStack = symbolTableStack;
            std::stack<SymbolTable> searchStack;
            
            while (!tempStack.empty()) {
                searchStack.push(tempStack.top());
                tempStack.pop();
            }
            
            while (!searchStack.empty()) {
                SymbolTable& table = searchStack.top();
                if (table.isDeclared(arrayAccess->arrayName)) {
                    arrayType = table.getType(arrayAccess->arrayName);
                    found = true;
                    break;
                }
                searchStack.pop();
            }
            
            if (!found) {
                addError("Undeclared array: " + arrayAccess->arrayName);
                return Type::UNKNOWN;
            }
            
            return arrayType;
        }
        
        return Type::UNKNOWN;
    }
    
    void analyzeBlock(const std::shared_ptr<BlockNode>& block) {
        if (!block) return;
        
        for (const auto& stmt : block->statements) {
            if (stmt) {
                analyzeStatement(stmt);
            }
        }
    }
    
    void analyzeStatement(const std::shared_ptr<StatementNode>& stmt) {
        if (auto assign = std::dynamic_pointer_cast<AssignmentNode>(stmt)) {
            // Check if variable is declared
            Type varType = Type::UNKNOWN;
            bool found = false;
            
            // Create a copy of the stack to search from top to bottom
            auto tempStack = symbolTableStack;
            std::stack<SymbolTable> searchStack;
            
            while (!tempStack.empty()) {
                searchStack.push(tempStack.top());
                tempStack.pop();
            }
            
            while (!searchStack.empty()) {
                SymbolTable& table = searchStack.top();
                if (table.isDeclared(assign->variableName)) {
                    varType = table.getType(assign->variableName);
                    found = true;
                    break;
                }
                searchStack.pop();
            }
            
            if (!found) {
                addError("Assignment to undeclared variable: " + assign->variableName);
                return;
            }

            if (assign->arrayIndex) {
                Type indexType = analyzeExpression(assign->arrayIndex);
                if (indexType != Type::INTEGER) {
                    addError("Array assignment index must be integer: " + assign->variableName);
                }
            }

            if (assign->matrixIndex) {
                Type matrixIndexType = analyzeExpression(assign->matrixIndex);
                if (matrixIndexType != Type::INTEGER) {
                    addError("Matrix assignment index must be integer: " + assign->variableName);
                }
            }
            
            // Analyze expression
            Type exprType = analyzeExpression(assign->expression);
            
            // Check type compatibility
            bool typeOK = (varType == exprType) || 
                          (varType == Type::REAL && exprType == Type::INTEGER);
            
            if (!typeOK) {
                addError("Type mismatch in assignment to variable: " + assign->variableName + 
                        " (expected " + typeToString(varType) + ", got " + typeToString(exprType) + ")");
            }
            
            // Mark variable as initialized
            // Search from top scope first
            auto tempStack2 = symbolTableStack;
            while (!tempStack2.empty()) {
                SymbolTable& table = tempStack2.top();
                if (table.isDeclared(assign->variableName)) {
                    table.setInitialized(assign->variableName);
                    break;
                }
                tempStack2.pop();
            }
        }
        else if (auto ifStmt = std::dynamic_pointer_cast<IfStatementNode>(stmt)) {
            // Check condition is boolean
            Type condType = analyzeExpression(ifStmt->condition);
            if (condType != Type::BOOLEAN) {
                addError("IF condition must be boolean");
            }
            
            // Analyze then and else blocks
            analyzeBlock(ifStmt->thenBlock);
            if (ifStmt->elseBlock) {
                analyzeBlock(ifStmt->elseBlock);
            }
        }
        else if (auto forStmt = std::dynamic_pointer_cast<ForStatementNode>(stmt)) {
            // Check loop variable is integer
            Type startType = analyzeExpression(forStmt->startExpr);
            Type endType = analyzeExpression(forStmt->endExpr);
            
            if (startType != Type::INTEGER) {
                addError("FOR loop start expression must be integer");
            }
            if (endType != Type::INTEGER) {
                addError("FOR loop end expression must be integer");
            }
            
            // Create new scope for loop
            pushScope();
            
            // Declare loop variable in loop scope
            currentScope().declare(forStmt->variableName, Type::INTEGER);
            currentScope().setInitialized(forStmt->variableName);
            
            // Analyze loop body
            analyzeBlock(forStmt->body);
            
            popScope();
        }
        else if (auto whileStmt = std::dynamic_pointer_cast<WhileStatementNode>(stmt)) {
            Type condType = analyzeExpression(whileStmt->condition);
            if (condType != Type::BOOLEAN) {
                addError("WHILE condition must be boolean");
            }
            
            analyzeBlock(whileStmt->body);
        }
        else if (auto repeatStmt = std::dynamic_pointer_cast<RepeatStatementNode>(stmt)) {
            analyzeBlock(repeatStmt->body);
            
            Type condType = analyzeExpression(repeatStmt->condition);
            if (condType != Type::BOOLEAN) {
                addError("UNTIL condition must be boolean");
            }
        }
        else if (auto readStmt = std::dynamic_pointer_cast<ReadStatementNode>(stmt)) {
            // Check if variable is declared
            bool found = false;
            
            auto tempStack = symbolTableStack;
            std::stack<SymbolTable> searchStack;
            
            while (!tempStack.empty()) {
                searchStack.push(tempStack.top());
                tempStack.pop();
            }
            
            while (!searchStack.empty()) {
                SymbolTable& table = searchStack.top();
                if (table.isDeclared(readStmt->variableName)) {
                    found = true;
                    // Mark variable as initialized
                    table.setInitialized(readStmt->variableName);
                    break;
                }
                searchStack.pop();
            }
            
            if (!found) {
                addError("READ of undeclared variable: " + readStmt->variableName);
            }
        }
        else if (auto writeStmt = std::dynamic_pointer_cast<WriteStatementNode>(stmt)) {
            // Just analyze the expression
            analyzeExpression(writeStmt->expression);
        }
        else if (auto returnStmt = std::dynamic_pointer_cast<ReturnStatementNode>(stmt)) {
            // Check return type against function return type
            // This would need to track current function context
            if (returnStmt->expression) {
                analyzeExpression(returnStmt->expression);
            }
        }
    }
    
    void analyzeFunction(const std::shared_ptr<FunctionNode>& func) {
        // Store the function's return type for later lookup
        functionReturnTypes[func->name] = func->returnType;
        
        // Create new scope for function
        pushScope();
        
        // Declare parameters
        for (const auto& param : func->parameters) {
            declareVariableInCurrentScope(param->name, param->typeName,
                                          "function " + func->name + " parameter", true);
            // Parameters are considered initialized
            currentScope().setInitialized(param->name);
        }
        
        // Declare local variables
        for (const auto& varDecl : func->variables) {
            for (const auto& name : varDecl->names) {
                declareVariableInCurrentScope(name, varDecl->typeName,
                                              "function " + func->name + " local variable");
            }
        }
        
        // Analyze function body
        analyzeBlock(func->body);
        
        popScope();
    }
    
    void analyzeProcedure(const std::shared_ptr<ProcedureNode>& proc) {
        // Create new scope for procedure
        pushScope();
        
        // Declare parameters
        for (const auto& param : proc->parameters) {
            declareVariableInCurrentScope(param->name, param->typeName,
                                          "procedure " + proc->name + " parameter", true);
            // Parameters are considered initialized
            currentScope().setInitialized(param->name);
        }
        
        // Declare local variables
        for (const auto& varDecl : proc->variables) {
            for (const auto& name : varDecl->names) {
                declareVariableInCurrentScope(name, varDecl->typeName,
                                              "procedure " + proc->name + " local variable");
            }
        }
        
        // Analyze procedure body
        analyzeBlock(proc->body);
        
        popScope();
    }
    
    void analyzeAlgorithm(const std::shared_ptr<AlgorithmNode>& algo) {
        // Create global scope
        pushScope();

        // Register structure definitions
        for (const auto& structure : algo->structures) {
            if (structureDefinitions.find(structure->name) != structureDefinitions.end()) {
                addError("Duplicate structure declaration: " + structure->name);
                continue;
            }

            std::map<std::string, std::string> fieldMap;
            for (const auto& field : structure->fields) {
                if (!isBuiltInTypeName(field->typeName)) {
                    addError("Unsupported structure field type in " + structure->name + ": " + field->typeName);
                    continue;
                }

                if (fieldMap.find(field->name) != fieldMap.end()) {
                    addError("Duplicate field '" + field->name + "' in structure " + structure->name);
                    continue;
                }

                fieldMap[field->name] = field->typeName;
            }
            structureDefinitions[structure->name] = fieldMap;
        }
        
        // Declare all global variables
        for (const auto& varDecl : algo->variables) {
            for (const auto& name : varDecl->names) {
                declareVariableInCurrentScope(name, varDecl->typeName, "global variable");
            }
        }
        
        // Analyze functions and procedures
        for (const auto& func : algo->functions) {
            analyzeFunction(func);
        }
        
        for (const auto& proc : algo->procedures) {
            analyzeProcedure(proc);
        }
        
        // Analyze the main block
        analyzeBlock(algo->body);
        
        popScope();
    }
    
public:
    SemanticAnalyzer() {}
    
    bool analyze(const std::shared_ptr<ASTNode>& ast) {
        errors.clear();
        functionReturnTypes.clear();
        structureDefinitions.clear();
        
        // Clear any existing symbol tables
        while (!symbolTableStack.empty()) {
            symbolTableStack.pop();
        }
        
        if (auto algo = std::dynamic_pointer_cast<AlgorithmNode>(ast)) {
            analyzeAlgorithm(algo);
        } else {
            addError("AST root is not an AlgorithmNode");
        }
        
        return errors.empty();
    }
    
    void printErrors() const {
        if (errors.empty()) {
            //std::cout << "No semantic errors found." << std::endl;
        } else {
            std::cout << "Semantic errors found (" << errors.size() << "):" << std::endl;
            for (size_t i = 0; i < errors.size(); i++) {
                std::cout << "  " << (i + 1) << ". " << errors[i] << std::endl;
            }
        }
    }
    
    void printSymbolTables() {
        std::cout << "\n=== SYMBOL TABLES ===" << std::endl;
        auto tempStack = symbolTableStack;
        std::stack<SymbolTable> printStack;
        
        while (!tempStack.empty()) {
            printStack.push(tempStack.top());
            tempStack.pop();
        }
        
        int scopeNum = 1;
        while (!printStack.empty()) {
            std::cout << "\nScope " << scopeNum++ << ":" << std::endl;
            printStack.top().print();
            printStack.pop();
        }
    }
};

#endif