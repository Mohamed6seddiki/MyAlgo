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

using namespace std;

// ========== TOKEN TYPES ==========
enum class TokenType {
    END_OF_FILE,
    IDENTIFIER, NUMBER, STRING,
    ALGORITHM, VAR, BEGIN, END,
    INTEGER, REAL, BOOLEAN,
    READ, WRITE,
    IF, THEN, ENDIF, FOR, FROM, TO, DO, ENDFOR,
    WHILE, ENDWHILE, REPEAT, UNTIL, ELSE,
    TRUE, FALSE,
    OR, AND, NOT,
    DIV, MOD,
    ASSIGN,
    PLUS, MINUS, MULTIPLY, DIVIDE,
    EQUAL, NOT_EQUAL, LESS, GREATER, LESS_EQUAL, GREATER_EQUAL,
    LPAREN, RPAREN, COMMA, COLON, SEMICOLON, PERIOD
};

struct Token {
    TokenType type;
    string value;   
    Token(TokenType t, const string& v = "") : type(t), value(v) {}
};

// ========== SYMBOL TABLE ==========
enum class Type { INTEGER, REAL, BOOLEAN, UNKNOWN, ERROR };

string typeToString(Type type) {
    switch (type) {
        case Type::INTEGER: return "integer";
        case Type::REAL: return "real";
        case Type::BOOLEAN: return "boolean";
        case Type::UNKNOWN: return "unknown";
        case Type::ERROR: return "error";
        default: return "unknown";
    }
}

class SymbolTable {
private:
    unordered_map<string, pair<Type, bool>> symbols;
    
public:
    bool declare(const string& name, Type type) {
        if (symbols.find(name) != symbols.end()) return false;
        symbols[name] = make_pair(type, false);
        return true;
    }
    
    bool setInitialized(const string& name) {
        auto it = symbols.find(name);
        if (it != symbols.end()) {
            it->second.second = true;
            return true;
        }
        return false;
    }
    
    Type getType(const string& name) const {
        auto it = symbols.find(name);
        if (it != symbols.end()) return it->second.first;
        return Type::UNKNOWN;
    }
    
    bool isDeclared(const string& name) const {
        return symbols.find(name) != symbols.end();
    }
    
    bool isInitialized(const string& name) const {
        auto it = symbols.find(name);
        if (it != symbols.end()) return it->second.second;
        return false;
    }
    
    void print() const {
        cout << "=== SYMBOL TABLE ===" << endl;
        for (const auto& entry : symbols) {
            cout << entry.first << " : " << typeToString(entry.second.first);
            if (entry.second.second) cout << " (initialized)";
            cout << endl;
        }
        cout << "===================" << endl;
    }
};

// ========== AST NODES ==========
class ASTNode { public: virtual ~ASTNode() = default; };

class VarDeclNode : public ASTNode {
public:
    vector<string> names;
    string typeName;
    VarDeclNode(const vector<string>& names, const string& typeName)
        : names(names), typeName(typeName) {}
};

class StatementNode : public ASTNode {};
class ExpressionNode : public ASTNode {};

class BlockNode : public StatementNode {
public:
    vector<shared_ptr<StatementNode>> statements;
    BlockNode(const vector<shared_ptr<StatementNode>>& statements)
        : statements(statements) {}
};

class AlgorithmNode : public ASTNode {
public:
    string name;
    vector<shared_ptr<VarDeclNode>> variables;
    shared_ptr<BlockNode> body;
    AlgorithmNode(const string& name, 
                  const vector<shared_ptr<VarDeclNode>>& variables,
                  const shared_ptr<BlockNode>& body)
        : name(name), variables(variables), body(body) {}
};

class AssignmentNode : public StatementNode {
public:
    string variableName;
    shared_ptr<ExpressionNode> expression;
    AssignmentNode(const string& variableName, 
                   const shared_ptr<ExpressionNode>& expression)
        : variableName(variableName), expression(expression) {}
};

class IfStatementNode : public StatementNode {
public:
    shared_ptr<ExpressionNode> condition;
    shared_ptr<BlockNode> thenBlock;
    shared_ptr<BlockNode> elseBlock;
    IfStatementNode(const shared_ptr<ExpressionNode>& condition,
                    const shared_ptr<BlockNode>& thenBlock,
                    const shared_ptr<BlockNode>& elseBlock = nullptr)
        : condition(condition), thenBlock(thenBlock), elseBlock(elseBlock) {}
};

class ForStatementNode : public StatementNode {
public:
    string variableName;
    shared_ptr<ExpressionNode> startExpr;
    shared_ptr<ExpressionNode> endExpr;
    shared_ptr<BlockNode> body;
    ForStatementNode(const string& variableName,
                     const shared_ptr<ExpressionNode>& startExpr,
                     const shared_ptr<ExpressionNode>& endExpr,
                     const shared_ptr<BlockNode>& body)
        : variableName(variableName), startExpr(startExpr), endExpr(endExpr), body(body) {}
};

