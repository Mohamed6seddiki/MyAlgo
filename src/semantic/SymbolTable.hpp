#ifndef SYMBOLTABLE_HPP
#define SYMBOLTABLE_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <iostream>

enum class Type {
    INTEGER,
    REAL,
    BOOLEAN,
    STRING,
    UNKNOWN,
    ERROR
};

std::string typeToString(Type type) {
    switch (type) {
        case Type::INTEGER: return "integer";
        case Type::REAL: return "real";
        case Type::BOOLEAN: return "boolean";
        case Type::STRING: return "string";
        case Type::UNKNOWN: return "unknown";
        case Type::ERROR: return "error";
        default: return "unknown";
    }
}

class Symbol {
public:
    std::string name;
    Type type;
    bool initialized;
    bool isParameter;
    
    Symbol(const std::string& name, Type type, bool initialized = false, bool isParameter = false)
        : name(name), type(type), initialized(initialized), isParameter(isParameter) {}
};

class SymbolTable {
private:
    std::unordered_map<std::string, Symbol> symbols;
    std::vector<std::string> symbolOrder; // To maintain insertion order
    
public:
    bool declare(const std::string& name, Type type, bool isParameter = false) {
        if (symbols.find(name) != symbols.end()) {
            return false; // Already declared
        }
        symbols.emplace(name, Symbol(name, type, false, isParameter));
        symbolOrder.push_back(name);
        return true;
    }
    
    bool setInitialized(const std::string& name) {
        auto it = symbols.find(name);
        if (it != symbols.end()) {
            it->second.initialized = true;
            return true;
        }
        return false;
    }
    
    Type getType(const std::string& name) const {
        auto it = symbols.find(name);
        if (it != symbols.end()) {
            return it->second.type;
        }
        return Type::UNKNOWN;
    }
    
    bool isDeclared(const std::string& name) const {
        return symbols.find(name) != symbols.end();
    }
    
    bool isInitialized(const std::string& name) const {
        auto it = symbols.find(name);
        if (it != symbols.end()) {
            return it->second.initialized;
        }
        return false;
    }
    
    bool isParameter(const std::string& name) const {
        auto it = symbols.find(name);
        if (it != symbols.end()) {
            return it->second.isParameter;
        }
        return false;
    }
    
    void print() const {
        std::cout << "=== SYMBOL TABLE ===" << std::endl;
        
        for (const auto& name : symbolOrder) {
            auto it = symbols.find(name);
            if (it != symbols.end()) {
                const Symbol& symbol = it->second;
                std::cout << name << " : " << typeToString(symbol.type);
                if (symbol.isParameter) {
                    std::cout << " (parameter)";
                }
                if (symbol.initialized) {
                    std::cout << " (initialized)";
                }
                std::cout << std::endl;
            }
        }
        std::cout << "===================" << std::endl;
    }
    
    void clear() {
        symbols.clear();
        symbolOrder.clear();
    }
};

#endif