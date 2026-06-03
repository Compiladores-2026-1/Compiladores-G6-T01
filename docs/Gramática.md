# Análise Sintática — Gramática em BNF

## 1. Visão Geral

A gramática do compilador **C-- Strict** é uma **Gramática Livre de Contexto (CFG)** processada pelo gerador de parsers **Bison (GNU Parser Generator)** com a técnica **LALR(1)**. Ela define formalmente todas as construções sintáticas válidas da linguagem, sendo especificada no arquivo `src/parser/parser.y`.

### 1.1. Componentes Formais da Gramática

Seguindo a definição formal $G = \langle V, \Sigma, P, S \rangle$:

| Componente | Descrição |
|------------|-----------|
| **$V$ — Não-Terminais** | Símbolos abstratos que representam estruturas sintáticas (ex: `program`, `statement`, `expression`) |
| **$\Sigma$ — Terminais** | Tokens produzidos pelo lexer (ex: `INT`, `IDENTIFIER`, `PLUS`, `SEMICOLON`) |
| **$P$ — Produções** | Regras de substituição que definem como não-terminais se expandem |
| **$S$ — Símbolo Inicial** | `program` — ponto de partida de toda derivação |

---

## 2. Tokens (Símbolos Terminais)

Os tokens a seguir são produzidos pelo analisador léxico (`src/lexer/lexer.l`) e consumidos pelo parser.

### 2.1. Palavras-Chave de Tipo

| Token | Lexema |
|-------|--------|
| `INT` | `int` |
| `FLOAT` | `float` |
| `DOUBLE` | `double` |
| `CHAR` | `char` |
| `BOOL` | `bool` |
| `VOID` | `void` |

### 2.2. Palavras-Chave de Controle de Fluxo

| Token | Lexema |
|-------|--------|
| `IF` | `if` |
| `ELSE` | `else` |
| `FOR` | `for` |
| `WHILE` | `while` |
| `BREAK` | `break` |
| `CONTINUE` | `continue` |
| `RETURN` | `return` |
| `MAIN` | `main` |

### 2.3. Palavras-Chave Lógicas

| Token | Lexema |
|-------|--------|
| `AND` | `and` |
| `OR` | `or` |
| `NOT` | `not` |

### 2.4. Operadores Aritméticos e de Atribuição

| Token | Lexema |
|-------|--------|
| `PLUS` | `+` |
| `MINUS` | `-` |
| `MULT` | `*` |
| `DIV` | `/` |
| `MOD` | `%` |
| `ASSIGN` | `=` |
| `PLUSEQ` | `+=` |
| `MINUSEQ` | `-=` |
| `MULTEQ` | `*=` |
| `DIVEQ` | `/=` |
| `MODEQ` | `%=` |

### 2.5. Operadores Relacionais e Unários

| Token | Lexema |
|-------|--------|
| `EQ` | `==` |
| `NEQ` | `!=` |
| `LT` | `<` |
| `GT` | `>` |
| `LE` | `<=` |
| `GE` | `>=` |
| `INC` | `++` |
| `DEC` | `--` |
| `NOT_OP` | `!` |

### 2.6. Delimitadores

| Token | Lexema |
|-------|--------|
| `LPAREN` | `(` |
| `RPAREN` | `)` |
| `LBRACE` | `{` |
| `RBRACE` | `}` |
| `SEMICOLON` | `;` |
| `COMMA` | `,` |

### 2.7. Literais e Identificadores

| Token | Descrição |
|-------|-----------|
| `IDENTIFIER` | Nome de variável ou função (`strval`) |
| `INTEGER_LITERAL` | Literal inteiro (`intval`) |
| `FLOAT_LITERAL` | Literal ponto flutuante com sufixo `f`/`F` (`floatval`) |
| `DOUBLE_LITERAL` | Literal ponto flutuante de dupla precisão (`doubleval`) |
| `CHAR_LITERAL` | Literal caractere entre aspas simples (`charval`) |
| `BOOL_LITERAL` | Literal booleano `true` (1) ou `false` (0) (`intval`) |

---

## 3. Gramática Completa em BNF

### 3.1. Símbolo Inicial e Estrutura do Programa

```bnf
program
    : main_function
    | global_declaration_list main_function
    ;
```

> Um programa C-- Strict consiste em uma função `main` obrigatória, opcionalmente precedida por declarações globais.

---

### 3.2. Declarações Globais

```bnf
global_declaration_list
    : global_declaration
    | global_declaration_list global_declaration
    ;

global_declaration
    : function_declaration
    | variable_declaration SEMICOLON
    ;
```

