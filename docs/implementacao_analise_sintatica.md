# Implementação da Análise Sintática

**Arquivo:** `src/parser/parser.y`  
**Ferramenta:** Bison (GNU Parser Generator)  
**Saídas geradas:** `parser.cpp` e `parser.hpp` (gerados automaticamente pelo CMake via `BISON_TARGET`)

---

## 1. Visão Geral

O analisador sintático é implementado com **Bison**, que lê o arquivo de gramática `parser.y` e gera um parser LALR(1). Durante o parsing, cada redução de regra gramatical executa uma ação em C++ que constrói um nó da AST. Ao final de um parse bem-sucedido, a variável global `root_ast` aponta para a raiz da árvore (`ProgramNode*`).

---

## 2. Estrutura do Arquivo `parser.y`

O arquivo `.y` é dividido em quatro seções:

```
[Seção de Declarações C++]
%%
[Regras Gramaticais com Ações]
%%
[Código C++ auxiliar]
```

### 2.1. Seção de Declarações

#### `%code requires`
Inclui os cabeçalhos necessários para que os tipos da `%union` sejam reconhecidos tanto no `parser.cpp` quanto no `parser.hpp` gerado:

```cpp
%code requires {
    #include "ast.hpp"
    #include <vector>
    #include <string>
}
```

#### `%union` — Tipos Semânticos

Define todos os tipos que um símbolo (terminal ou não-terminal) pode carregar:

```cpp
%union {
    int    intval;
    float  floatval;
    double doubleval;
    char   charval;
    char  *strval;

    ASTNode      *ast_node;
    BlockNode    *block_node;
    FuncDeclNode *func_decl_node;
    ProgramNode  *prog_node;
    VarDeclNode  *var_decl_node;

    std::vector<ASTNode*>    *node_list;
    std::vector<VarDeclNode*>*param_list;
}
```

#### Declaração de Tokens

Tokens sem valor semântico:
```bison
%token INT FLOAT DOUBLE CHAR BOOL VOID
%token IF ELSE FOR WHILE
%token BREAK CONTINUE RETURN MAIN
%token AND OR NOT
%token PLUS MINUS MULT DIV MOD
%token ASSIGN PLUSEQ MINUSEQ MULTEQ DIVEQ MODEQ
%token EQ NEQ LT GT LE GE INC DEC NOT_OP
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON COMMA
%token UNKNOWN_TOKEN
```

Tokens com valor semântico:
```bison
%token <strval>    IDENTIFIER
%token <intval>    INTEGER_LITERAL BOOL_LITERAL
%token <floatval>  FLOAT_LITERAL
%token <doubleval> DOUBLE_LITERAL
%token <charval>   CHAR_LITERAL
```

#### Tipos dos Não-Terminais

Cada não-terminal declara qual campo da `%union` carrega seu valor semântico:

```bison
%type <prog_node>      program
%type <func_decl_node> main_function function_declaration
%type <block_node>     block
%type <ast_node>       statement expression assignment_expression ...
%type <node_list>      statement_list global_declaration_list ...
%type <param_list>     optional_parameters parameter_list
%type <intval>         type_specifier assignment_operator
```

---

## 3. Gramática e Construção da AST

### 3.1. Símbolo Inicial: `program`

```bison
program
    : main_function
    | global_declaration_list main_function
    ;
```

Todo programa C-- Strict deve ter exatamente uma `main_function`. Declarações globais (variáveis e funções) podem preceder a `main`. A ação cria um `ProgramNode` e atribui a `root_ast`.

### 3.2. Função Principal: `main_function`

```bison
main_function
    : INT MAIN LPAREN RPAREN block
    ;
```

A `main` é tratada como regra separada para tornar obrigatória sua presença. Ela não aceita parâmetros.

### 3.3. Declaração de Função: `function_declaration`

Suporta tanto definição (com corpo) quanto protótipo (sem corpo):

```bison
function_declaration
    : type_specifier IDENTIFIER LPAREN optional_parameters RPAREN block       /* definição */
    | type_specifier IDENTIFIER LPAREN optional_parameters RPAREN SEMICOLON   /* protótipo */
    ;
```

Cria um `FuncDeclNode`. Quando for protótipo, o campo `body` é `nullptr`.

### 3.4. Declaração de Variável: `variable_declaration`

Suporta múltiplos declaradores em uma só linha (ex: `int x, y = 0, z;`):

```bison
variable_declaration
    : type_specifier init_declarator_list
    ;
```