class WhileStatementNode : public StatementNode {
public:
    shared_ptr<ExpressionNode> condition;
    shared_ptr<BlockNode> body;
    WhileStatementNode(const shared_ptr<ExpressionNode>& condition,
                       const shared_ptr<BlockNode>& body)
        : condition(condition), body(body) {}
};

class RepeatStatementNode : public StatementNode {
public:
    shared_ptr<BlockNode> body;
    shared_ptr<ExpressionNode> condition;
    RepeatStatementNode(const shared_ptr<BlockNode>& body,
                        const shared_ptr<ExpressionNode>& condition)
        : body(body), condition(condition) {}
};

class ReadStatementNode : public StatementNode {
public:
    string variableName;
    ReadStatementNode(const string& variableName) : variableName(variableName) {}
};

class WriteStatementNode : public StatementNode {
public:
    shared_ptr<ExpressionNode> expression;
    WriteStatementNode(const shared_ptr<ExpressionNode>& expression)
        : expression(expression) {}
};

class BinaryOpNode : public ExpressionNode {
public:
    shared_ptr<ExpressionNode> left;
    string op;
    shared_ptr<ExpressionNode> right;
    BinaryOpNode(const shared_ptr<ExpressionNode>& left,
                 const string& op,
                 const shared_ptr<ExpressionNode>& right)
        : left(left), op(op), right(right) {}
};

class UnaryOpNode : public ExpressionNode {
public:
    string op;
    shared_ptr<ExpressionNode> operand;
    UnaryOpNode(const string& op,
                const shared_ptr<ExpressionNode>& operand)
        : op(op), operand(operand) {}
};

class VariableNode : public ExpressionNode {
public:
    string name;
    VariableNode(const string& name) : name(name) {}
};

class NumberNode : public ExpressionNode {
public:
    string value;
    NumberNode(const string& value) : value(value) {}
};

class BooleanNode : public ExpressionNode {
public:
    bool value;
    BooleanNode(bool value) : value(value) {}
};

class StringNode : public ExpressionNode {
public:
    string value;
    StringNode(const string& value) : value(value) {}
};

class CallNode : public ExpressionNode {
public:
    string functionName;
    vector<shared_ptr<ExpressionNode>> arguments;
    CallNode(const string& functionName,
             const vector<shared_ptr<ExpressionNode>>& arguments = {})
        : functionName(functionName), arguments(arguments) {}
};

// ========== LEXER ==========
class Lexer {
private:
    string source;
    size_t position;
    char currentChar;
    map<string, TokenType> keywords;

    void initKeywords() {
        keywords["algorithm"] = TokenType::ALGORITHM;
        keywords["var"] = TokenType::VAR;
        keywords["begin"] = TokenType::BEGIN;
        keywords["end"] = TokenType::END;
        keywords["integer"] = TokenType::INTEGER;
        keywords["real"] = TokenType::REAL;
        keywords["boolean"] = TokenType::BOOLEAN;
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
        keywords["DIV"] = TokenType::DIV;
        keywords["MOD"] = TokenType::MOD;
    }

    void advance() {
        position++;
        if (position < source.length())
            currentChar = source[position];
        else
            currentChar = '\0';
    }

    void skipWhitespace() {
        while (currentChar != '\0' && isspace(currentChar)) {
            advance();
        }
    }

    Token number() {
        string value;
        while (currentChar != '\0' && isdigit(currentChar)) {
            value += currentChar;
            advance();
        }
        
        if (currentChar == '.') {
            value += currentChar;
            advance();
            while (currentChar != '\0' && isdigit(currentChar)) {
                value += currentChar;
                advance();
            }
        }
        
        return Token(TokenType::NUMBER, value);
    }

    Token stringLiteral() {
        string value;
        advance();
        
        while (currentChar != '\0' && currentChar != '"') {
            value += currentChar;
            advance();
        }
        
        if (currentChar == '"') {
            advance();
            return Token(TokenType::STRING, value);
        }
        
        return Token(TokenType::STRING, value);
    }

    Token identifier() {
        string value;
        while (currentChar != '\0' && (isalnum(currentChar) || currentChar == '_')) {
            value += currentChar;
            advance();
        }

        if (keywords.find(value) != keywords.end()) {
            return Token(keywords[value], value);
        }

        return Token(TokenType::IDENTIFIER, value);
    }

public:
    Lexer(const string& text) : source(text), position(0) {
        currentChar = source.empty() ? '\0' : source[0];
        initKeywords();
    }

    vector<Token> tokenize() {
        vector<Token> tokens;

        while (currentChar != '\0') {
            if (isspace(currentChar)) {
                skipWhitespace();
                continue;
            }

            if (currentChar == '"') {
                tokens.push_back(stringLiteral());
                continue;
            }

            if (isalpha(currentChar) || currentChar == '_') {
                tokens.push_back(identifier());
                continue;
            }

            if (isdigit(currentChar)) {
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

            advance();
        }

        tokens.emplace_back(TokenType::END_OF_FILE);
        return tokens;
    }
};

// ========== PARSER ==========
class Parser {
private:
    vector<Token> tokens;
    size_t current;

    Token peek() const {
        if (current >= tokens.size()) return Token(TokenType::END_OF_FILE);
        return tokens[current];
    }