> O escopo global aceita funções (com ou sem corpo) e variáveis globais terminadas por `;`.

---

### 3.3. Função Principal

```bnf
main_function
    : INT MAIN LPAREN RPAREN block
    ;
```

> A função `main` é obrigatória, deve retornar `int` e não aceita parâmetros.

---

### 3.4. Declaração de Função

```bnf
function_declaration
    : type_specifier IDENTIFIER LPAREN optional_parameters RPAREN block
    | type_specifier IDENTIFIER LPAREN optional_parameters RPAREN SEMICOLON
    ;
```

> A primeira alternativa é a **definição** (com corpo); a segunda é a **declaração/protótipo** (sem corpo, terminada com `;`).

---

### 3.5. Especificador de Tipo

```bnf
type_specifier
    : INT
    | FLOAT
    | DOUBLE
    | CHAR
    | BOOL
    | VOID
    ;
```

---

### 3.6. Parâmetros de Função

```bnf
optional_parameters
    : parameter_list
    | /* vazio */
    ;

parameter_list
    : parameter
    | parameter_list COMMA parameter
    ;

parameter
    : type_specifier IDENTIFIER
    ;
```

> Funções podem ter zero ou mais parâmetros. Cada parâmetro é composto por um tipo e um identificador.

---

### 3.7. Declaração de Variável

```bnf
variable_declaration
    : type_specifier init_declarator_list
    ;

init_declarator_list
    : init_declarator
    | init_declarator_list COMMA init_declarator
    ;

init_declarator
    : IDENTIFIER optional_initializer
    ;

optional_initializer
    : ASSIGN expression
    | /* vazio */
    ;
```

> Múltiplas variáveis do mesmo tipo podem ser declaradas em uma única instrução, separadas por vírgulas. A inicialização é opcional.
>
> Exemplo: `int x = 1, y, z = 3;`

---

### 3.8. Blocos de Código

```bnf
block
    : LBRACE statement_list RBRACE
    ;
```

> Um bloco é delimitado por chaves e contém uma lista de instruções e declarações locais intercaladas.

---

### 3.9. Lista de Instruções e Declarações Locais

```bnf
statement_list
    : /* vazio */
    | statement_list statement
    | statement_list local_declaration
    ;

local_declaration
    : function_declaration
    | variable_declaration SEMICOLON
    ;
```

> Dentro de um bloco, declarações locais (variáveis e funções) podem ser intercaladas livremente com instruções. A lista pode ser vazia.

---

### 3.10. Instrução (Statement)

```bnf
statement
    : matched_statement
    | unmatched_statement
    ;
```

> A distinção entre `matched_statement` e `unmatched_statement` resolve a ambiguidade do *dangling else*.

#### 3.10.1. Instrução Emparelhada (Matched)

```bnf
matched_statement
    : block
    | jump_statement
    | expression_statement
    | WHILE LPAREN expression RPAREN matched_statement
    | FOR LPAREN optional_for_initializer SEMICOLON optional_for_condition SEMICOLON optional_for_step RPAREN matched_statement
    | IF LPAREN expression RPAREN matched_statement ELSE matched_statement
    ;
```

> Em uma instrução emparelhada, todo `if` possui um `else` correspondente.

#### 3.10.2. Instrução Não-Emparelhada (Unmatched)

```bnf
unmatched_statement
    : IF LPAREN expression RPAREN statement
    | IF LPAREN expression RPAREN matched_statement ELSE unmatched_statement
    | WHILE LPAREN expression RPAREN unmatched_statement
    | FOR LPAREN optional_for_initializer SEMICOLON optional_for_condition SEMICOLON optional_for_step RPAREN unmatched_statement
    ;
```

> Em uma instrução não-emparelhada, existe pelo menos um `if` sem `else` correspondente.

---

### 3.11. Instrução de Salto

```bnf
jump_statement
    : BREAK SEMICOLON
    | CONTINUE SEMICOLON
    | RETURN optional_expression SEMICOLON
    ;
```

---

### 3.12. Instrução de Expressão

```bnf
expression_statement
    : optional_expression SEMICOLON
    ;

optional_expression
    : expression
    | /* vazio */
    ;
```

> Uma expressão seguida de `;` constitui uma instrução. A expressão pode ser omitida (instrução vazia: `;`).

---

### 3.13. Componentes do Loop `for`

```bnf
optional_for_initializer
    : expression
    | variable_declaration
    | /* vazio */
    ;

optional_for_condition
    : expression
    | /* vazio */
    ;

optional_for_step
    : expression
    | /* vazio */
    ;
```

> O loop `for` possui três cláusulas opcionais: inicialização (que pode ser uma declaração de variável), condição e passo.
>
> Exemplo: `for (int i = 0; i < 10; i++) { ... }`

