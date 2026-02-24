# MyAlgo Language Specification

## 1. Overview

MyAlgo is a Pascal-like educational language with a full front-end and code generation pipeline:

- Lexical analysis
- Parsing to AST
- Semantic analysis (type/declaration checks)
- C++ code generation and execution

The language is case-sensitive for identifiers and keywords as currently implemented in the lexer.

---

## 2. Program Layout

A valid program follows this order:

1. `algorithm` header
2. optional `structure` declarations
3. optional `var` section (global variables)
4. optional `function`/`procedure` declarations
5. main block `begin ... end.`

```algo
algorithm ProgramName

structure Person
	name: string;
	age: integer;
endstructure

var
	x: integer;

function inc(a: integer): integer
beginfunction
	return a + 1;
endfunction

begin
	x := inc(5);
	write(x);
end.
```

---

## 3. Lexical Elements

### 3.1 Identifiers

- Start with a letter or `_`
- Continue with letters, digits, `_`

Examples:

- `x`
- `total_sum`
- `_tmp1`

### 3.2 Literals

- Integer: `42`
- Real: `3.14`
- String: `"hello"`
- Boolean: `true`, `false`

### 3.3 Comments

Single-line comments:

- `// comment`
- `# comment`

---

## 4. Types

Built-in types:

- `integer`
- `real`
- `boolean`
- `string`

User-defined types:

- `structure` names (e.g., `Person`)

Collection types:

- 1D array: `array[n] of <type>`
- 2D matrix: `array[r][c] of <type>`

Examples:

```algo
var
	a: integer;
	scores: array[10] of real;
	mat: array[3][4] of integer;
```

---

## 5. Structure Declarations

Syntax:

```algo
structure Name
	field1: type;
	field2, field3: type;
endstructure
```

Usage:

```algo
var
	p: Person;

begin
	p.name := "Alice";
	p.age := 22;
	write(p.name);
end.
```

Notes:

- Field access uses dot notation: `obj.field`
- Structure fields currently use built-in field types

---

## 6. Variable Declarations

```algo
var
	x, y: integer;
	s: string;
	flags: array[5] of boolean;
	m: array[2][3] of integer;
```

Each declaration ends with `;`.

---

## 7. Functions and Procedures

### 7.1 Function

Returns a value using `return`.

```algo
function add(a: integer, b: integer): integer
beginfunction
	return a + b;
endfunction
```

### 7.2 Procedure

No return value.

```algo
procedure printValue(x: integer)
beginprocedure
	write(x);
endprocedure
```

### 7.3 `var` Parameters

Use `var` for pass-by-reference behavior.

```algo
procedure swap(var a: integer, var b: integer)
var
	t: integer;
beginprocedure
	t := a;
	a := b;
	b := t;
endprocedure
```

---

## 8. Statements

### 8.1 Assignment

```algo
x := 10;
arr[2] := 7;
mat[i][j] := 5;
p.age := 30;
```

### 8.2 Input / Output

```algo
read(x);
write("value:");
write(x);
```

### 8.3 If / Else

```algo
if x > 0 then
	write("positive");
else
	write("non-positive");
endif
```

### 8.4 For

```algo
for i from 0 to 4 do
	write(i);
endfor
```

### 8.5 While

```algo
while x < 10 do
	x := x + 1;
endwhile
```

### 8.6 Repeat / Until

```algo
repeat
	x := x - 1;
until x = 0
```

### 8.7 Return

Allowed in functions (and currently accepted by parser generally):

```algo
return expr;
```

---

## 9. Expressions

Supported operators:

- Arithmetic: `+`, `-`, `*`, `/`, `DIV`, `MOD`
- Comparison: `=`, `<>`, `<`, `>`, `<=`, `>=`
- Logical: `and`, `or`, `not`

Precedence (high → low):

1. Unary `-`, `not`
2. `*`, `/`, `DIV`, `MOD`
3. `+`, `-`
4. comparisons
5. `and`
6. `or`

Examples:

```algo
z := (a + b) * 2;
ok := (x >= 10) and not (y = 0);
```

---

## 10. Semantic Rules (Current)

The semantic analyzer checks:

- Variable declaration before use
- Type compatibility in assignments and expressions
- Boolean conditions in `if`, `while`, `until`
- Integer indices for arrays/matrices
- Duplicate declarations in a scope
- Function name resolution for calls

---

## 11. Current Constraints

- Program order is strict (see Section 2)
- One source file contains one `algorithm`
- Arrays/matrices use fixed compile-time sizes
- `read` accepts variable/field targets
- Generated C++ is compiled with `g++` in the current runtime flow

---

## 12. Minimal Valid Example

```algo
algorithm Hello
var
	x: integer;
begin
	x := 5;
	write("Hello");
	write(x);
end.
```

