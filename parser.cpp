#include "parser.hpp"
#include <stdexcept>
#include <iostream>

Token Parser::peek() const {
    if (current >= tokens.size()) return Token(TokenType::END_OF_FILE);
    return tokens[current];
}

Token Parser::advance() {
    if (current < tokens.size()) current++;
    return previous();
}

Token Parser::previous() const {
    if (current == 0) return Token(TokenType::END_OF_FILE);
    return tokens[current - 1];
}

bool Parser::isAtEnd() const { return peek().type == TokenType::END_OF_FILE; }

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) { advance(); return true; }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw std::runtime_error("Parse error: " + message + " at token '" + peek().value + "'");
}

std::shared_ptr<ASTNode> Parser::parseAlgorithm() {
    consume(TokenType::ALGORITHM, "Expected 'algorithm'");
    std::string name = consume(TokenType::IDENTIFIER, "Expected algorithm name").value;
    
    std::vector<std::shared_ptr<VarDeclNode>> variables;
    std::vector<std::shared_ptr<FunctionNode>> functions;
    std::vector<std::shared_ptr<ProcedureNode>> procedures;
    
    if (match(TokenType::VAR)) {
        while (check(TokenType::IDENTIFIER)) {
            std::string varName = consume(TokenType::IDENTIFIER, "Expected variable name").value;
            std::vector<std::string> names = {varName};
            while (match(TokenType::COMMA)) {
                names.push_back(consume(TokenType::IDENTIFIER, "Expected variable name").value);
            }
            consume(TokenType::COLON, "Expected ':' after variable name(s)");
            
            // Check if it's an array type
            bool isArray = false;
            int arraySize = 0;
            
            if (match(TokenType::ARRAY)) {
                isArray = true;
                consume(TokenType::LBRACKET, "Expected '[' after 'array'");
                Token sizeToken = consume(TokenType::NUMBER, "Expected array size");
                arraySize = std::stoi(sizeToken.value);
                consume(TokenType::RBRACKET, "Expected ']' after array size");
                consume(TokenType::OF, "Expected 'of' after array declaration");
            }
            
            Token typeToken = advance();
            std::string typeName;
            switch (typeToken.type) {
                case TokenType::INTEGER: typeName = "integer"; break;
                case TokenType::REAL: typeName = "real"; break;
                case TokenType::BOOLEAN: typeName = "boolean"; break;
                case TokenType::STRING_TYPE: typeName = "string"; break;
                default: throw std::runtime_error("Expected type (integer, real, boolean, or string)");
            }
            
            variables.push_back(std::make_shared<VarDeclNode>(names, typeName, isArray, arraySize));
            consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");
        }
    }
    
    // Parse functions and procedures
    while (true) {
        if (check(TokenType::FUNCTION)) {
            advance();
            functions.push_back(parseFunction());
        } else if (check(TokenType::PROCEDURE)) {
            advance();
            procedures.push_back(parseProcedure());
        } else {
            break;
        }
    }
    
    consume(TokenType::BEGIN, "Expected 'begin'");
    auto block = parseStatementBlock(false);
    consume(TokenType::END, "Expected 'end'");
    consume(TokenType::PERIOD, "Expected '.' at end of algorithm");
    
    return std::make_shared<AlgorithmNode>(name, variables, functions, procedures, block);
}