---

## 4. Expressões e Hierarquia de Precedência

A hierarquia de precedência é codificada diretamente na estrutura da gramática, do nível mais baixo (menor precedência) ao mais alto (maior precedência).

### 4.1. Tabela de Precedência

| Nível | Categoria | Operadores | Associatividade |
|-------|-----------|-----------|-----------------|
| 1 (mais baixa) | Atribuição | `=`, `+=`, `-=`, `*=`, `/=`, `%=` | Direita |
| 2 | Lógico OR | `or` | Esquerda |
| 3 | Lógico AND | `and` | Esquerda |
| 4 | Relacional | `<`, `>`, `<=`, `>=`, `==`, `!=` | Esquerda |
| 5 | Aditivo | `+`, `-` | Esquerda |
| 6 | Multiplicativo | `*`, `/`, `%` | Esquerda |
| 7 | Unário (prefixo) | `!`, `not`, `-`, `++`, `--` | Direita |
| 8 (mais alta) | Primário/Pós-fixo | literais, `()`, `id++`, `id--` | — |

### 4.2. Regras de Expressão

```bnf
expression
    : assignment_expression
    ;
```

#### 4.2.1. Expressão de Atribuição

```bnf
assignment_expression
    : logical_or_expression
    | IDENTIFIER assignment_operator assignment_expression
    ;

assignment_operator
    : ASSIGN
    | PLUSEQ
    | MINUSEQ
    | MULTEQ
    | DIVEQ
    | MODEQ
    ;
```

> A atribuição é **associativa à direita**: `a = b = c` é interpretado como `a = (b = c)`.

#### 4.2.2. Expressão Lógica OR

```bnf
logical_or_expression
    : logical_and_expression
    | logical_or_expression OR logical_and_expression
    ;
```

#### 4.2.3. Expressão Lógica AND

```bnf
logical_and_expression
    : relational_expression
    | logical_and_expression AND relational_expression
    ;
```

#### 4.2.4. Expressão Relacional

```bnf
relational_expression
    : additive_expression
    | relational_expression relational_operator additive_expression
    ;

relational_operator
    : GT
    | LT
    | GE
    | LE
    | EQ
    | NEQ
    ;
```

#### 4.2.5. Expressão Aditiva

```bnf
additive_expression
    : multiplicative_expression
    | additive_expression PLUS multiplicative_expression
    | additive_expression MINUS multiplicative_expression
    ;
```

#### 4.2.6. Expressão Multiplicativa

```bnf
multiplicative_expression
    : unary_expression
    | multiplicative_expression MULT unary_expression
    | multiplicative_expression DIV unary_expression
    | multiplicative_expression MOD unary_expression
    ;
```

#### 4.2.7. Expressão Unária

```bnf
unary_expression
    : primary_expression
    | unary_operator unary_expression
    | IDENTIFIER INC
    | IDENTIFIER DEC
    ;

unary_operator
    : NOT_OP
    | NOT
    | MINUS
    | INC
    | DEC
    ;
```

> As formas `IDENTIFIER INC` e `IDENTIFIER DEC` representam o **pós-incremento** (`x++`) e **pós-decremento** (`x--`).
> Os demais operadores unários (`++x`, `--x`, `-x`, `!x`, `not x`) são tratados como **prefixo**.

#### 4.2.8. Expressão Primária

```bnf
primary_expression
    : IDENTIFIER optional_call_suffix
    | INTEGER_LITERAL
    | FLOAT_LITERAL
    | DOUBLE_LITERAL
    | BOOL_LITERAL
    | CHAR_LITERAL
    | LPAREN expression RPAREN
    | UNKNOWN_TOKEN
    ;
```

> Um `IDENTIFIER` seguido de `optional_call_suffix` pode ser tanto um acesso a variável quanto uma chamada de função.

---

### 4.3. Chamadas de Função e Argumentos

```bnf
optional_call_suffix
    : LPAREN optional_argument_list RPAREN
    | /* vazio */
    ;

optional_argument_list
    : /* vazio */
    | argument_list
    ;

argument_list
    : expression
    | argument_list COMMA expression
    ;
```

> Uma chamada de função aceita zero ou mais argumentos separados por vírgula.

---

## 5. Resolução da Ambiguidade do *Dangling Else*

### 5.1. O Problema

A construção `if ... if ... else` é naturalmente ambígua:

```c
if (a > 0)
    if (b > 0)
        x = 1;
    else
        x = 2;  // A qual 'if' pertence este 'else'?
```

### 5.2. Solução Adotada

