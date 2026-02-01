// Parser.cpp
#include "token.hpp"
#include "AST.hpp"
#include <stdexcept>
#include <iostream>
#include <memory>

class Parser {
private:
    std::vector<Token> tokens;
    size_t current;

    // Helper methods
    Token peek() const {
        if (current >= tokens.size()) {
            return Token(TokenType::END_OF_FILE);
        }
        return tokens[current];
    }

    Token advance() {
        if (!isAtEnd()) {
            current++;
        }
        return previous();
    }

    Token previous() const {
        if (current == 0) {
            return Token(TokenType::END_OF_FILE);
        }
        return tokens[current - 1];
    }

    bool isAtEnd() const {
        return peek().type == TokenType::END_OF_FILE;
    }

    bool check(TokenType type) const {
        if (isAtEnd()) return false;
        return peek().type == type;
    }

    bool match(TokenType type) {
        if (check(type)) {
            advance();
            return true;
        }
        return false;
    }

    Token consume(TokenType type, const std::string& message) {
        if (check(type)) {
            return advance();
        }
        throw std::runtime_error("Parse error: " + message);
    }

    // Error reporting
    void error(const Token& token, const std::string& message) {
        std::cerr << "Error at token '" << token.value << "': " << message << std::endl;
        throw std::runtime_error("Parse error");
    }

    // Grammar rules implementation

    // Program structure
    std::shared_ptr<ASTNode> parseAlgorithm() {
        // algorithm <identifier>
        consume(TokenType::ALGORITHM, "Expected 'algorithm'");
        std::string name = consume(TokenType::IDENTIFIER, "Expected algorithm name").value;
        
        // Optional variable declarations
        std::vector<std::shared_ptr<VarDeclNode>> variables;
        if (match(TokenType::VAR)) {
            variables = parseVarDeclarations();
        }
        
        // begin ... end
        consume(TokenType::BEGIN, "Expected 'begin'");
        auto block = parseStatementBlock();
        consume(TokenType::END, "Expected 'end'");
        consume(TokenType::PERIOD, "Expected '.' at end of algorithm");
        
        return std::make_shared<AlgorithmNode>(name, variables, block);
    }

    std::vector<std::shared_ptr<VarDeclNode>> parseVarDeclarations() {
        std::vector<std::shared_ptr<VarDeclNode>> declarations;
        
        while (check(TokenType::IDENTIFIER)) {
            auto decl = parseVarDeclaration();
            declarations.push_back(decl);
            consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");
        }
        
        return declarations;
    }

    std::shared_ptr<VarDeclNode> parseVarDeclaration() {
        // identifier : type
        std::string varName = consume(TokenType::IDENTIFIER, "Expected variable name").value;
        
        // Check for comma-separated list
        std::vector<std::string> names = {varName};
        while (match(TokenType::COMMA)) {
            names.push_back(consume(TokenType::IDENTIFIER, "Expected variable name").value);
        }
        
        consume(TokenType::COLON, "Expected ':' after variable name(s)");
        
        // Parse type
        Token typeToken = advance();
        std::string typeName;
        switch (typeToken.type) {
            case TokenType::INTEGER:
                typeName = "integer";
                break;
            case TokenType::REAL:
                typeName = "real";
                break;
            case TokenType::BOOLEAN:
                typeName = "boolean";
                break;
            default:
                error(typeToken, "Expected type (integer, real, or boolean)");
        }
        
        return std::make_shared<VarDeclNode>(names, typeName);
    }

    // Statements
    std::shared_ptr<BlockNode> parseStatementBlock() {
        std::vector<std::shared_ptr<StatementNode>> statements;
        
        while (!check(TokenType::END) && !check(TokenType::ENDIF) && 
               !check(TokenType::ENDFOR) && !check(TokenType::ENDWHILE) &&
               !check(TokenType::UNTIL) && !check(TokenType::ELSE) && !isAtEnd()) {
            
            auto stmt = parseStatement();
            if (stmt) {
                statements.push_back(stmt);
                
                // Only consume semicolon if it's there
                // (some statements might not have semicolons in Pascal-like syntax)
                if (check(TokenType::SEMICOLON)) {
                    advance();
                }
            }
        }
        
        return std::make_shared<BlockNode>(statements);
    }

