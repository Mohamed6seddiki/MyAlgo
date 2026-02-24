# MyAlgo

![Version](https://img.shields.io/badge/version-1.0-blue)
![Language](https://img.shields.io/badge/language-C%2B%2B17-orange)
![Build](https://img.shields.io/badge/build-CMake-success)
![Status](https://img.shields.io/badge/status-beta-yellow)

MyAlgo is a Pascal-like educational language and compiler project written in C++.

It includes a full front-end and execution pipeline:

- Lexical analysis
- Parsing to AST
- Semantic checks
- C++ code generation
- Native execution of generated program

## Why this project

MyAlgo is designed for learning compiler construction with a real codebase and practical language features (functions, procedures, arrays, matrices, structures, control flow).

## Features

- Program model: `algorithm ... begin ... end.`
- Types: `integer`, `real`, `boolean`, `string`
- User-defined `structure` declarations
- 1D arrays and 2D matrices (`array[n]` / `array[r][c] of type`)
- Functions and procedures with parameters
- Pass-by-reference parameters via `var`
- Statements:
  - assignment (`<=`)
  - `if / else / endif`
  - `for / endfor`
  - `while / endwhile`
  - `repeat / until`
  - `read(...)`, `write(...)`, `return`
- Operators:
  - arithmetic: `+ - * / div mod`
  - comparison: `= <> < <= > >=`
  - logic: `and or not`
- Comments:
  - `// single-line`
  - `# single-line`

## Compilation pipeline

When you run `myalgo <file.algo>`, the tool:

1. Tokenizes source code
2. Parses source into AST
3. Performs semantic validation
4. Generates `<file>.cpp`
5. Compiles generated C++ using `g++`
6. Executes the resulting binary

> Important: `myalgo` is built with CMake/MSVC or any C++ compiler, but generated code is currently compiled by invoking `g++` at runtime. Ensure `g++` is available in your `PATH`.

## Requirements

- CMake `>= 3.16`
- C++17 compiler (MSVC, GCC, or Clang) to build `myalgo`
- `g++` available in `PATH` for generated `.cpp` compilation step
- Windows, Linux, or macOS

## Build and run

### Option A: Install from Releases and run in terminal

If you installed MyAlgo from the GitHub Releases installer, you can run programs directly from terminal:

- [Download MyAlgo v1.0](https://github.com/Mohamed6seddiki/MyAlgo/releases/tag/v1.0)

1. Open PowerShell or CMD in the folder that contains your `.algo` file.
2. Run:

```powershell
myalgo test.algo
```

If your file has another name, replace `test.algo` with your file name.

### Option B: CMake (recommended)

```bash
cmake -S . -B build
cmake --build build
```

Run a sample program:

```bash
./build/Release/myalgo.exe examples/hello.algo
```

If your generator/config differs, the binary path may be `build/myalgo` or `build/Debug/myalgo.exe`.

### Option C: CMake Presets

This repository provides presets in `CMakePresets.json`:

```bash
cmake --preset default
cmake --build --preset default
```

## Quick language example

```algo
algorithm Demo

function add(a: integer, b: integer): integer
beginfunction
	return a + b;
endfunction

var
	x: integer;

begin
	x := add(4, 5);
	write(x);
end.
```

## Documentation

- Grammar: [docs/Grammar.md](docs/Grammar.md)
- Language spec: [docs/languge_spec.md](docs/languge_spec.md)
- Roadmap: [docs/roadmap.md](docs/roadmap.md)

## Repository structure

```text
MyAlgo/
├─ CMakeLists.txt
├─ CMakePresets.json
├─ src/
│  ├─ main.cpp
│  ├─ core/
│  ├─ lexer/
│  ├─ parser/
│  ├─ semantic/
│  ├─ codegen/
│  └─ AST/
├─ examples/         # Sample .algo programs
├─ testes/           # Feature tests and generated outputs
├─ docs/             # Grammar/spec/roadmap
└─ installer/        # Inno Setup script and build helper
└─ build/     
```

## Examples

Sample programs are available in `examples/`:

- `hello.algo`
- `arithmetic.algo`
- `if_else.algo`
- `loop_for.algo`
- `loop_while.algo`
- `function_example.algo`
- `array_sum.algo`
- `matrix_example.algo`
- `structure_example.algo`
- `factorial.algo`

Run any example:

```bash
./build/Release/myalgo.exe examples/factorial.algo
```

## Testing

The `testes/` folder contains `.algo` scenario files and generated `.cpp/.exe` outputs used during development.

Run an individual test file:

```bash
./build/Release/myalgo.exe testes/test_all_features.algo
```

## Known limitations (current)

- Diagnostics do not yet include full line/column-rich reporting in all paths
- Runtime execution depends on external `g++`
- No CI workflow committed yet

## Windows installer

An installer setup is included using Inno Setup.

Prerequisite: install Inno Setup 6 (`ISCC.exe`).

Build installer:

```powershell
cd installer
.\build-installer.ps1
```

Output is generated under `installer/Output/`.

## Contributing

Contributions are welcome.

1. Fork the repository
2. Create a branch (`feature/your-feature`)
3. Add or update tests/examples where relevant
4. Open a pull request with a clear description

Suggested contribution areas:

- Better semantic diagnostics
- Additional negative tests (invalid programs)
- Code generator robustness improvements
- CI automation
- Documentation improvements

## Author 👨‍💻

**Mohamed Seddiki**
- 🐙 GitHub: [@Mohamed6seddiki](https://github.com/Mohamed6seddiki)

## License

This project is licensed under the terms in [LICENSE](LICENSE).