    Token advance() {
        if (current < tokens.size()) current++;
        return previous();
    }

    Token previous() const {
        if (current == 0) return Token(TokenType::END_OF_FILE);
        return tokens[current - 1];
    }

    bool isAtEnd() const { return peek().type == TokenType::END_OF_FILE; }

    bool check(TokenType type) const {
        if (isAtEnd()) return false;
        return peek().type == type;
    }

    bool match(TokenType type) {
        if (check(type)) { advance(); return true; }
        return false;
    }

    Token consume(TokenType type, const string& message) {
        if (check(type)) return advance();
        throw runtime_error("Parse error: " + message);
    }

    shared_ptr<ASTNode> parseAlgorithm() {
        consume(TokenType::ALGORITHM, "Expected 'algorithm'");
        string name = consume(TokenType::IDENTIFIER, "Expected algorithm name").value;
        
        vector<shared_ptr<VarDeclNode>> variables;
        if (match(TokenType::VAR)) {
            while (check(TokenType::IDENTIFIER)) {
                string varName = consume(TokenType::IDENTIFIER, "Expected variable name").value;
                vector<string> names = {varName};
                while (match(TokenType::COMMA)) {
                    names.push_back(consume(TokenType::IDENTIFIER, "Expected variable name").value);
                }
                consume(TokenType::COLON, "Expected ':' after variable name(s)");
                
                Token typeToken = advance();
                string typeName;
                switch (typeToken.type) {
                    case TokenType::INTEGER: typeName = "integer"; break;
                    case TokenType::REAL: typeName = "real"; break;
                    case TokenType::BOOLEAN: typeName = "boolean"; break;
                    default: throw runtime_error("Expected type (integer, real, or boolean)");
                }
                
                variables.push_back(make_shared<VarDeclNode>(names, typeName));
                consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");
            }
        }
        
        consume(TokenType::BEGIN, "Expected 'begin'");
        auto block = parseStatementBlock();
        consume(TokenType::END, "Expected 'end'");
        consume(TokenType::PERIOD, "Expected '.' at end of algorithm");
        
        return make_shared<AlgorithmNode>(name, variables, block);
    }

    shared_ptr<BlockNode> parseStatementBlock() {
        vector<shared_ptr<StatementNode>> statements;
        
        while (!check(TokenType::END) && !check(TokenType::ENDIF) && 
               !check(TokenType::ENDFOR) && !check(TokenType::ENDWHILE) &&
               !check(TokenType::UNTIL) && !check(TokenType::ELSE) && !isAtEnd()) {
            
            auto stmt = parseStatement();
            if (stmt) {
                statements.push_back(stmt);
                if (check(TokenType::SEMICOLON)) advance();
            }
        }
        
        return make_shared<BlockNode>(statements);
    }

    shared_ptr<StatementNode> parseStatement() {
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
            throw runtime_error("Expected statement");
        }
    }

    shared_ptr<AssignmentNode> parseAssignment() {
        string identifier = consume(TokenType::IDENTIFIER, "Expected identifier").value;
        consume(TokenType::ASSIGN, "Expected ':=' for assignment");
        auto expr = parseExpression();
        return make_shared<AssignmentNode>(identifier, expr);
    }

    shared_ptr<IfStatementNode> parseIfStatement() {
        auto condition = parseExpression();
        consume(TokenType::THEN, "Expected 'then' after if condition");
        auto thenBlock = parseStatementBlock();
        
        shared_ptr<BlockNode> elseBlock = nullptr;
        if (match(TokenType::ELSE)) {
            elseBlock = parseStatementBlock();
        }
        
        consume(TokenType::ENDIF, "Expected 'endif'");
        return make_shared<IfStatementNode>(condition, thenBlock, elseBlock);
    }

    shared_ptr<ForStatementNode> parseForStatement() {
        string varName = consume(TokenType::IDENTIFIER, "Expected loop variable").value;
        consume(TokenType::FROM, "Expected 'from' in for loop");
        auto startExpr = parseExpression();
        consume(TokenType::TO, "Expected 'to' in for loop");
        auto endExpr = parseExpression();
        consume(TokenType::DO, "Expected 'do' in for loop");
        auto body = parseStatementBlock();
        consume(TokenType::ENDFOR, "Expected 'endfor'");
        return make_shared<ForStatementNode>(varName, startExpr, endExpr, body);
    }

    shared_ptr<WhileStatementNode> parseWhileStatement() {
        auto condition = parseExpression();
        consume(TokenType::DO, "Expected 'do' after while condition");
        auto body = parseStatementBlock();
        consume(TokenType::ENDWHILE, "Expected 'endwhile'");
        return make_shared<WhileStatementNode>(condition, body);
    }

    shared_ptr<RepeatStatementNode> parseRepeatStatement() {
        auto body = parseStatementBlock();
        consume(TokenType::UNTIL, "Expected 'until' after repeat block");
        auto condition = parseExpression();
        return make_shared<RepeatStatementNode>(body, condition);
    }