A gramática resolve essa ambiguidade formalmente através da distinção entre `matched_statement` e `unmatched_statement`:

- **`matched_statement`**: Toda estrutura `if` possui exatamente um `else` correspondente.
- **`unmatched_statement`**: Existe pelo menos um `if` sem `else`.

```bnf
matched_statement
    : IF LPAREN expression RPAREN matched_statement ELSE matched_statement
    ;

unmatched_statement
    : IF LPAREN expression RPAREN statement
    | IF LPAREN expression RPAREN matched_statement ELSE unmatched_statement
    ;
```

**Resultado:** O `else` sempre se associa ao `if` mais próximo ainda sem `else`, eliminando qualquer conflito *shift/reduce* no Bison.

---

## 6. Tipos Semânticos da União Bison

O parser utiliza a seguinte `%union` para armazenar valores de tokens e construir a AST:

```c
%union {
    int intval;         /* Valores inteiros e booleanos */
    float floatval;     /* Valores ponto flutuante (float) */
    double doubleval;   /* Valores ponto flutuante (double) */
    char charval;       /* Caracteres */
    char *strval;       /* Identificadores */

    /* Nós da AST */
    class ASTNode*      ast_node;
    class BlockNode*    block_node;
    class FuncDeclNode* func_decl_node;
    class ProgramNode*  prog_node;
    class VarDeclNode*  var_decl_node;

    /* Listas de nós */
    std::vector<class ASTNode*>*    node_list;
    std::vector<class VarDeclNode*>* param_list;
}
```

---

## 7. Não-Terminais da Gramática

A tabela abaixo lista todos os símbolos não-terminais definidos em `src/parser/parser.y`:

| Não-Terminal | Tipo Semântico | Descrição |
|---|---|---|
| `program` | `prog_node` | Símbolo inicial; raiz da AST |
| `main_function` | `func_decl_node` | Declaração da função `main` |
| `function_declaration` | `func_decl_node` | Definição ou protótipo de função |
| `global_declaration_list` | `node_list` | Lista de declarações no escopo global |
| `global_declaration` | `node_list` | Uma declaração global (função ou variável) |
| `type_specifier` | `intval` | Especificador de tipo primitivo |
| `variable_declaration` | `node_list` | Declaração de variável com tipo |
| `init_declarator_list` | `node_list` | Lista de declaradores com inicialização opcional |
| `init_declarator` | `ast_node` | Um único declarador com inicialização opcional |
| `optional_initializer` | `ast_node` | Inicialização opcional de variável |
| `optional_parameters` | `param_list` | Lista de parâmetros ou vazio |
| `parameter_list` | `param_list` | Lista não-vazia de parâmetros |
| `parameter` | `var_decl_node` | Um parâmetro de função |
| `block` | `block_node` | Bloco de código delimitado por `{}` |
| `local_declaration` | `node_list` | Declaração local (dentro de bloco) |
| `statement_list` | `node_list` | Lista de instruções e declarações locais |
| `statement` | `ast_node` | Instrução genérica |
| `matched_statement` | `ast_node` | Instrução com `if-else` completo |
| `unmatched_statement` | `ast_node` | Instrução com `if` sem `else` |
| `jump_statement` | `ast_node` | `break`, `continue` ou `return` |
| `expression_statement` | `ast_node` | Expressão seguida de `;` |
| `optional_expression` | `ast_node` | Expressão ou vazio |
| `optional_for_initializer` | `ast_node` | Cláusula de inicialização do `for` |
| `optional_for_condition` | `ast_node` | Cláusula de condição do `for` |
| `optional_for_step` | `ast_node` | Cláusula de incremento do `for` |
| `expression` | `ast_node` | Expressão genérica |
| `assignment_expression` | `ast_node` | Expressão de atribuição |
| `assignment_operator` | `intval` | Operador de atribuição |
| `logical_or_expression` | `ast_node` | Expressão lógica OR |
| `logical_and_expression` | `ast_node` | Expressão lógica AND |
| `relational_expression` | `ast_node` | Expressão relacional |
| `relational_operator` | `intval` | Operador relacional |
| `additive_expression` | `ast_node` | Expressão aditiva |
| `multiplicative_expression` | `ast_node` | Expressão multiplicativa |
| `unary_expression` | `ast_node` | Expressão unária |
| `unary_operator` | `intval` | Operador unário |
| `primary_expression` | `ast_node` | Expressão primária |
| `optional_call_suffix` | `node_list` | Sufixo de chamada de função |
| `optional_argument_list` | `node_list` | Lista de argumentos ou vazio |
| `argument_list` | `node_list` | Lista não-vazia de argumentos |
