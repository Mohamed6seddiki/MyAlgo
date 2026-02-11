#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <vector>
#include <memory>

// Forward declarations
class ASTNode;
class AlgorithmNode;
class FunctionNode;
class ProcedureNode;
class ParameterNode;
class VarDeclNode;
class StatementNode;
class BlockNode;
class AssignmentNode;
class ReturnStatementNode;
class IfStatementNode;
class ForStatementNode;
class WhileStatementNode;
class RepeatStatementNode;
class ReadStatementNode;
class WriteStatementNode;
class ExpressionStatementNode;
class ExpressionNode;
class BinaryOpNode;
class UnaryOpNode;
class VariableNode;
class NumberNode;
class BooleanNode;
class StringNode;
class CallNode;
class ArrayAccessNode;

// Base class for all AST nodes
class ASTNode {
public:
    virtual ~ASTNode() = default;
};

// Parameter declaration
class ParameterNode : public ASTNode {
public:
    std::string name;
    std::string typeName;
    bool isReference;  // For var parameters (pass by reference)
    
    ParameterNode(const std::string& name, const std::string& typeName, bool isReference = false)
        : name(name), typeName(typeName), isReference(isReference) {}
};

// Function definition
class FunctionNode : public ASTNode {
public:
    std::string name;
    std::vector<std::shared_ptr<ParameterNode>> parameters;
    std::string returnType;
    std::vector<std::shared_ptr<VarDeclNode>> variables;
    std::shared_ptr<BlockNode> body;
    
    FunctionNode(const std::string& name,
                 const std::vector<std::shared_ptr<ParameterNode>>& parameters,
                 const std::string& returnType,
                 const std::vector<std::shared_ptr<VarDeclNode>>& variables,
                 const std::shared_ptr<BlockNode>& body)
        : name(name), parameters(parameters), returnType(returnType),
          variables(variables), body(body) {}
};

// Procedure definition
class ProcedureNode : public ASTNode {
public:
    std::string name;
    std::vector<std::shared_ptr<ParameterNode>> parameters;
    std::vector<std::shared_ptr<VarDeclNode>> variables;
    std::shared_ptr<BlockNode> body;
    
    ProcedureNode(const std::string& name,
                  const std::vector<std::shared_ptr<ParameterNode>>& parameters,
                  const std::vector<std::shared_ptr<VarDeclNode>>& variables,
                  const std::shared_ptr<BlockNode>& body)
        : name(name), parameters(parameters), variables(variables), body(body) {}
};

// Algorithm definition
class AlgorithmNode : public ASTNode {
public:
    std::string name;
    std::vector<std::shared_ptr<VarDeclNode>> variables;
    std::vector<std::shared_ptr<FunctionNode>> functions;
    std::vector<std::shared_ptr<ProcedureNode>> procedures;
    std::shared_ptr<BlockNode> body;
    
    AlgorithmNode(const std::string& name, 
                  const std::vector<std::shared_ptr<VarDeclNode>>& variables,
                  const std::vector<std::shared_ptr<FunctionNode>>& functions,
                  const std::vector<std::shared_ptr<ProcedureNode>>& procedures,
                  const std::shared_ptr<BlockNode>& body)
        : name(name), variables(variables), functions(functions), 
          procedures(procedures), body(body) {}
};

// Variable declaration
class VarDeclNode : public ASTNode {
public:
    std::vector<std::string> names;  // Could declare multiple variables at once
    std::string typeName;
    bool isArray;               // Whether this is an array declaration
    int arraySize;              // Size of array (if isArray is true)
    
    VarDeclNode(const std::vector<std::string>& names, const std::string& typeName, 
                bool isArray = false, int arraySize = 0)
        : names(names), typeName(typeName), isArray(isArray), arraySize(arraySize) {}
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

// Return statement
class ReturnStatementNode : public StatementNode {
public:
    std::shared_ptr<ExpressionNode> expression;
    
    ReturnStatementNode(const std::shared_ptr<ExpressionNode>& expression = nullptr)
        : expression(expression) {}
};

// Assignment statement
class AssignmentNode : public StatementNode {
public:
    std::string variableName;
    std::shared_ptr<ExpressionNode> expression;
    std::shared_ptr<ExpressionNode> arrayIndex;  // nullptr if not array assignment
    
    AssignmentNode(const std::string& variableName, 
                   const std::shared_ptr<ExpressionNode>& expression,
                   const std::shared_ptr<ExpressionNode>& arrayIndex = nullptr)
        : variableName(variableName), expression(expression), arrayIndex(arrayIndex) {}
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

// Expression statement (for procedure calls and other expressions as statements)
class ExpressionStatementNode : public StatementNode {
public:
    std::shared_ptr<ExpressionNode> expression;
    
    ExpressionStatementNode(const std::shared_ptr<ExpressionNode>& expression)
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

// String literal
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

// Array access (indexing)
class ArrayAccessNode : public ExpressionNode {
public:
    std::string arrayName;
    std::shared_ptr<ExpressionNode> index;
    
    ArrayAccessNode(const std::string& arrayName, const std::shared_ptr<ExpressionNode>& index)
        : arrayName(arrayName), index(index) {}
};

#endif