    shared_ptr<ReadStatementNode> parseReadStatement() {
        consume(TokenType::LPAREN, "Expected '(' after read");
        string varName = consume(TokenType::IDENTIFIER, "Expected variable name in read").value;
        consume(TokenType::RPAREN, "Expected ')' after read variable");
        return make_shared<ReadStatementNode>(varName);
    }

    shared_ptr<WriteStatementNode> parseWriteStatement() {
        consume(TokenType::LPAREN, "Expected '(' after write");
        auto expr = parseExpression();
        consume(TokenType::RPAREN, "Expected ')' after write expression");
        return make_shared<WriteStatementNode>(expr);
    }

    shared_ptr<ExpressionNode> parseExpression() {
        return parseLogicalOr();
    }

    shared_ptr<ExpressionNode> parseLogicalOr() {
        auto expr = parseLogicalAnd();
        while (match(TokenType::OR)) {
            Token op = previous();
            auto right = parseLogicalAnd();
            expr = make_shared<BinaryOpNode>(expr, op.value, right);
        }
        return expr;
    }

    shared_ptr<ExpressionNode> parseLogicalAnd() {
        auto expr = parseEquality();
        while (match(TokenType::AND)) {
            Token op = previous();
            auto right = parseEquality();
            expr = make_shared<BinaryOpNode>(expr, op.value, right);
        }
        return expr;
    }

    shared_ptr<ExpressionNode> parseEquality() {
        auto expr = parseComparison();
        while (match(TokenType::EQUAL) || match(TokenType::NOT_EQUAL)) {
            Token op = previous();
            auto right = parseComparison();
            expr = make_shared<BinaryOpNode>(expr, op.value, right);
        }
        return expr;
    }

    shared_ptr<ExpressionNode> parseComparison() {
        auto expr = parseTerm();
        while (match(TokenType::LESS) || match(TokenType::LESS_EQUAL) ||
               match(TokenType::GREATER) || match(TokenType::GREATER_EQUAL)) {
            Token op = previous();
            auto right = parseTerm();
            expr = make_shared<BinaryOpNode>(expr, op.value, right);
        }
        return expr;
    }

    shared_ptr<ExpressionNode> parseTerm() {
        auto expr = parseFactor();
        while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
            Token op = previous();
            auto right = parseFactor();
            expr = make_shared<BinaryOpNode>(expr, op.value, right);
        }
        return expr;
    }

    shared_ptr<ExpressionNode> parseFactor() {
        auto expr = parseUnary();
        while (match(TokenType::MULTIPLY) || match(TokenType::DIVIDE) ||
               match(TokenType::DIV) || match(TokenType::MOD)) {
            Token op = previous();
            auto right = parseUnary();
            expr = make_shared<BinaryOpNode>(expr, op.value, right);
        }
        return expr;
    }

    shared_ptr<ExpressionNode> parseUnary() {
        if (match(TokenType::NOT) || match(TokenType::MINUS)) {
            Token op = previous();
            auto right = parseUnary();
            return make_shared<UnaryOpNode>(op.value, right);
        }
        return parsePrimary();
    }

    shared_ptr<ExpressionNode> parsePrimary() {
        if (match(TokenType::NUMBER)) {
            return make_shared<NumberNode>(previous().value);
        }
        
        if (match(TokenType::STRING)) {
            return make_shared<StringNode>(previous().value);
        }
        
        if (match(TokenType::TRUE)) {
            return make_shared<BooleanNode>(true);
        }
        
        if (match(TokenType::FALSE)) {
            return make_shared<BooleanNode>(false);
        }
        
        if (match(TokenType::IDENTIFIER)) {
            string name = previous().value;
            if (match(TokenType::LPAREN)) {
                vector<shared_ptr<ExpressionNode>> args;
                if (!check(TokenType::RPAREN)) {
                    do {
                        args.push_back(parseExpression());
                    } while (match(TokenType::COMMA));
                }
                consume(TokenType::RPAREN, "Expected ')' after arguments");
                return make_shared<CallNode>(name, args);
            }
            return make_shared<VariableNode>(name);
        }
        
        if (match(TokenType::LPAREN)) {
            auto expr = parseExpression();
            consume(TokenType::RPAREN, "Expected ')' after expression");
            return expr;
        }
        
        throw runtime_error("Expected expression");
    }

