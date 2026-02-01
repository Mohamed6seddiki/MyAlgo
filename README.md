# MyAlgo

![Version](https://img.shields.io/badge/version-0.1-blue)
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

## 🛠️ Requirements

- **Operating System:** Windows / Linux / MacOS
- **Compiler:** g++ (C++ compiler)
- **Terminal:** Command Prompt, PowerShell, or any Unix shell

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

This is an educational project, and contributions are welcome! If you'd like to add features or fix bugs:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/new-feature`)
3. Commit your changes (`git commit -m 'Add new feature'`)
4. Push to the branch (`git push origin feature/new-feature`)
5. Open a Pull Request

---

## 📄 License

This project is open-source and available for educational purposes. Feel free to use, modify, and learn from it.

---

## 👨‍💻 Author

Created as an educational project to understand programming language design and implementation.

---

## 📞 Support

If you encounter any issues or have questions:

- Open an issue on the repository
- Check the code comments for implementation details
- Experiment with the code to learn more!

---

**Happy coding! 🚀**