std::shared_ptr<FunctionNode> Parser::parseFunction() {
    std::string name = consume(TokenType::IDENTIFIER, "Expected function name").value;
    
    // Parse parameters
    std::vector<std::shared_ptr<ParameterNode>> parameters;
    
    // Check if there are parameters
    if (match(TokenType::LPAREN)) {
        if (!check(TokenType::RPAREN)) {
            do {
                // Check for var parameter (pass by reference)
                bool isReference = match(TokenType::VAR);
                
                // Parse parameter name
                std::string paramName = consume(TokenType::IDENTIFIER, "Expected parameter name").value;
                std::vector<std::string> paramNames = {paramName};
                
                // Check for more parameter names separated by commas
                while (match(TokenType::COMMA)) {
                    // Don't consume if next is colon
                    if (check(TokenType::COLON)) {
                        break;
                    }
                    paramNames.push_back(consume(TokenType::IDENTIFIER, "Expected parameter name").value);
                }
                
                consume(TokenType::COLON, "Expected ':' after parameter name(s)");
                
                Token typeToken = advance();
                std::string typeName;
                switch (typeToken.type) {
                    case TokenType::INTEGER: typeName = "integer"; break;
                    case TokenType::REAL: typeName = "real"; break;
                    case TokenType::BOOLEAN: typeName = "boolean"; break;
                    case TokenType::STRING_TYPE: typeName = "string"; break;
                    default: throw std::runtime_error("Expected type (integer, real, boolean, or string)");
                }
                
                // Create a parameter node for each name
                for (const auto& pName : paramNames) {
                    parameters.push_back(std::make_shared<ParameterNode>(pName, typeName, isReference));
                }
                
            } while (match(TokenType::COMMA));
        }
        consume(TokenType::RPAREN, "Expected ')' after parameters");
    }
    
    // Parse return type
    consume(TokenType::COLON, "Expected ':' before return type");
    Token returnTypeToken = advance();
    std::string returnType;
    switch (returnTypeToken.type) {
        case TokenType::INTEGER: returnType = "integer"; break;
        case TokenType::REAL: returnType = "real"; break;
        case TokenType::BOOLEAN: returnType = "boolean"; break;
        case TokenType::STRING_TYPE: returnType = "string"; break;
        default: throw std::runtime_error("Expected return type (integer, real, boolean, or string)");
    }
    
    // Parse local variables
    std::vector<std::shared_ptr<VarDeclNode>> localVars;
    if (match(TokenType::VAR)) {
        while (check(TokenType::IDENTIFIER)) {
            std::string varName = consume(TokenType::IDENTIFIER, "Expected variable name").value;
            std::vector<std::string> names = {varName};
            while (match(TokenType::COMMA)) {
                names.push_back(consume(TokenType::IDENTIFIER, "Expected variable name").value);
            }
            consume(TokenType::COLON, "Expected ':' after variable name(s)");
            
            Token typeToken = advance();
            std::string typeName;
            switch (typeToken.type) {
                case TokenType::INTEGER: typeName = "integer"; break;
                case TokenType::REAL: typeName = "real"; break;
                case TokenType::BOOLEAN: typeName = "boolean"; break;
                case TokenType::STRING_TYPE: typeName = "string"; break;
                default: throw std::runtime_error("Expected type (integer, real, boolean, or string)");
            }
            
            localVars.push_back(std::make_shared<VarDeclNode>(names, typeName));
            consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");
        }
    }
    
    // Parse function body - require beginfunction
    if (!match(TokenType::BEGINFUNCTION)) {
        throw std::runtime_error("Expected 'beginfunction' in function '" + name + "'");
    }
    
    auto body = parseStatementBlock(true);
    
    // Handle end of function
    if (!match(TokenType::ENDFUNCTION)) {
        throw std::runtime_error("Expected 'endfunction' at end of function '" + name + "'");
    }
    
    return std::make_shared<FunctionNode>(name, parameters, returnType, localVars, body);
}