public:
    Parser(const vector<Token>& tokens) : tokens(tokens), current(0) {}
    
    shared_ptr<ASTNode> parse() {
        try {
            return parseAlgorithm();
        } catch (const runtime_error& e) {
            cerr << "Parsing failed: " << e.what() << endl;
            throw;
        }
    }
    
    void printAST(const shared_ptr<ASTNode>& node, int depth = 0) {
        string indent(depth * 2, ' ');
        
        if (!node) {
            cout << indent << "(null)" << endl;
            return;
        }
        
        if (auto algo = dynamic_pointer_cast<AlgorithmNode>(node)) {
            cout << indent << "Algorithm: " << algo->name << endl;
            cout << indent << "Variables:" << endl;
            for (const auto& var : algo->variables) {
                printAST(var, depth + 1);
            }
            cout << indent << "Body:" << endl;
            printAST(algo->body, depth + 1);
        }
        else if (auto varDecl = dynamic_pointer_cast<VarDeclNode>(node)) {
            cout << indent << "VarDecl: ";
            for (size_t i = 0; i < varDecl->names.size(); i++) {
                if (i > 0) cout << ", ";
                cout << varDecl->names[i];
            }
            cout << " : " << varDecl->typeName << endl;
        }
        else if (auto block = dynamic_pointer_cast<BlockNode>(node)) {
            cout << indent << "Block (" << block->statements.size() << " statements):" << endl;
            for (const auto& stmt : block->statements) {
                printAST(stmt, depth + 1);
            }
        }
        else if (auto assign = dynamic_pointer_cast<AssignmentNode>(node)) {
            cout << indent << "Assignment: " << assign->variableName << " =" << endl;
            printAST(assign->expression, depth + 1);
        }
        else if (auto ifStmt = dynamic_pointer_cast<IfStatementNode>(node)) {
            cout << indent << "If:" << endl;
            cout << indent << "  Condition:" << endl;
            printAST(ifStmt->condition, depth + 2);
            cout << indent << "  Then:" << endl;
            printAST(ifStmt->thenBlock, depth + 2);
            if (ifStmt->elseBlock) {
                cout << indent << "  Else:" << endl;
                printAST(ifStmt->elseBlock, depth + 2);
            }
        }
        else if (auto forStmt = dynamic_pointer_cast<ForStatementNode>(node)) {
            cout << indent << "For: " << forStmt->variableName << endl;
            cout << indent << "  From:" << endl;
            printAST(forStmt->startExpr, depth + 2);
            cout << indent << "  To:" << endl;
            printAST(forStmt->endExpr, depth + 2);
            cout << indent << "  Body:" << endl;
            printAST(forStmt->body, depth + 2);
        }
        else if (auto whileStmt = dynamic_pointer_cast<WhileStatementNode>(node)) {
            cout << indent << "While:" << endl;
            cout << indent << "  Condition:" << endl;
            printAST(whileStmt->condition, depth + 2);
            cout << indent << "  Body:" << endl;
            printAST(whileStmt->body, depth + 2);
        }
        else if (auto repeatStmt = dynamic_pointer_cast<RepeatStatementNode>(node)) {
            cout << indent << "Repeat-Until:" << endl;
            cout << indent << "  Body:" << endl;
            printAST(repeatStmt->body, depth + 2);
            cout << indent << "  Condition:" << endl;
            printAST(repeatStmt->condition, depth + 2);
        }
        else if (auto readStmt = dynamic_pointer_cast<ReadStatementNode>(node)) {
            cout << indent << "Read: " << readStmt->variableName << endl;
        }
        else if (auto writeStmt = dynamic_pointer_cast<WriteStatementNode>(node)) {
            cout << indent << "Write:" << endl;
            printAST(writeStmt->expression, depth + 1);
        }
        else if (auto binOp = dynamic_pointer_cast<BinaryOpNode>(node)) {
            cout << indent << "BinaryOp: " << binOp->op << endl;
            cout << indent << "  Left:" << endl;
            printAST(binOp->left, depth + 2);
            cout << indent << "  Right:" << endl;
            printAST(binOp->right, depth + 2);
        }
        else if (auto unaryOp = dynamic_pointer_cast<UnaryOpNode>(node)) {
            cout << indent << "UnaryOp: " << unaryOp->op << endl;
            printAST(unaryOp->operand, depth + 1);
        }
        else if (auto var = dynamic_pointer_cast<VariableNode>(node)) {
            cout << indent << "Variable: " << var->name << endl;
        }
        else if (auto num = dynamic_pointer_cast<NumberNode>(node)) {
            cout << indent << "Number: " << num->value << endl;
        }
        else if (auto boolean = dynamic_pointer_cast<BooleanNode>(node)) {
            cout << indent << "Boolean: " << (boolean->value ? "true" : "false") << endl;
        }
        else if (auto str = dynamic_pointer_cast<StringNode>(node)) {
            cout << indent << "String: \"" << str->value << "\"" << endl;
        }
        else if (auto call = dynamic_pointer_cast<CallNode>(node)) {
            cout << indent << "Call: " << call->functionName << " (" << call->arguments.size() << " args)" << endl;
        }
        else {
            cout << indent << "Unknown AST node type" << endl;
        }
    }
};

// ========== SEMANTIC ANALYZER ==========
class SemanticAnalyzer {
private:
    SymbolTable symbolTable;
    vector<string> errors;
    
    void addError(const string& message) {
        errors.push_back(message);
    }
    
    Type stringToType(const string& typeStr) {
        if (typeStr == "integer") return Type::INTEGER;
        if (typeStr == "real") return Type::REAL;
        if (typeStr == "boolean") return Type::BOOLEAN;
        return Type::UNKNOWN;
    }
    
