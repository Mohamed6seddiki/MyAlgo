🔹 Program
Program ::= "algorithm" Identifier
            VarSection
            Block ;

# MyAlgo Grammar (Current)

This grammar reflects the current parser behavior in the project.

## 1) Program Structure

```ebnf
Program          ::= "algorithm" Identifier
                                         { StructureDecl }
                                         [ VarSection ]
                                         { FunctionDecl | ProcedureDecl }
                                         MainBlock "." ;

MainBlock        ::= "begin" StatementList "end" ;
StatementList    ::= { Statement [ ";" ] } ;
```

## 2) Structures

```ebnf
StructureDecl    ::= "structure" Identifier
                                         { StructFieldDecl }
                                         "endstructure" ;

StructFieldDecl  ::= IdentifierList ":" TypeName ";" ;
```

## 3) Variables and Types

```ebnf
VarSection       ::= "var" { VarDecl } ;

VarDecl          ::= IdentifierList ":" TypeSpec ";" ;

TypeSpec         ::= TypeName
                                     | "array" "[" Number "]" [ "[" Number "]" ] "of" TypeName ;

TypeName         ::= "integer"
                                     | "real"
                                     | "boolean"
                                     | "string"
                                     | Identifier ;   (* user-defined structure type *)

IdentifierList   ::= Identifier { "," Identifier } ;
```

## 4) Functions and Procedures

```ebnf
FunctionDecl     ::= "function" Identifier
                                         [ "(" ParamList ")" ]
                                         ":" TypeName
                                         [ VarSection ]
                                         "beginfunction" StatementList "endfunction" ;

ProcedureDecl    ::= "procedure" Identifier
                                         [ "(" ParamList ")" ]
                                         [ VarSection ]
                                         "beginprocedure" StatementList "endprocedure" ;

ParamList        ::= ParamDecl { "," ParamDecl } ;
ParamDecl        ::= [ "var" ] IdentifierList ":" TypeName ;
```

## 5) Statements

```ebnf
Statement        ::= Assignment
                                     | IfStmt
                                     | ForStmt
                                     | WhileStmt
                                     | RepeatStmt
                                     | ReadStmt
                                     | WriteStmt
                                     | ReturnStmt
                                     | CallStmt ;

Assignment       ::= LValue ":=" Expression ;

LValue           ::= IdentifierOrField
                                         [ "[" Expression "]" [ "[" Expression "]" ] ] ;

IdentifierOrField::= Identifier { "." Identifier } ;

CallStmt         ::= IdentifierOrField "(" [ ArgList ] ")" ;
ArgList          ::= Expression { "," Expression } ;

ReadStmt         ::= "read" "(" IdentifierOrField ")" ;
WriteStmt        ::= "write" "(" Expression ")" ;

ReturnStmt       ::= "return" [ Expression ] ;

IfStmt           ::= "if" Expression "then"
                                         StatementList
                                         [ "else" StatementList ]
                                         "endif" ;

ForStmt          ::= "for" Identifier "from" Expression "to" Expression "do"
                                         StatementList
                                         "endfor" ;

WhileStmt        ::= "while" Expression "do"
                                         StatementList
                                         "endwhile" ;

RepeatStmt       ::= "repeat"
                                         StatementList
                                         "until" Expression ;
```

## 6) Expressions

```ebnf
Expression       ::= LogicalOr ;

LogicalOr        ::= LogicalAnd { "or" LogicalAnd } ;
LogicalAnd       ::= Equality  { "and" Equality  } ;

Equality         ::= Comparison { ("=" | "<>") Comparison } ;
Comparison       ::= Term       { ("<" | "<=" | ">" | ">=") Term } ;
Term             ::= Factor     { ("+" | "-") Factor } ;
Factor           ::= Unary      { ("*" | "/" | "DIV" | "MOD") Unary } ;

Unary            ::= ("-" | "not") Unary
                                     | Primary ;

Primary          ::= Number
                                     | String
                                     | "true"
                                     | "false"
                                     | IdentifierOrField
                                         [ "(" [ ArgList ] ")"
                                         | "[" Expression "]" [ "[" Expression "]" ] ]
                                     | "(" Expression ")" ;
```

## 7) Lexical Notes

```ebnf
Identifier       ::= letter { letter | digit | "_" } ;
Number           ::= digit { digit } [ "." digit { digit } ] ;
String           ::= '"' { any-char-except-quote } '"' ;
```

Comments supported by lexer:
- `// ...` single-line
- `# ...` single-line