A lista de declaradores é construída como `std::vector<ASTNode*>*`. O tipo base (`type_specifier`) é aplicado a todos os declaradores após a lista ser construída, usando `dynamic_cast<VarDeclNode*>`.

### 3.5. Blocos: `block`

```bison
block
    : LBRACE statement_list RBRACE
    ;
```

`statement_list` aceita intercalação de declarações locais e comandos, tornando `int x = 0; x++;` válido dentro de um bloco.

### 3.6. Hierarquia de Expressões e Precedência

A precedência é implementada pela hierarquia de não-terminais (do menor para o maior):

| Nível (menor → maior) | Não-terminal             | Operadores              |
|------------------------|--------------------------|-------------------------|
| 1 (menor precedência)  | `assignment_expression`  | `=`, `+=`, `-=`, ...    |
| 2                      | `logical_or_expression`  | `or`                    |
| 3                      | `logical_and_expression` | `and`                   |
| 4                      | `relational_expression`  | `==`, `!=`, `<`, `>`, `<=`, `>=` |
| 5                      | `additive_expression`    | `+`, `-`                |
| 6                      | `multiplicative_expression` | `*`, `/`, `%`        |
| 7 (maior precedência)  | `unary_expression`       | `!`, `not`, `-`, `++`, `--` |

Todos os níveis usam **recursão à esquerda**, o que é compatível com parsers LALR.

### 3.7. Operadores Pós-fixos

```bison
unary_expression
    : IDENTIFIER INC  { $$ = new UnOpNode(POST_INC, new IdNode($1)); }
    | IDENTIFIER DEC  { $$ = new UnOpNode(POST_DEC, new IdNode($1)); }
    ;
```

Os tokens especiais `POST_INC` (1000) e `POST_DEC` (1001) são definidos em `include/ast.hpp` para distinguir pré-fixo de pós-fixo na AST.

### 3.8. Chamadas de Função

A ambiguidade entre `IDENTIFIER` como variável e como chamada de função é resolvida com o não-terminal `optional_call_suffix`:

```bison
primary_expression
    : IDENTIFIER optional_call_suffix
    ;

optional_call_suffix
    : LPAREN optional_argument_list RPAREN   /* é chamada */
    | /* empty */                            /* é variável */
    ;
```

Se houver sufixo, cria um `FuncCallNode`; caso contrário, um `IdNode`.

### 3.9. Dangling Else

O problema do *dangling else* é resolvido pela distinção entre `matched_statement` e `unmatched_statement`:

- `matched_statement`: todos os `if`s internos têm `else`.
- `unmatched_statement`: pelo menos um `if` interno não tem `else`.

A regra do `if` com `else` sempre casa com o `if` mais próximo, seguindo a semântica padrão de C.

---

## 4. Tratamento de Erros

A função `yyerror` imprime a localização do erro com o número da linha (variável global `line` fornecida pelo lexer):

```cpp
void yyerror(const char *s) {
    fprintf(stderr, "Erro sintático na linha %d: %s\n", line, s);
}
```

---

## 5. Integração com o CMake

O CMake localiza o Bison e gera automaticamente `parser.cpp` e `parser.hpp`:

```cmake
find_package(BISON REQUIRED)
BISON_TARGET(Parser parser.y
    ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.hpp)
```

O arquivo `parser.hpp` gerado é incluído via `tokens.hpp` pelo lexer, garantindo que os códigos numéricos dos tokens sejam idênticos entre lexer e parser.

---

## 6. Runners dos Modos `--parser` e `--ast`

### Modo `--parser` (`src/runners/parser_runner.cpp`)

Chama `yyparse()` e reporta sucesso ou falha:

```cpp
int run_parser_mode() {
    int result = yyparse();
    if (result == 0)
        std::cout << "\nAnálise sintática concluída com sucesso.\n";
    else
        std::cerr << "\nFalha na analise sintática.\n";
    return result;
}
```

### Modo `--ast` (`src/runners/ast_runner.cpp`)

Além de parsear, imprime a AST completa usando `imprimirAST(root_ast)`:

```cpp
int run_ast_mode() {
    int result = yyparse();
    if (result == 0 && root_ast != NULL) {
        imprimirAST(root_ast);
    }
    return result;
}
```

A impressão é feita pelo `ASTPrinter` em `src/ast/ASTPrinter.cpp`, que implementa o padrão Visitor para percorrer a árvore com indentação.