    Type checkExpression(const shared_ptr<ExpressionNode>& expr) {
        if (auto num = dynamic_pointer_cast<NumberNode>(expr)) {
            if (num->value.find('.') != string::npos) return Type::REAL;
            return Type::INTEGER;
        }
        else if (auto str = dynamic_pointer_cast<StringNode>(expr)) {
            return Type::UNKNOWN;
        }
        else if (auto boolean = dynamic_pointer_cast<BooleanNode>(expr)) {
            return Type::BOOLEAN;
        }
        else if (auto var = dynamic_pointer_cast<VariableNode>(expr)) {
            if (!symbolTable.isDeclared(var->name)) {
                addError("Undeclared variable: " + var->name);
                return Type::ERROR;
            }
            return symbolTable.getType(var->name);
        }
        else if (auto binOp = dynamic_pointer_cast<BinaryOpNode>(expr)) {
            Type leftType = checkExpression(binOp->left);
            Type rightType = checkExpression(binOp->right);
            
            if (binOp->op == "+" || binOp->op == "-" || binOp->op == "*" || binOp->op == "/") {
                if (leftType == Type::INTEGER && rightType == Type::INTEGER) return Type::INTEGER;
                if ((leftType == Type::INTEGER || leftType == Type::REAL) && 
                    (rightType == Type::INTEGER || rightType == Type::REAL)) return Type::REAL;
                addError("Type mismatch in arithmetic operation");
                return Type::ERROR;
            }
            else if (binOp->op == "DIV" || binOp->op == "MOD") {
                if (leftType == Type::INTEGER && rightType == Type::INTEGER) return Type::INTEGER;
                addError("DIV and MOD require integer operands");
                return Type::ERROR;
            }
            else if (binOp->op == "=" || binOp->op == "<>" || binOp->op == "<" || binOp->op == ">" ||
                     binOp->op == "<=" || binOp->op == ">=") {
                if ((leftType == Type::INTEGER || leftType == Type::REAL) && 
                    (rightType == Type::INTEGER || rightType == Type::REAL)) return Type::BOOLEAN;
                if (leftType == Type::BOOLEAN && rightType == Type::BOOLEAN) return Type::BOOLEAN;
                addError("Comparison operators require compatible operands");
                return Type::ERROR;
            }
            else if (binOp->op == "and" || binOp->op == "or") {
                if (leftType == Type::BOOLEAN && rightType == Type::BOOLEAN) return Type::BOOLEAN;
                addError("Logical operators require boolean operands");
                return Type::ERROR;
            }
            
            addError("Unsupported operator: " + binOp->op);
            return Type::ERROR;
        }
        else if (auto unaryOp = dynamic_pointer_cast<UnaryOpNode>(expr)) {
            Type operandType = checkExpression(unaryOp->operand);
            
            if (unaryOp->op == "-") {
                if (operandType == Type::INTEGER || operandType == Type::REAL) return operandType;
                addError("Unary minus requires numeric operand");
                return Type::ERROR;
            }
            else if (unaryOp->op == "not") {
                if (operandType == Type::BOOLEAN) return Type::BOOLEAN;
                addError("Logical NOT requires boolean operand");
                return Type::ERROR;
            }
            
            addError("Unsupported unary operator: " + unaryOp->op);
            return Type::ERROR;
        }
        else if (auto call = dynamic_pointer_cast<CallNode>(expr)) {
            if (call->functionName == "write") {
                if (!call->arguments.empty()) checkExpression(call->arguments[0]);
                return Type::UNKNOWN;
            }
            addError("Unknown function: " + call->functionName);
            return Type::ERROR;
        }
        
        addError("Unknown expression type");
        return Type::ERROR;
    }
    
    void checkStatement(const shared_ptr<StatementNode>& stmt) {
        if (auto assign = dynamic_pointer_cast<AssignmentNode>(stmt)) {
            if (!symbolTable.isDeclared(assign->variableName)) {
                addError("Assignment to undeclared variable: " + assign->variableName);
                return;
            }
            
            Type varType = symbolTable.getType(assign->variableName);
            Type exprType = checkExpression(assign->expression);
            
            if (exprType == Type::ERROR) return;
            
            if (varType == Type::INTEGER && exprType == Type::INTEGER) {
                // OK
            }
            else if (varType == Type::REAL && (exprType == Type::INTEGER || exprType == Type::REAL)) {
                // OK
            }
            else if (varType == Type::BOOLEAN && exprType == Type::BOOLEAN) {
                // OK
            }
            else {
                addError("Type mismatch in assignment to " + assign->variableName);
                return;
            }
            
            symbolTable.setInitialized(assign->variableName);
        }
        else if (auto ifStmt = dynamic_pointer_cast<IfStatementNode>(stmt)) {
            Type condType = checkExpression(ifStmt->condition);
            if (condType != Type::BOOLEAN && condType != Type::ERROR) {
                addError("IF condition must be boolean");
            }
            
            checkBlock(ifStmt->thenBlock);
            if (ifStmt->elseBlock) checkBlock(ifStmt->elseBlock);
        }
        else if (auto forStmt = dynamic_pointer_cast<ForStatementNode>(stmt)) {
            if (!symbolTable.isDeclared(forStmt->variableName)) {
                symbolTable.declare(forStmt->variableName, Type::INTEGER);
            } else {
                Type varType = symbolTable.getType(forStmt->variableName);
                if (varType != Type::INTEGER) {
                    addError("FOR loop variable must be integer");
                }
            }
            
            symbolTable.setInitialized(forStmt->variableName);
            
            Type startType = checkExpression(forStmt->startExpr);
            Type endType = checkExpression(forStmt->endExpr);
            
            if (startType != Type::INTEGER && startType != Type::ERROR) {
                addError("FOR start expression must be integer");
            }
            if (endType != Type::INTEGER && endType != Type::ERROR) {
                addError("FOR end expression must be integer");
            }
            
            checkBlock(forStmt->body);
        }
        else if (auto whileStmt = dynamic_pointer_cast<WhileStatementNode>(stmt)) {
            Type condType = checkExpression(whileStmt->condition);
            if (condType != Type::BOOLEAN && condType != Type::ERROR) {
                addError("WHILE condition must be boolean");
            }
            checkBlock(whileStmt->body);
        }
        else if (auto repeatStmt = dynamic_pointer_cast<RepeatStatementNode>(stmt)) {
            checkBlock(repeatStmt->body);
            Type condType = checkExpression(repeatStmt->condition);
            if (condType != Type::BOOLEAN && condType != Type::ERROR) {
                addError("REPEAT-UNTIL condition must be boolean");
            }
        }
        else if (auto readStmt = dynamic_pointer_cast<ReadStatementNode>(stmt)) {
            if (!symbolTable.isDeclared(readStmt->variableName)) {
                addError("READ statement: undeclared variable: " + readStmt->variableName);
                return;
            }
            symbolTable.setInitialized(readStmt->variableName);
        }
        else if (auto writeStmt = dynamic_pointer_cast<WriteStatementNode>(stmt)) {
            checkExpression(writeStmt->expression);
        }
    }
    
