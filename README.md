# MyAlgo Compiler

![Version](https://img.shields.io/badge/version-1.0-blue)
![Language](https://img.shields.io/badge/language-C++-orange)
![License](https://img.shields.io/badge/license-MIT-green)
![Status](https://img.shields.io/badge/status-Active-brightgreen)

**MyAlgo** is a complete compiler that translates Pascal-like algorithmic pseudocode into executable C++ programs. Designed as an educational tool, it demonstrates the full compilation pipeline from lexical analysis to code generation.

---

## 🎯 Overview

MyAlgo bridges the gap between algorithmic thinking and practical programming by compiling university-style pseudocode into optimized C++ code. The compiler implements a complete pipeline including:

- **Lexical Analysis** - Tokenization of source code
- **Syntax Analysis** - Parsing and AST construction
- **Semantic Analysis** - Type checking and validation
- **Code Generation** - C++ code output

---

## ✨ Features

### Language Support
- **Data Types**: `integer`, `real`, `boolean`, `string`
- **Variables**: Declaration and assignment
- **Operators**: 
  - Arithmetic: `+`, `-`, `*`, `/`, `DIV`, `MOD`
  - Relational: `=`, `<>`, `<`, `>`, `<=`, `>=`
  - Logical: `and`, `or`, `not`
- **Control Structures**:
  - Conditionals: `if-then-else-endif`
  - Loops: `for-from-to-do-endfor`, `while-do-endwhile`, `repeat-until`
- **I/O Operations**: `read`, `write`
- **Comments**: Single-line comments with `//`

### Compiler Features
- Full lexical and syntax analysis
- Abstract Syntax Tree (AST) generation
- Symbol table management
- Type checking and semantic validation
- C++ code generation with optimization
- Comprehensive error reporting

---

## 📋 Requirements

- **Operating System**: Windows, Linux, or macOS
- **Compiler**: g++ (C++11 or later)
- **Terminal**: Command Prompt, PowerShell, or Unix shell

---

## 🚀 Quick Start

### Installation

1. **Clone or download** the repository
2. **Navigate** to the project directory

### Compilation

#### Windows (PowerShell/CMD)
```bash
g++ main.cpp lexer.cpp parser.cpp -o myalgo
```

#### Linux / macOS
```bash
g++ main.cpp lexer.cpp parser.cpp -o myalgo
chmod +x myalgo
```

### Usage

#### Windows
```bash
.\myalgo test.algo
```

#### Linux / macOS
```bash
./myalgo test.algo
```

This will:
1. Compile `test.algo` to C++ code (`test.cpp`)
2. Automatically compile the C++ code to an executable (`test.exe` or `test`)
3. Execute the resulting program

---

## 📝 Language Syntax

### Program Structure

```pascal
algorithm ProgramName
var
    variable1, variable2 : integer;
    variable3 : real;
    flag : boolean;
begin
    // Your code here
end.
```

### Variable Declaration

```pascal
var
    x, y, z : integer;
    temperature : real;
    isValid : boolean;
    message : string;
```

### Assignment

```pascal
x := 10;
y := x + 5;
temperature := 98.6;
isValid := true;
```

### Arithmetic Operations

```pascal
result := a + b * c;
quotient := a DIV b;    // Integer division
remainder := a MOD b;   // Modulo
average := sum / count; // Real division
```

### Conditional Statements

```pascal
if x > 0 then
    write "Positive";
else
    write "Non-positive";
endif
```

### Loops

#### For Loop
```pascal
for i from 1 to 10 do
    write i;
endfor
```

#### While Loop
```pascal
while x < 100 do
    x := x + 1;
endwhile
```

#### Repeat-Until Loop
```pascal
repeat
    x := x - 1;
until x = 0
```

### Input/Output

```pascal
read x;
write "The value is: ";
write x;
```

### Logical Operations

```pascal
if (x > 0) and (y > 0) then
    write "Both positive";
endif

if (x = 0) or (y = 0) then
    write "At least one is zero";
endif

if not flag then
    write "Flag is false";
endif
```

---

## 💡 Example Programs

### Example 1: Simple Calculator

**File**: `calculator.algo`

```pascal
algorithm Calculator
var
    num1, num2 : integer;
    operation : integer;
    answer : integer;
begin
    write "Simple Calculator";
    write "1. Addition";
    write "2. Subtraction";
    write "3. Multiplication";
    write "4. Division";
    write "Enter choice (1-4): ";
    read operation;
    
    write "Enter first number: ";
    read num1;
    write "Enter second number: ";
    read num2;
    
    if operation = 1 then
        answer := num1 + num2;
        write "Result: ";
        write answer;
    endif
    
    if operation = 2 then
        answer := num1 - num2;
        write "Result: ";
        write answer;
    endif
    
    if operation = 3 then
        answer := num1 * num2;
        write "Result: ";
        write answer;
    endif
    
    if operation = 4 then
        if num2 <> 0 then
            answer := num1 DIV num2;
            write "Result: ";
            write answer;
        else
            write "Error: Division by zero!";
        endif
    endif
end.
```

### Example 2: Factorial Calculator

```pascal
algorithm Factorial
var
    n, i, result : integer;
begin
    write "Enter a number: ";
    read n;
    
    result := 1;
    for i from 1 to n do
        result := result * i;
    endfor
    
    write "Factorial is: ";
    write result;
end.
```

### Example 3: Even/Odd Checker

```pascal
algorithm EvenOdd
var
    number : integer;
begin
    write "Enter a number: ";
    read number;
    
    if (number MOD 2) = 0 then
        write "The number is even";
    else
        write "The number is odd";
    endif
end.
```

---

## 🏗️ Project Architecture

```
MyAlgo/
├── AST.hpp              # Abstract Syntax Tree node definitions
├── CodeGenerator.hpp    # C++ code generation
├── SemanticAnalyzer.hpp # Type checking and semantic analysis
├── SymbolTable.hpp      # Symbol table management
├── token.hpp            # Token type definitions
├── lexer.cpp            # Lexical analyzer implementation
├── parser.cpp           # Parser implementation
├── main.cpp             # Compiler entry point
├── test.algo            # Example MyAlgo program
├── Grammar.text         # Language grammar specification
├── README.md            # Project documentation
└── LICENSE              # License file
```

### Compilation Pipeline

```
┌─────────────┐
│ Source Code │ (.algo file)
└──────┬──────┘
       │
       ▼
┌─────────────┐
│   Lexer     │ (Tokenization)
└──────┬──────┘
       │
       ▼
┌─────────────┐
│   Parser    │ (AST Construction)
└──────┬──────┘
       │
       ▼
┌─────────────┐
│  Semantic   │ (Type Checking)
│  Analyzer   │
└──────┬──────┘
       │
       ▼
┌─────────────┐
│    Code     │ (C++ Generation)
│  Generator  │
└──────┬──────┘
       │
       ▼
┌─────────────┐
│  C++ Code   │ (.cpp file)
└─────────────┘
```

---

## 🔧 Advanced Features

### Symbol Table
- Tracks variable declarations and types
- Validates variable usage
- Detects duplicate declarations
- Ensures type consistency

### Type System
- Static type checking
- Type inference for literals
- Automatic type conversion where safe
- Error reporting for type mismatches

### Error Handling
- Lexical error detection
- Syntax error reporting with line numbers
- Semantic error validation
- Helpful error messages

---

## 🛣️ Roadmap

- [x] **Lexer** - Complete tokenization
- [x] **Parser** - Full AST construction
- [x] **Semantic Analysis** - Type checking
- [x] **Code Generator** - C++ output
- [x] **Control Structures** - if/for/while/repeat
- [x] **Type System** - integer/real/boolean
- [ ] **Functions** - User-defined procedures
- [ ] **Arrays** - Array data structures
- [ ] **String Operations** - Advanced string handling
- [ ] **Optimization** - Code optimization passes
- [ ] **Standard Library** - Built-in functions
- [ ] **IDE Integration** - Syntax highlighting

---

## 📚 Learning Objectives

This project teaches:

1. **Compiler Design Principles**
   - Lexical analysis and tokenization
   - Context-free grammars and parsing
   - Abstract syntax trees
   - Symbol tables and scoping

2. **Language Implementation**
   - Type systems and type checking
   - Semantic analysis
   - Code generation techniques
   - Error handling and reporting

3. **Software Engineering**
   - Modular design patterns
   - C++ best practices
   - Testing and debugging
   - Documentation

---

## 🤝 Contributing

Contributions are welcome! Here's how you can help:

1. **Fork** the repository
2. **Create** a feature branch
   ```bash
   git checkout -b feature/amazing-feature
   ```
3. **Commit** your changes
   ```bash
   git commit -m 'Add amazing feature'
   ```
4. **Push** to the branch
   ```bash
   git push origin feature/amazing-feature
   ```
5. **Open** a Pull Request

### Contribution Ideas
- Add new language features
- Improve error messages
- Optimize code generation
- Write more example programs
- Enhance documentation
- Add unit tests

---

## 🐛 Known Issues

- String concatenation not yet implemented
- Function definitions not supported
- Array indexing not available
- Limited standard library

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## 👨‍💻 Author

Created as an educational compiler project to demonstrate:
- Complete compilation pipeline
- Language design principles
- Code generation techniques
- Software engineering best practices

---

## 🙏 Acknowledgments

- Inspired by Pascal and university pseudocode conventions
- Built with modern C++ practices
- Designed for educational purposes

---

## 📞 Support

**Questions or Issues?**
- Open an issue on the repository
- Check existing documentation
- Review example programs
- Examine the grammar specification

---

## 🔗 Resources

### For Compiler Theory
- "Compilers: Principles, Techniques, and Tools" (Dragon Book)
- "Modern Compiler Implementation" by Andrew Appel
- "Engineering a Compiler" by Cooper & Torczon

### For C++ Programming
- [C++ Reference](https://en.cppreference.com/)
- [ISO C++ Guidelines](https://isocpp.org/)

---

**Happy Compiling! 🚀**

*Transform pseudocode into reality with MyAlgo*
