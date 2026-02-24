# MyAlgo Roadmap — v0.1

Simple roadmap for solo development.

---

## Now (v1.0 — Current)

- [x] Lexer + Parser + Semantic Analyzer + Code Generator
- [x] Basic types: `integer`, `real`, `boolean`, `string`
- [x] Control flow: `if/else`, `for`, `while`, `repeat/until`
- [x] Functions and procedures
- [x] Arrays (1D) and matrices (2D)
- [x] Structures with field access
- [x] CMake build setup
- [x] Feature-focused test files
- [x] Grammar and language spec docs

Status: **Beta** — Core features work; diagnostics need improvement.

---

## Next (v1.1)

1. Better error messages with line/column info
2. Add regression test runner (run all `test*.algo` at once)
3. Add negative tests (invalid programs)
4. Code generator fixes for edge cases


---

## Later (v1.2+)

- Return path validation for functions
- Call argument type checking
- Arrays of structures
- Minimal stdlib (`length`, `abs`)
- CI/CD (GitHub Actions)
- Architecture refactoring (split headers to `.cpp`)
- Optional: interpreter backend

---

## How to Contribute (Solo)

Each week:
1. Pick one item from **Next**
2. Implement + test thoroughly
3. Move to **Done** and pick next item
4. Update this file when done