    void checkBlock(const shared_ptr<BlockNode>& block) {
        if (!block) return;
        for (const auto& stmt : block->statements) {
            if (stmt) checkStatement(stmt);
        }
    }
    
public:
    SemanticAnalyzer() {}
    
    bool analyze(const shared_ptr<ASTNode>& ast) {
        errors.clear();
        
        if (auto algo = dynamic_pointer_cast<AlgorithmNode>(ast)) {
            for (const auto& varDecl : algo->variables) {
                for (const auto& varName : varDecl->names) {
                    Type type = stringToType(varDecl->typeName);
                    if (!symbolTable.declare(varName, type)) {
                        addError("Duplicate variable declaration: " + varName);
                    }
                }
            }
            
            checkBlock(algo->body);
        }
        
        return errors.empty();
    }
    
    void printSymbolTable() const {
        symbolTable.print();
    }
    
    void printErrors() const {
        if (errors.empty()) {
            cout << "No semantic errors found." << endl;
            return;
        }
        
        cout << "=== SEMANTIC ERRORS ===" << endl;
        for (const auto& error : errors) {
            cout << "Error: " << error << endl;
        }
        cout << "======================" << endl;
    }
    
    bool hasErrors() const {
        return !errors.empty();
    }
};

// ========== CODE GENERATOR ==========
class CodeGenerator {
private:
    shared_ptr<ASTNode> ast;
    ofstream outputFile;
    string outputFilename;
    int indentLevel;
    
    string indent() const {
        return string(indentLevel * 4, ' ');
    }
    
    string sanitizeIdentifier(const string& name) {
        return name;
    }
    
    void generateIncludes() {
        outputFile << "// Generated by MyAlgo Compiler\n";
        outputFile << "// DO NOT EDIT THIS FILE DIRECTLY\n\n";
        outputFile << "#include <iostream>\n";
        outputFile << "#include <cmath>\n";
        outputFile << "#include <string>\n";
        outputFile << "using namespace std;\n\n";
    }
    