std::shared_ptr<ProcedureNode> Parser::parseProcedure() {
    std::string name = consume(TokenType::IDENTIFIER, "Expected procedure name").value;
    
    // Parse parameters
    std::vector<std::shared_ptr<ParameterNode>> parameters;
    
    // Check if there are parameters
    if (match(TokenType::LPAREN)) {
        if (!check(TokenType::RPAREN)) {
            do {
                // Check for var parameter (pass by reference)
                bool isReference = match(TokenType::VAR);
                
                // Parse parameter name
                std::string paramName = consume(TokenType::IDENTIFIER, "Expected parameter name").value;
                std::vector<std::string> paramNames = {paramName};
                
                // Check for more parameter names separated by commas
                while (match(TokenType::COMMA)) {
                    // Don't consume if next is colon
                    if (check(TokenType::COLON)) {
                        break;
                    }
                    paramNames.push_back(consume(TokenType::IDENTIFIER, "Expected parameter name").value);
                }
                
                consume(TokenType::COLON, "Expected ':' after parameter name(s)");
                
                Token typeToken = advance();
                std::string typeName;
                switch (typeToken.type) {
                    case TokenType::INTEGER: typeName = "integer"; break;
                    case TokenType::REAL: typeName = "real"; break;
                    case TokenType::BOOLEAN: typeName = "boolean"; break;
                    case TokenType::STRING_TYPE: typeName = "string"; break;
                    default: throw std::runtime_error("Expected type (integer, real, boolean, or string)");
                }
                
                // Create a parameter node for each name
                for (const auto& pName : paramNames) {
                    parameters.push_back(std::make_shared<ParameterNode>(pName, typeName, isReference));
                }
                
            } while (match(TokenType::COMMA));
        }
        consume(TokenType::RPAREN, "Expected ')' after parameters");
    }
    
    // Parse local variables
    std::vector<std::shared_ptr<VarDeclNode>> localVars;
    if (match(TokenType::VAR)) {
        while (check(TokenType::IDENTIFIER)) {
            std::string varName = consume(TokenType::IDENTIFIER, "Expected variable name").value;
            std::vector<std::string> names = {varName};
            while (match(TokenType::COMMA)) {
                names.push_back(consume(TokenType::IDENTIFIER, "Expected variable name").value);
            }
            consume(TokenType::COLON, "Expected ':' after variable name(s)");
            
            Token typeToken = advance();
            std::string typeName;
            switch (typeToken.type) {
                case TokenType::INTEGER: typeName = "integer"; break;
                case TokenType::REAL: typeName = "real"; break;
                case TokenType::BOOLEAN: typeName = "boolean"; break;
                case TokenType::STRING_TYPE: typeName = "string"; break;
                default: throw std::runtime_error("Expected type (integer, real, boolean, or string)");
            }
            
            localVars.push_back(std::make_shared<VarDeclNode>(names, typeName));
            consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");
        }
    }
    
    // Parse procedure body - require beginprocedure
    if (!match(TokenType::BEGINPROCEDURE)) {
        throw std::runtime_error("Expected 'beginprocedure' in procedure '" + name + "'");
    }
    
    auto body = parseStatementBlock(true);
    
    // Handle end of procedure
    if (!match(TokenType::ENDPROCEDURE)) {
        throw std::runtime_error("Expected 'endprocedure' at end of procedure '" + name + "'");
    }
    
    return std::make_shared<ProcedureNode>(name, parameters, localVars, body);
}

std::shared_ptr<BlockNode> Parser::parseStatementBlock(bool inFunctionOrProcedure) {
    std::vector<std::shared_ptr<StatementNode>> statements;
    
    while (!isAtEnd()) {
        // Check for block terminators
        if (check(TokenType::END) || 
            check(TokenType::ENDIF) || 
            check(TokenType::ENDFOR) || 
            check(TokenType::ENDWHILE) ||
            check(TokenType::UNTIL) || 
            check(TokenType::ELSE) || 
            check(TokenType::ENDFUNCTION) || 
            check(TokenType::ENDPROCEDURE)) {
            break;
        }
        
        auto stmt = parseStatement();
        if (stmt) {
            statements.push_back(stmt);
        }
        
        // Handle semicolon - it's optional in many cases
        if (check(TokenType::SEMICOLON)) {
            advance();
        }
    }
    
    return std::make_shared<BlockNode>(statements);
}

std::shared_ptr<StatementNode> Parser::parseStatement() {
    // Check for statement terminators
    if (check(TokenType::ENDIF) || check(TokenType::ENDFOR) || 
        check(TokenType::ENDWHILE) || check(TokenType::END) ||
        check(TokenType::UNTIL) || check(TokenType::ELSE) ||
        check(TokenType::ENDFUNCTION) || check(TokenType::ENDPROCEDURE)) {
        return nullptr;
    }
    
    if (check(TokenType::IDENTIFIER)) {
        // Save position
        size_t savedPos = current;
        std::string identifier = peek().value;
        advance();
        
        if (check(TokenType::ASSIGN)) {
            // It's an assignment
            current = savedPos;
            return parseAssignment();
        } else if (check(TokenType::LPAREN)) {
            // It's a procedure call
            current = savedPos;
            auto expr = parseExpression();
            return std::make_shared<ExpressionStatementNode>(expr);
        } else {
            // Just an identifier - might be a variable used as statement (error)
            current = savedPos;
            // Try to parse as assignment, will throw if no :=
            return parseAssignment();
        }
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
    } else if (match(TokenType::RETURN)) {
        return parseReturnStatement();
    } else {
        // Skip unknown token and continue
        if (!isAtEnd() && !check(TokenType::SEMICOLON)) {
            std::cerr << "Warning: Unexpected token '" << peek().value << "', skipping" << std::endl;
            advance();
        }
        return nullptr;
    }
}

