# MyAlgo Compiler

![Version](https://img.shields.io/badge/version-1.0-blue)
![Language](https://img.shields.io/badge/language-C++-orange)
![Status](https://img.shields.io/badge/status-Active-green)

**MyAlgo** is a complete programming language compiler written in C++. It compiles MyAlgo pseudocode (inspired by university algorithms courses) into executable C++ code. This project demonstrates how to build a full compiler pipeline: lexical analysis → parsing → semantic analysis → code generation.

---

## ⚡ Features

- **Full Compiler Pipeline:**
  - ✅ Lexical Analysis (Tokenization)
  - ✅ Syntactic Analysis (Parsing)
  - ✅ Semantic Analysis (Type checking)
  - ✅ Code Generation (C++ output)

- **Language Features:**
  - ✅ Variables with 4 data types: `integer`, `real`, `boolean`, `string`
  - ✅ Arrays: `array[size] of type`
  - ✅ Functions with parameters and return types
  - ✅ Procedures with parameters
  - ✅ All control flow: `if/else`, `for`, `while`, `repeat-until`
  - ✅ Arithmetic & logical operations
  - ✅ String operations (concatenation, comparison)
  - ✅ Input/Output: `read()`, `write()`

- **Code Generation:**
  - Compiles to C++ code
  - Automatic compilation to executable
  - Direct program execution

---

## 📝 Example Programs

### Example 1: Simple Array Program
**File:** `test_arrays.algo`

```algo
algorithm TestArrays
var
    numbers: array[5] of integer;
    i: integer;
    sum: integer;

begin
    numbers[0] := 10;
    numbers[1] := 20;
    numbers[2] := 30;
    numbers[3] := 40;
    numbers[4] := 50;
    
    sum := 0;
    for i from 0 to 4 do
        sum := sum + numbers[i];
    endfor
    
    write("Sum of array: ");
    write(sum);
end.
```

**Output:**
```
Sum of array: 
150
```

### Example 2: Functions and String Support
**File:** `test.algo`

```algo
algorithm StringDemo
var
    message: string;
    name: string;

begin
    message := "Hello";
    name := "World";
    
    write(message + " " + name);
    
    if message = "Hello" then
        write("Greeting matched!");
    endif
end.
```

### Example 3: Calculator with Functions
**File:** `test.algo`

```algo
algorithm Calculator
var
    a, b, result: real;

function add(x: real, y: real): real
beginfunction
    return x + y;
endfunction

procedure printResult()
beginprocedure
    write("Result: ");
    write(result);
endprocedure

begin
    a := 10.5;
    b := 20.3;
    result := add(a, b);
    printResult();
end.
```

---

## 🔧 Data Types

| Type | Example | Notes |
|------|---------|-------|
| `integer` | `x: integer` | 32-bit integer |
| `real` | `pi: real` | Floating-point number |
| `boolean` | `flag: boolean` | true or false |
| `string` | `name: string` | Text strings |
| `array` | `arr: array[10] of integer` | Fixed-size arrays |

---

## 📋 Language Syntax

### Variable Declaration
```algo
var
    x, y: integer;
    name: string;
    numbers: array[5] of integer;
```

### Control Structures
```algo
-- If statement
if condition then
    write("True");
else
    write("False");
endif

-- For loop
for i from 1 to 10 do
    write(i);
endfor

-- While loop
while condition do
    write("Looping");
endwhile

-- Repeat-until loop
repeat
    write("Do something");
until condition
```

### Functions and Procedures
```algo
-- Function (returns a value)
function add(a: integer, b: integer): integer
beginfunction
    return a + b;
endfunction

-- Procedure (no return value)
procedure printMessage(msg: string)
beginprocedure
    write(msg);
endprocedure
```

### Input/Output
```algo
-- Read input
read(x);

-- Write output
write("Hello");
write(42);
write(x + y);
```

---

## 🛠️ Requirements

- **Operating System:** Windows / Linux / MacOS
- **Compiler:** g++ (C++ compiler)
- **Terminal:** Command Prompt, PowerShell, or any Unix shell

---

## 🚀 How to Run

### Windows (PowerShell/CMD)

1. Open **Terminal** or **PowerShell** in the project folder
2. Compile the compiler:
   ```bash
   g++ -std=c++17 main.cpp lexer.cpp parser.cpp -o myalgo
   ```
3. Run the compiler with a `.algo` file:
   ```bash
   .\myalgo test.algo
   ```
4. The compiler will:
   - Generate C++ code (e.g., `test.cpp`)
   - Automatically compile it with `g++ -std=c++11`
   - Execute the resulting program

### Linux / MacOS

1. Open **Terminal** in the project folder
2. Compile the compiler:
   ```bash
   g++ -std=c++17 main.cpp lexer.cpp parser.cpp -o myalgo
   ```
3. Run the compiler with a `.algo` file:
   ```bash
   ./myalgo test.algo
   ```

---

## Known Limitations

- Reserved keywords cannot be used as variable names or function parameters
- Arrays are fixed-size and must be declared with a constant size
- Single-dimensional arrays only (multi-dimensional arrays not yet supported)
- No built-in string functions (length, substring, etc.)
- No struct/record types
- Comments not yet implemented

---

## 🐛 Troubleshooting

### Error: "undefined reference to `std::string'"
- Ensure you're compiling with C++11 or later: `g++ -std=c++11`

### Error: "Expected ':' after identifier in parameter list"
- Check that you're using a valid identifier name (not a reserved keyword)
- **Reserved keywords:** algorithm, var, begin, end, function, procedure, return, if, then, else, endif, for, from, to, do, endfor, while, endwhile, repeat, until, read, write, endprocedure, beginprocedure, beginfunction, endfunction, integer, real, boolean, string, array, of

### Error: "Unexpected token"
- Check syntax carefully - ensure all control structures are properly closed
- Verify function/procedure definitions end with proper keywords

---

## 📚 Architecture

The MyAlgo Compiler follows a traditional multi-stage compilation pipeline:

1. **Lexical Analysis (Lexer)**
   - Reads source code character by character
   - Produces token stream
   - Recognizes keywords, identifiers, operators, literals

2. **Parsing**
   - Builds Abstract Syntax Tree (AST) from tokens
   - Validates grammar
   - Checks syntax structure

3. **Semantic Analysis**
   - Type checking
   - Symbol table management
   - Semantic validation

4. **Code Generation**
   - Traverses AST
   - Generates equivalent C++ code
   - Handles type conversions

5. **C++ Compilation**
   - Uses system g++ compiler
   - Produces executable binary

6. **Execution**
   - Runs generated executable
   - Outputs results

---

## 📖 Implementation Files

| File | Purpose |
|------|---------|
| `token.hpp` | Token type definitions |
| `lexer.cpp`, `lexer.hpp` | Lexical analyzer (tokenizer) |
| `parser.cpp`, `parser.hpp` | Syntax analyzer (parser) |
| `AST.hpp` | Abstract Syntax Tree node definitions |
| `SemanticAnalyzer.hpp` | Type checking and semantic validation |
| `SymbolTable.hpp` | Symbol table and scope management |
| `CodeGenerator.hpp` | C++ code generation |
| `main.cpp` | Compiler entry point and orchestration |

---

## 🔄 Supported Operations

### Arithmetic Operations
- Addition: `a + b`
- Subtraction: `a - b`
- Multiplication: `a * b`
- Division: `a / b`

### Comparison Operations
- Equal: `a = b`
- Not equal: `a <> b`
- Less than: `a < b`
- Greater than: `a > b`
- Less/equal: `a <= b`
- Greater/equal: `a >= b`

### Logical Operations
- AND: `a and b`
- OR: `a or b`
- NOT: `not a`

### String Operations
- Concatenation: `"Hello" + " " + "World"`
- Comparison: `name = "John"`

### Array Operations
- Declaration: `arr: array[10] of integer`
- Indexing: `arr[0]`, `arr[i]`
- Assignment: `arr[i] := value`

---

## 🎯 Compilation Example

**Source:** `hello.algo`
```algo
algorithm HelloWorld
var
    message: string;
begin
    message := "Hello, World!";
    write(message);
end.
```

**Generated C++:** `hello.cpp`
```cpp
#include <iostream>
#include <string>
using namespace std;

int main() {
    string message;
    message = "Hello, World!";
    cout << message << endl;
    return 0;
}
```

**Compilation & Execution:**
```bash
$ ./myalgo hello.algo
Hello, World!
```

---

## 🔮 Roadmap / Future Enhancements

- [ ] Multi-dimensional arrays
- [ ] Struct/record types
- [ ] Built-in string functions
- [ ] File I/O operations
- [ ] Standard library functions
- [ ] Better error messages with line numbers
- [ ] Optimization passes
- [ ] Debugging support
- [ ] Better syntax error recovery

---

## 💡 Learning Goals

This project is designed to teach:

- How **lexical analysis** (tokenization) works
- How to build a **recursive descent parser** and create an Abstract Syntax Tree
- How **semantic analysis** performs type checking
- How **code generation** transpiles to target languages
- The fundamentals of **compiler design**
- Practical **C++ programming** skills
- Building complete development tools

---

## 🤝 Contributing

This is an educational project, and contributions are welcome! If you'd like to add features or fix bugs:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/new-feature`)
3. Commit your changes (`git commit -m 'Add new feature'`)
4. Push to the branch (`git push origin feature/new-feature`)
5. Open a Pull Request

---

## 📄 License

This project is licensed under the MIT License - see the LICENSE file for details.

---

## 👨‍💻 Author

Created as an educational project to understand programming language design and implementation.

---

## 📞 Support

If you encounter any issues or have questions:

- Open an issue on the repository
- Check the code comments for implementation details
- Review example `.algo` files in the repository
- Experiment with the code to learn more!

---

**Happy coding! 🚀**