    std::shared_ptr<StatementNode> parseStatement() {
        // Handle block terminators - return nullptr to indicate end of block
        if (check(TokenType::ENDIF) || check(TokenType::ENDFOR) || 
            check(TokenType::ENDWHILE) || check(TokenType::END) ||
            check(TokenType::UNTIL) || check(TokenType::ELSE)) {
            return nullptr;
        }
        
        if (check(TokenType::IDENTIFIER)) {
            return parseAssignment();
        } else if (match(TokenType::IF)) {
            return parseIfStatement();
        } else if (match(TokenType::FOR)) {
            return parseForStatement();
        } else if (match(TokenType::WHILE)) {
            return parseWhileStatement();
        } else if (match(TokenType::REPEAT)) {
            return parseRepeatStatement();
        } else if (match(TokenType::READ)) {
            return parseReadStatement();
        } else if (match(TokenType::WRITE)) {
            return parseWriteStatement();
        } else {
            error(peek(), "Expected statement");
            return nullptr;
        }
    }

    std::shared_ptr<AssignmentNode> parseAssignment() {
        std::string identifier = consume(TokenType::IDENTIFIER, "Expected identifier").value;
        
        consume(TokenType::ASSIGN, "Expected ':=' for assignment");
        
        // Assignment statement
        auto expr = parseExpression();
        return std::make_shared<AssignmentNode>(identifier, expr);
    }

    std::shared_ptr<IfStatementNode> parseIfStatement() {
        // if condition then statements endif
        auto condition = parseExpression();
        consume(TokenType::THEN, "Expected 'then' after if condition");
        
        auto thenBlock = parseStatementBlock();
        
        // Handle optional else
        std::shared_ptr<BlockNode> elseBlock = nullptr;
        if (match(TokenType::ELSE)) {
            elseBlock = parseStatementBlock();
        }
        
        consume(TokenType::ENDIF, "Expected 'endif'");
        
        return std::make_shared<IfStatementNode>(condition, thenBlock, elseBlock);
    }

    std::shared_ptr<ForStatementNode> parseForStatement() {
        // for var from start to end do statements endfor
        std::string varName = consume(TokenType::IDENTIFIER, "Expected loop variable").value;
        consume(TokenType::FROM, "Expected 'from' in for loop");
        
        auto startExpr = parseExpression();
        consume(TokenType::TO, "Expected 'to' in for loop");
        auto endExpr = parseExpression();
        consume(TokenType::DO, "Expected 'do' in for loop");
        
        auto body = parseStatementBlock();
        consume(TokenType::ENDFOR, "Expected 'endfor'");
        
        return std::make_shared<ForStatementNode>(varName, startExpr, endExpr, body);
    }

    std::shared_ptr<WhileStatementNode> parseWhileStatement() {
        // while condition do statements endwhile
        auto condition = parseExpression();
        consume(TokenType::DO, "Expected 'do' after while condition");
        
        auto body = parseStatementBlock();
        consume(TokenType::ENDWHILE, "Expected 'endwhile'");
        
        return std::make_shared<WhileStatementNode>(condition, body);
    }

    std::shared_ptr<RepeatStatementNode> parseRepeatStatement() {
        // repeat statements until condition
        auto body = parseStatementBlock();
        consume(TokenType::UNTIL, "Expected 'until' after repeat block");
        
        auto condition = parseExpression();
        
        return std::make_shared<RepeatStatementNode>(body, condition);
    }

    std::shared_ptr<ReadStatementNode> parseReadStatement() {
        // read(identifier)
        consume(TokenType::LPAREN, "Expected '(' after read");
        std::string varName = consume(TokenType::IDENTIFIER, "Expected variable name in read").value;
        consume(TokenType::RPAREN, "Expected ')' after read variable");
        
        return std::make_shared<ReadStatementNode>(varName);
    }

    std::shared_ptr<WriteStatementNode> parseWriteStatement() {
        // write(expression)
        consume(TokenType::LPAREN, "Expected '(' after write");
        auto expr = parseExpression();
        consume(TokenType::RPAREN, "Expected ')' after write expression");
        
        return std::make_shared<WriteStatementNode>(expr);
    }

    // Expressions
    std::shared_ptr<ExpressionNode> parseExpression() {
        return parseLogicalOr();
    }

    std::shared_ptr<ExpressionNode> parseLogicalOr() {
        auto expr = parseLogicalAnd();
        
        while (match(TokenType::OR)) {
            Token op = previous();
            auto right = parseLogicalAnd();
            expr = std::make_shared<BinaryOpNode>(expr, op.value, right);
        }
        
        return expr;
    }

    std::shared_ptr<ExpressionNode> parseLogicalAnd() {
        auto expr = parseEquality();
        
        while (match(TokenType::AND)) {
            Token op = previous();
            auto right = parseEquality();
            expr = std::make_shared<BinaryOpNode>(expr, op.value, right);
        }
        
        return expr;
    }

