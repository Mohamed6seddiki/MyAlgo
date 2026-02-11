# MyAlgo Compiler

![Version](https://img.shields.io/badge/version-1.0-blue)
![Language](https://img.shields.io/badge/language-C++-orange)
![Status](https://img.shields.io/badge/status-Experimental-red)

**MyAlgo** is an educational programming language interpreter inspired by **university pseudo-code**, designed to help students learn how programming languages work and experiment with building their own language using C++.

---

## ⚡ Features

- Fully written in **C++**
- Syntax similar to **university pseudo-code**
- Currently supports:
  - Variables (`IDENTIFIER`)
  - Arithmetic operations (`+`, `-`, `*`, `/`)
  - Assignment (`<-`)
  - Print statement (`ECRIRE`)
- Interpreter **analyzes code and executes it directly** (no compilation needed)

---

## 📝 Example MyAlgo Program

**File:** `test.algo`

```algo
x <- 55
y <- x + 5
ECRIRE y
```

**Output (Lexer stage):**

```
IDENTIFIER : x
ASSIGN : <-
NUMBER : 55
IDENTIFIER : y
ASSIGN : <-
IDENTIFIER : x
PLUS
NUMBER : 5
ECRIRE
IDENTIFIER : y
EOF
```

> **Note:** The current version only includes the Lexer.

---

## 📋 Requirements

- **Operating System**: Windows, Linux, or macOS
- **Compiler**: g++ (C++11 or later)
- **Terminal**: Command Prompt, PowerShell, or Unix shell

---

## 🚀 How to Run

### Windows (PowerShell/CMD)

1. Open **Terminal** or **PowerShell** in the project folder
2. Compile the code:
   ```bash
   g++ main.cpp lexer.cpp -o myalgo
   ```
3. Run the interpreter with a `.algo` file:
   ```bash
   .\myalgo test.algo
   ```

### Linux / MacOS

1. Open **Terminal** in the project folder
2. Compile the code:
   ```bash
   g++ main.cpp lexer.cpp -o myalgo
   ```
3. Run the interpreter with a `.algo` file:
   ```bash
   ./myalgo test.algo
   ```

This will:
1. Compile `test.algo` to C++ code (`test.cpp`)
2. Automatically compile the C++ code to an executable (`test.exe` or `test`)
3. Execute the resulting program

---

## 🗺️ Project Structure

```
MyAlgo/
│── token.hpp       # Token definitions
│── lexer.cpp       # Lexical analyzer (tokenization)
│── main.cpp        # Entry point (reads file and runs lexer)
│── test.algo       # Example MyAlgo program
│── README.md       # Project documentation
```

---

## 📚 Syntax Guide

### Variables and Assignment
```algo
x <- 42
name <- "MyAlgo"
```

### Arithmetic Operations
```algo
result <- 10 + 5 * 2
difference <- x - y
```

### Output
```algo
ECRIRE x
ECRIRE result
```

---

## 🔮 Roadmap

- [x] **Lexer** - Tokenize source code
- [ ] **Parser** - Build Abstract Syntax Tree (AST)
- [ ] **Interpreter** - Execute instructions from AST
- [ ] **Conditional Statements** - Add `SI`, `ALORS`, `SINON`
- [ ] **Loops** - Add `POUR`, `TANTQUE`
- [ ] **Functions** - Add procedure definitions
- [ ] **Error Handling** - Improve error messages and debugging
- [ ] **Standard Library** - Add built-in functions

---

## 💡 Learning Goals

This project is designed to teach:

- How **lexical analysis** (tokenization) works
- How to build a **parser** and create an Abstract Syntax Tree
- How **interpreters** execute code
- The fundamentals of **compiler/interpreter design**
- Practical **C++ programming** skills

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

This project is open-source and available for educational purposes. Feel free to use, modify, and learn from it.

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
- Check the code comments for implementation details
- Experiment with the code to learn more!

---

**Happy Compiling! 🚀**

*Transform pseudocode into reality with MyAlgo*