std::shared_ptr<AssignmentNode> Parser::parseAssignment() {
    std::string identifier = consume(TokenType::IDENTIFIER, "Expected identifier").value;
    
    std::shared_ptr<ExpressionNode> arrayIndex = nullptr;
    
    // Check for array indexing
    if (match(TokenType::LBRACKET)) {
        arrayIndex = parseExpression();
        consume(TokenType::RBRACKET, "Expected ']' after array index");
    }
    
    consume(TokenType::ASSIGN, "Expected ':=' for assignment");
    auto expr = parseExpression();
    return std::make_shared<AssignmentNode>(identifier, expr, arrayIndex);
}

std::shared_ptr<ReturnStatementNode> Parser::parseReturnStatement() {
    std::shared_ptr<ExpressionNode> expr = nullptr;
    if (!check(TokenType::SEMICOLON) && !check(TokenType::ENDIF) && 
        !check(TokenType::ENDFOR) && !check(TokenType::ENDWHILE) &&
        !check(TokenType::UNTIL) && !check(TokenType::ELSE) && 
        !check(TokenType::ENDFUNCTION) && !check(TokenType::ENDPROCEDURE) && 
        !check(TokenType::END)) {
        expr = parseExpression();
    }
    return std::make_shared<ReturnStatementNode>(expr);
}

std::shared_ptr<IfStatementNode> Parser::parseIfStatement() {
    auto condition = parseExpression();
    consume(TokenType::THEN, "Expected 'then' after if condition");
    auto thenBlock = parseStatementBlock(false);
    
    std::shared_ptr<BlockNode> elseBlock = nullptr;
    if (match(TokenType::ELSE)) {
        elseBlock = parseStatementBlock(false);
    }
    
    consume(TokenType::ENDIF, "Expected 'endif'");
    return std::make_shared<IfStatementNode>(condition, thenBlock, elseBlock);
}

std::shared_ptr<ForStatementNode> Parser::parseForStatement() {
    std::string varName = consume(TokenType::IDENTIFIER, "Expected loop variable").value;
    consume(TokenType::FROM, "Expected 'from' in for loop");
    auto startExpr = parseExpression();
    consume(TokenType::TO, "Expected 'to' in for loop");
    auto endExpr = parseExpression();
    consume(TokenType::DO, "Expected 'do' in for loop");
    auto body = parseStatementBlock(false);
    consume(TokenType::ENDFOR, "Expected 'endfor'");
    return std::make_shared<ForStatementNode>(varName, startExpr, endExpr, body);
}

std::shared_ptr<WhileStatementNode> Parser::parseWhileStatement() {
    auto condition = parseExpression();
    consume(TokenType::DO, "Expected 'do' after while condition");
    auto body = parseStatementBlock(false);
    consume(TokenType::ENDWHILE, "Expected 'endwhile'");
    return std::make_shared<WhileStatementNode>(condition, body);
}

std::shared_ptr<RepeatStatementNode> Parser::parseRepeatStatement() {
    auto body = parseStatementBlock(false);
    consume(TokenType::UNTIL, "Expected 'until' after repeat block");
    auto condition = parseExpression();
    return std::make_shared<RepeatStatementNode>(body, condition);
}

std::shared_ptr<ReadStatementNode> Parser::parseReadStatement() {
    consume(TokenType::LPAREN, "Expected '(' after read");
    std::string varName = consume(TokenType::IDENTIFIER, "Expected variable name in read").value;
    consume(TokenType::RPAREN, "Expected ')' after read variable");
    return std::make_shared<ReadStatementNode>(varName);
}

std::shared_ptr<WriteStatementNode> Parser::parseWriteStatement() {
    consume(TokenType::LPAREN, "Expected '(' after write");
    auto expr = parseExpression();
    consume(TokenType::RPAREN, "Expected ')' after write expression");
    return std::make_shared<WriteStatementNode>(expr);
}

std::shared_ptr<ExpressionNode> Parser::parseExpression() {
    return parseLogicalOr();
}

std::shared_ptr<ExpressionNode> Parser::parseLogicalOr() {
    auto expr = parseLogicalAnd();
    while (match(TokenType::OR)) {
        Token op = previous();
        auto right = parseLogicalAnd();
        expr = std::make_shared<BinaryOpNode>(expr, op.value, right);
    }
    return expr;
}