    std::shared_ptr<ExpressionNode> parseEquality() {
        auto expr = parseComparison();
        
        while (match(TokenType::EQUAL) || match(TokenType::NOT_EQUAL)) {
            Token op = previous();
            auto right = parseComparison();
            expr = std::make_shared<BinaryOpNode>(expr, op.value, right);
        }
        
        return expr;
    }

    std::shared_ptr<ExpressionNode> parseComparison() {
        auto expr = parseTerm();
        
        while (match(TokenType::LESS) || match(TokenType::LESS_EQUAL) ||
               match(TokenType::GREATER) || match(TokenType::GREATER_EQUAL)) {
            Token op = previous();
            auto right = parseTerm();
            expr = std::make_shared<BinaryOpNode>(expr, op.value, right);
        }
        
        return expr;
    }

    std::shared_ptr<ExpressionNode> parseTerm() {
        auto expr = parseFactor();
        
        while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
            Token op = previous();
            auto right = parseFactor();
            expr = std::make_shared<BinaryOpNode>(expr, op.value, right);
        }
        
        return expr;
    }

    std::shared_ptr<ExpressionNode> parseFactor() {
        auto expr = parseUnary();
        
        while (match(TokenType::MULTIPLY) || match(TokenType::DIVIDE) ||
               match(TokenType::DIV) || match(TokenType::MOD)) {
            Token op = previous();
            auto right = parseUnary();
            expr = std::make_shared<BinaryOpNode>(expr, op.value, right);
        }
        
        return expr;
    }

    std::shared_ptr<ExpressionNode> parseUnary() {
        if (match(TokenType::NOT) || match(TokenType::MINUS)) {
            Token op = previous();
            auto right = parseUnary();
            return std::make_shared<UnaryOpNode>(op.value, right);
        }
        
        return parsePrimary();
    }

    std::shared_ptr<ExpressionNode> parsePrimary() {
        if (match(TokenType::NUMBER)) {
            return std::make_shared<NumberNode>(previous().value);
        }
        
        if (match(TokenType::STRING)) {
            return std::make_shared<StringNode>(previous().value);
        }
        
        if (match(TokenType::TRUE)) {
            return std::make_shared<BooleanNode>(true);
        }
        
        if (match(TokenType::FALSE)) {
            return std::make_shared<BooleanNode>(false);
        }
        
        if (match(TokenType::IDENTIFIER)) {
            std::string name = previous().value;
            
            // Check if it's a function call
            if (match(TokenType::LPAREN)) {
                auto args = parseArgumentList();
                consume(TokenType::RPAREN, "Expected ')' after arguments");
                return std::make_shared<CallNode>(name, args);
            }
            
            // Otherwise it's a variable
            return std::make_shared<VariableNode>(name);
        }
        
        if (match(TokenType::LPAREN)) {
            auto expr = parseExpression();
            consume(TokenType::RPAREN, "Expected ')' after expression");
            return expr;
        }
        
        error(peek(), "Expected expression (number, string, identifier, boolean, or parenthesized expression)");
        return nullptr;
    }

    std::vector<std::shared_ptr<ExpressionNode>> parseArgumentList() {
        std::vector<std::shared_ptr<ExpressionNode>> args;
        
        if (!check(TokenType::RPAREN)) {
            do {
                args.push_back(parseExpression());
            } while (match(TokenType::COMMA));
        }
        
        return args;
    }