    void generateExpression(const shared_ptr<ExpressionNode>& expr) {
        if (auto num = dynamic_pointer_cast<NumberNode>(expr)) {
            outputFile << num->value;
        }
        else if (auto str = dynamic_pointer_cast<StringNode>(expr)) {
            outputFile << "\"" << str->value << "\"";
        }
        else if (auto boolean = dynamic_pointer_cast<BooleanNode>(expr)) {
            outputFile << (boolean->value ? "true" : "false");
        }
        else if (auto var = dynamic_pointer_cast<VariableNode>(expr)) {
            outputFile << sanitizeIdentifier(var->name);
        }
        else if (auto binOp = dynamic_pointer_cast<BinaryOpNode>(expr)) {
            string op = binOp->op;
            if (op == "DIV") {
                outputFile << "(";
                generateExpression(binOp->left);
                outputFile << " / ";
                generateExpression(binOp->right);
                outputFile << ")";
            }
            else if (op == "MOD") {
                outputFile << "(";
                generateExpression(binOp->left);
                outputFile << " % ";
                generateExpression(binOp->right);
                outputFile << ")";
            }
            else if (op == "=") {
                outputFile << "(";
                generateExpression(binOp->left);
                outputFile << " == ";
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
        else if (auto unaryOp = dynamic_pointer_cast<UnaryOpNode>(expr)) {
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
        else if (auto call = dynamic_pointer_cast<CallNode>(expr)) {
            if (call->functionName == "write") {
                outputFile << "cout << ";
                if (!call->arguments.empty()) generateExpression(call->arguments[0]);
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
    
    void generateStatement(const shared_ptr<StatementNode>& stmt) {
        if (auto assign = dynamic_pointer_cast<AssignmentNode>(stmt)) {
            outputFile << indent() << sanitizeIdentifier(assign->variableName) << " = ";
            generateExpression(assign->expression);
            outputFile << ";\n";
        }
        else if (auto ifStmt = dynamic_pointer_cast<IfStatementNode>(stmt)) {
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
        else if (auto forStmt = dynamic_pointer_cast<ForStatementNode>(stmt)) {
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
        else if (auto whileStmt = dynamic_pointer_cast<WhileStatementNode>(stmt)) {
            outputFile << indent() << "while (";
            generateExpression(whileStmt->condition);
            outputFile << ") {\n";
            
            indentLevel++;
            generateBlock(whileStmt->body);
            indentLevel--;
            
            outputFile << indent() << "}\n";
        }
        else if (auto repeatStmt = dynamic_pointer_cast<RepeatStatementNode>(stmt)) {
            outputFile << indent() << "do {\n";
            
            indentLevel++;
            generateBlock(repeatStmt->body);
            indentLevel--;
            
            outputFile << indent() << "} while (!(";
            generateExpression(repeatStmt->condition);
            outputFile << "));\n";
        }
        else if (auto readStmt = dynamic_pointer_cast<ReadStatementNode>(stmt)) {
            outputFile << indent() << "cin >> " << sanitizeIdentifier(readStmt->variableName) << ";\n";
        }
        else if (auto writeStmt = dynamic_pointer_cast<WriteStatementNode>(stmt)) {
            outputFile << indent() << "cout << ";
            generateExpression(writeStmt->expression);
            outputFile << " << endl;\n";
        }
    }
    
    void generateBlock(const shared_ptr<BlockNode>& block) {
        if (!block) return;
        for (const auto& stmt : block->statements) {
            if (stmt) generateStatement(stmt);
        }
    }
    
    void generateVariableDeclarations(const vector<shared_ptr<VarDeclNode>>& variables) {
        if (variables.empty()) return;
        
        outputFile << indent() << "// Variable declarations\n";
        for (const auto& varDecl : variables) {
            string cppTypeStr;
            if (varDecl->typeName == "integer") cppTypeStr = "int";
            else if (varDecl->typeName == "real") cppTypeStr = "double";
            else if (varDecl->typeName == "boolean") cppTypeStr = "bool";
            else cppTypeStr = "int";
            
            outputFile << indent() << cppTypeStr << " ";
            for (size_t i = 0; i < varDecl->names.size(); i++) {
                if (i > 0) outputFile << ", ";
                outputFile << sanitizeIdentifier(varDecl->names[i]);
            }
            outputFile << ";\n";
        }
        outputFile << "\n";
    }
    
    void generateAlgorithm(const shared_ptr<AlgorithmNode>& algo) {
        outputFile << "// Generated C++ code from MyAlgo algorithm: " << algo->name << "\n";
        outputFile << "// Original algorithm: " << algo->name << "\n\n";
        
        generateIncludes();
        
        outputFile << "int main() {\n";
        indentLevel++;
        
        generateVariableDeclarations(algo->variables);
        generateBlock(algo->body);
        
        outputFile << "\n" << indent() << "return 0;\n";
        indentLevel--;
        outputFile << "}\n";
    }
    
public:
    CodeGenerator(const shared_ptr<ASTNode>& ast, 
                  const string& outputFilename = "output.cpp")
        : ast(ast), outputFilename(outputFilename), indentLevel(0) {}
    
    bool generate() {
        outputFile.open(outputFilename);
        if (!outputFile.is_open()) {
            cerr << "Error: Could not open output file " << outputFilename << endl;
            return false;
        }
        
        try {
            if (auto algo = dynamic_pointer_cast<AlgorithmNode>(ast)) {
                generateAlgorithm(algo);
            } else {
                cerr << "Error: AST root is not an AlgorithmNode" << endl;
                return false;
            }
            
            outputFile.close();
            cout << "C++ code generated successfully: " << outputFilename << endl;
            return true;
        }
        catch (const exception& e) {
            cerr << "Error during code generation: " << e.what() << endl;
            outputFile.close();
            return false;
        }
    }
};

// ========== MAIN FUNCTION ==========
string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::STRING: return "STRING";
        case TokenType::ALGORITHM: return "ALGORITHM";
        case TokenType::VAR: return "VAR";
        case TokenType::BEGIN: return "BEGIN";
        case TokenType::END: return "END";
        case TokenType::INTEGER: return "INTEGER";
        case TokenType::REAL: return "REAL";
        case TokenType::BOOLEAN: return "BOOLEAN";
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

        // Syntactic analysis
        cout << "\n2. Running syntactic analysis..." << endl;
        Parser parser(tokens);
        shared_ptr<ASTNode> ast = parser.parse();
        cout << "   Parsing successful!" << endl;

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