std::shared_ptr<ExpressionNode> Parser::parseLogicalAnd() {
    auto expr = parseEquality();
    while (match(TokenType::AND)) {
        Token op = previous();
        auto right = parseEquality();
        expr = std::make_shared<BinaryOpNode>(expr, op.value, right);
    }
    return expr;
}

std::shared_ptr<ExpressionNode> Parser::parseEquality() {
    auto expr = parseComparison();
    while (match(TokenType::EQUAL) || match(TokenType::NOT_EQUAL)) {
        Token op = previous();
        auto right = parseComparison();
        expr = std::make_shared<BinaryOpNode>(expr, op.value, right);
    }
    return expr;
}

std::shared_ptr<ExpressionNode> Parser::parseComparison() {
    auto expr = parseTerm();
    while (match(TokenType::LESS) || match(TokenType::LESS_EQUAL) ||
           match(TokenType::GREATER) || match(TokenType::GREATER_EQUAL)) {
        Token op = previous();
        auto right = parseTerm();
        expr = std::make_shared<BinaryOpNode>(expr, op.value, right);
    }
    return expr;
}

std::shared_ptr<ExpressionNode> Parser::parseTerm() {
    auto expr = parseFactor();
    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        Token op = previous();
        auto right = parseFactor();
        expr = std::make_shared<BinaryOpNode>(expr, op.value, right);
    }
    return expr;
}

std::shared_ptr<ExpressionNode> Parser::parseFactor() {
    auto expr = parseUnary();
    while (match(TokenType::MULTIPLY) || match(TokenType::DIVIDE) ||
           match(TokenType::DIV) || match(TokenType::MOD)) {
        Token op = previous();
        auto right = parseUnary();
        expr = std::make_shared<BinaryOpNode>(expr, op.value, right);
    }
    return expr;
}

std::shared_ptr<ExpressionNode> Parser::parseUnary() {
    if (match(TokenType::MINUS) || match(TokenType::NOT)) {
        Token op = previous();
        auto operand = parseUnary();
        return std::make_shared<UnaryOpNode>(op.value, operand);
    }
    return parsePrimary();
}

std::shared_ptr<ExpressionNode> Parser::parsePrimary() {
    if (match(TokenType::TRUE)) {
        return std::make_shared<BooleanNode>(true);
    }
    if (match(TokenType::FALSE)) {
        return std::make_shared<BooleanNode>(false);
    }
    if (match(TokenType::NUMBER)) {
        return std::make_shared<NumberNode>(previous().value);
    }
    if (match(TokenType::STRING)) {
        return std::make_shared<StringNode>(previous().value);
    }
    if (match(TokenType::IDENTIFIER)) {
        std::string name = previous().value;
        
        // Check if it's a function call
        if (match(TokenType::LPAREN)) {
            std::vector<std::shared_ptr<ExpressionNode>> arguments;
            if (!check(TokenType::RPAREN)) {
                do {
                    arguments.push_back(parseExpression());
                } while (match(TokenType::COMMA));
            }
            consume(TokenType::RPAREN, "Expected ')' after arguments");
            return std::make_shared<CallNode>(name, arguments);
        }
        
        // Check if it's array access
        if (match(TokenType::LBRACKET)) {
            auto index = parseExpression();
            consume(TokenType::RBRACKET, "Expected ']' after array index");
            return std::make_shared<ArrayAccessNode>(name, index);
        }
        
        // It's a variable
        return std::make_shared<VariableNode>(name);
    }
    if (match(TokenType::LPAREN)) {
        auto expr = parseExpression();
        consume(TokenType::RPAREN, "Expected ')' after expression");
        return expr;
    }
    
    throw std::runtime_error("Expected expression, found '" + peek().value + "'");
}

// Constructor
Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), current(0) {}

std::shared_ptr<ASTNode> Parser::parse() {
    try {
        return parseAlgorithm();
    } catch (const std::runtime_error& e) {
        std::cerr << "Parsing error: " << e.what() << std::endl;
        return nullptr;
    }
}

void Parser::printAST(const std::shared_ptr<ASTNode>& node, int depth) {
    // ... (printAST implementation remains the same as before)
    // Keep your existing printAST implementation
}