public:
    Parser(const std::vector<Token>& tokens)
        : tokens(tokens), current(0) {}
    
    std::shared_ptr<ASTNode> parse() {
        try {
            return parseAlgorithm();
        } catch (const std::runtime_error& e) {
            std::cerr << "Parsing failed: " << e.what() << std::endl;
            throw;
        }
    }
    
    // Utility for debugging
    void printAST(const std::shared_ptr<ASTNode>& node, int depth = 0) {
        std::string indent(depth * 2, ' ');
        
        if (!node) {
            std::cout << indent << "(null)" << std::endl;
            return;
        }
        
        if (auto algo = std::dynamic_pointer_cast<AlgorithmNode>(node)) {
            std::cout << indent << "Algorithm: " << algo->name << std::endl;
            std::cout << indent << "Variables:" << std::endl;
            for (const auto& var : algo->variables) {
                printAST(var, depth + 1);
            }
            std::cout << indent << "Body:" << std::endl;
            printAST(algo->body, depth + 1);
        }
        else if (auto varDecl = std::dynamic_pointer_cast<VarDeclNode>(node)) {
            std::cout << indent << "VarDecl: ";
            for (size_t i = 0; i < varDecl->names.size(); i++) {
                if (i > 0) std::cout << ", ";
                std::cout << varDecl->names[i];
            }
            std::cout << " : " << varDecl->typeName << std::endl;
        }
        else if (auto block = std::dynamic_pointer_cast<BlockNode>(node)) {
            std::cout << indent << "Block (" << block->statements.size() << " statements):" << std::endl;
            for (const auto& stmt : block->statements) {
                if (stmt) {
                    printAST(stmt, depth + 1);
                }
            }
        }
        else if (auto assign = std::dynamic_pointer_cast<AssignmentNode>(node)) {
            std::cout << indent << "Assignment: " << assign->variableName << " =" << std::endl;
            printAST(assign->expression, depth + 1);
        }
        else if (auto ifStmt = std::dynamic_pointer_cast<IfStatementNode>(node)) {
            std::cout << indent << "If:" << std::endl;
            std::cout << indent << "  Condition:" << std::endl;
            printAST(ifStmt->condition, depth + 2);
            std::cout << indent << "  Then:" << std::endl;
            printAST(ifStmt->thenBlock, depth + 2);
            if (ifStmt->elseBlock) {
                std::cout << indent << "  Else:" << std::endl;
                printAST(ifStmt->elseBlock, depth + 2);
            }
        }
        else if (auto forStmt = std::dynamic_pointer_cast<ForStatementNode>(node)) {
            std::cout << indent << "For: " << forStmt->variableName << std::endl;
            std::cout << indent << "  From:" << std::endl;
            printAST(forStmt->startExpr, depth + 2);
            std::cout << indent << "  To:" << std::endl;
            printAST(forStmt->endExpr, depth + 2);
            std::cout << indent << "  Body:" << std::endl;
            printAST(forStmt->body, depth + 2);
        }
        else if (auto whileStmt = std::dynamic_pointer_cast<WhileStatementNode>(node)) {
            std::cout << indent << "While:" << std::endl;
            std::cout << indent << "  Condition:" << std::endl;
            printAST(whileStmt->condition, depth + 2);
            std::cout << indent << "  Body:" << std::endl;
            printAST(whileStmt->body, depth + 2);
        }
        else if (auto repeatStmt = std::dynamic_pointer_cast<RepeatStatementNode>(node)) {
            std::cout << indent << "Repeat-Until:" << std::endl;
            std::cout << indent << "  Body:" << std::endl;
            printAST(repeatStmt->body, depth + 2);
            std::cout << indent << "  Condition:" << std::endl;
            printAST(repeatStmt->condition, depth + 2);
        }
        else if (auto readStmt = std::dynamic_pointer_cast<ReadStatementNode>(node)) {
            std::cout << indent << "Read: " << readStmt->variableName << std::endl;
        }
        else if (auto writeStmt = std::dynamic_pointer_cast<WriteStatementNode>(node)) {
            std::cout << indent << "Write:" << std::endl;
            printAST(writeStmt->expression, depth + 1);
        }
        else if (auto binOp = std::dynamic_pointer_cast<BinaryOpNode>(node)) {
            std::cout << indent << "BinaryOp: " << binOp->op << std::endl;
            std::cout << indent << "  Left:" << std::endl;
            printAST(binOp->left, depth + 2);
            std::cout << indent << "  Right:" << std::endl;
            printAST(binOp->right, depth + 2);
        }
        else if (auto unaryOp = std::dynamic_pointer_cast<UnaryOpNode>(node)) {
            std::cout << indent << "UnaryOp: " << unaryOp->op << std::endl;
            printAST(unaryOp->operand, depth + 1);
        }
        else if (auto var = std::dynamic_pointer_cast<VariableNode>(node)) {
            std::cout << indent << "Variable: " << var->name << std::endl;
        }
        else if (auto num = std::dynamic_pointer_cast<NumberNode>(node)) {
            std::cout << indent << "Number: " << num->value << std::endl;
        }
        else if (auto boolean = std::dynamic_pointer_cast<BooleanNode>(node)) {
            std::cout << indent << "Boolean: " << (boolean->value ? "true" : "false") << std::endl;
        }
        else if (auto str = std::dynamic_pointer_cast<StringNode>(node)) {
            std::cout << indent << "String: \"" << str->value << "\"" << std::endl;
        }
        else if (auto call = std::dynamic_pointer_cast<CallNode>(node)) {
            std::cout << indent << "Call: " << call->functionName << " (" << call->arguments.size() << " args)" << std::endl;
        }
        else {
            std::cout << indent << "Unknown AST node type" << std::endl;
        }
    }
};