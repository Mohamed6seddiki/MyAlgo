// AST.hpp
#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <vector>
#include <memory>

// Forward declarations
class ASTNode;
class AlgorithmNode;
class VarDeclNode;
class StatementNode;
class BlockNode;
class AssignmentNode;
class IfStatementNode;
class ForStatementNode;
class WhileStatementNode;
class RepeatStatementNode;
class ReadStatementNode;
class WriteStatementNode;
class ExpressionNode;
class BinaryOpNode;
class UnaryOpNode;
class VariableNode;
class NumberNode;
class BooleanNode;
class StringNode;  // ADDED
class CallNode;

// Base class for all AST nodes
class ASTNode {
public:
    virtual ~ASTNode() = default;
};

// Algorithm definition
class AlgorithmNode : public ASTNode {
public:
    std::string name;
    std::vector<std::shared_ptr<VarDeclNode>> variables;
    std::shared_ptr<BlockNode> body;
    
    AlgorithmNode(const std::string& name, 
                  const std::vector<std::shared_ptr<VarDeclNode>>& variables,
                  const std::shared_ptr<BlockNode>& body)
        : name(name), variables(variables), body(body) {}
};

// Variable declaration
class VarDeclNode : public ASTNode {
public:
    std::vector<std::string> names;  // Could declare multiple variables at once
    std::string typeName;
    
    VarDeclNode(const std::vector<std::string>& names, const std::string& typeName)
        : names(names), typeName(typeName) {}
};

// Base class for statements
class StatementNode : public ASTNode {};

// Block of statements
class BlockNode : public StatementNode {
public:
    std::vector<std::shared_ptr<StatementNode>> statements;
    
    BlockNode(const std::vector<std::shared_ptr<StatementNode>>& statements)
        : statements(statements) {}
};

// Assignment statement
class AssignmentNode : public StatementNode {
public:
    std::string variableName;
    std::shared_ptr<ExpressionNode> expression;
    
    AssignmentNode(const std::string& variableName, 
                   const std::shared_ptr<ExpressionNode>& expression)
        : variableName(variableName), expression(expression) {}
};

// If statement
class IfStatementNode : public StatementNode {
public:
    std::shared_ptr<ExpressionNode> condition;
    std::shared_ptr<BlockNode> thenBlock;
    std::shared_ptr<BlockNode> elseBlock;  // Can be null
    
    IfStatementNode(const std::shared_ptr<ExpressionNode>& condition,
                    const std::shared_ptr<BlockNode>& thenBlock,
                    const std::shared_ptr<BlockNode>& elseBlock = nullptr)
        : condition(condition), thenBlock(thenBlock), elseBlock(elseBlock) {}
};

// For loop
class ForStatementNode : public StatementNode {
public:
    std::string variableName;
    std::shared_ptr<ExpressionNode> startExpr;
    std::shared_ptr<ExpressionNode> endExpr;
    std::shared_ptr<BlockNode> body;
    
    ForStatementNode(const std::string& variableName,
                     const std::shared_ptr<ExpressionNode>& startExpr,
                     const std::shared_ptr<ExpressionNode>& endExpr,
                     const std::shared_ptr<BlockNode>& body)
        : variableName(variableName), startExpr(startExpr), endExpr(endExpr), body(body) {}
};

// While loop
class WhileStatementNode : public StatementNode {
public:
    std::shared_ptr<ExpressionNode> condition;
    std::shared_ptr<BlockNode> body;
    
    WhileStatementNode(const std::shared_ptr<ExpressionNode>& condition,
                       const std::shared_ptr<BlockNode>& body)
        : condition(condition), body(body) {}
};

// Repeat-until loop
class RepeatStatementNode : public StatementNode {
public:
    std::shared_ptr<BlockNode> body;
    std::shared_ptr<ExpressionNode> condition;
    
    RepeatStatementNode(const std::shared_ptr<BlockNode>& body,
                        const std::shared_ptr<ExpressionNode>& condition)
        : body(body), condition(condition) {}
};

// Read statement
class ReadStatementNode : public StatementNode {
public:
    std::string variableName;
    
    ReadStatementNode(const std::string& variableName)
        : variableName(variableName) {}
};

// Write statement
class WriteStatementNode : public StatementNode {
public:
    std::shared_ptr<ExpressionNode> expression;
    
    WriteStatementNode(const std::shared_ptr<ExpressionNode>& expression)
        : expression(expression) {}
};

// Base class for expressions
class ExpressionNode : public ASTNode {};

// Binary operation
class BinaryOpNode : public ExpressionNode {
public:
    std::shared_ptr<ExpressionNode> left;
    std::string op;
    std::shared_ptr<ExpressionNode> right;
    
    BinaryOpNode(const std::shared_ptr<ExpressionNode>& left,
                 const std::string& op,
                 const std::shared_ptr<ExpressionNode>& right)
        : left(left), op(op), right(right) {}
};

// Unary operation
class UnaryOpNode : public ExpressionNode {
public:
    std::string op;
    std::shared_ptr<ExpressionNode> operand;
    
    UnaryOpNode(const std::string& op,
                const std::shared_ptr<ExpressionNode>& operand)
        : op(op), operand(operand) {}
};

// Variable reference
class VariableNode : public ExpressionNode {
public:
    std::string name;
    
    VariableNode(const std::string& name) : name(name) {}
};

// Number literal
class NumberNode : public ExpressionNode {
public:
    std::string value;  // Keep as string for simplicity
    
    NumberNode(const std::string& value) : value(value) {}
};

// Boolean literal
class BooleanNode : public ExpressionNode {
public:
    bool value;
    
    BooleanNode(bool value) : value(value) {}
};

// String literal - ADDED
class StringNode : public ExpressionNode {
public:
    std::string value;
    
    StringNode(const std::string& value) : value(value) {}
};

// Function call
class CallNode : public ExpressionNode {
public:
    std::string functionName;
    std::vector<std::shared_ptr<ExpressionNode>> arguments;
    
    CallNode(const std::string& functionName,
             const std::vector<std::shared_ptr<ExpressionNode>>& arguments = {})
        : functionName(functionName), arguments(arguments) {}
};

#endif