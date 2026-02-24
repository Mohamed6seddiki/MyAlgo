#ifndef PARSER_HPP
#define PARSER_HPP

#include "token.hpp"
#include "AST.hpp"
#include <vector>
#include <memory>

class Parser {
private:
    std::vector<Token> tokens;
    size_t current;

    Token peek() const;
    Token advance();
    Token previous() const;
    bool isAtEnd() const;
    bool check(TokenType type) const;
    bool match(TokenType type);
    Token consume(TokenType type, const std::string& message);
    std::string parseTypeName();
    std::shared_ptr<StructDeclNode> parseStructure();

    std::shared_ptr<ASTNode> parseAlgorithm();
    std::shared_ptr<FunctionNode> parseFunction();
    std::shared_ptr<ProcedureNode> parseProcedure();
    std::shared_ptr<BlockNode> parseStatementBlock(bool inFunctionOrProcedure = false);
    std::shared_ptr<StatementNode> parseStatement();
    std::shared_ptr<AssignmentNode> parseAssignment();
    std::shared_ptr<ReturnStatementNode> parseReturnStatement();
    std::shared_ptr<IfStatementNode> parseIfStatement();
    std::shared_ptr<ForStatementNode> parseForStatement();
    std::shared_ptr<WhileStatementNode> parseWhileStatement();
    std::shared_ptr<RepeatStatementNode> parseRepeatStatement();
    std::shared_ptr<ReadStatementNode> parseReadStatement();
    std::shared_ptr<WriteStatementNode> parseWriteStatement();
    std::shared_ptr<ExpressionNode> parseExpression();
    std::shared_ptr<ExpressionNode> parseLogicalOr();
    std::shared_ptr<ExpressionNode> parseLogicalAnd();
    std::shared_ptr<ExpressionNode> parseEquality();
    std::shared_ptr<ExpressionNode> parseComparison();
    std::shared_ptr<ExpressionNode> parseTerm();
    std::shared_ptr<ExpressionNode> parseFactor();
    std::shared_ptr<ExpressionNode> parseUnary();
    std::shared_ptr<ExpressionNode> parsePrimary();

public:
    Parser(const std::vector<Token>& tokens);
    std::shared_ptr<ASTNode> parse();
    void printAST(const std::shared_ptr<ASTNode>& node, int depth = 0);
};

#endif