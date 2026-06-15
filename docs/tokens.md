# Análise Léxica — Tokens da Linguagem C-- Strict

Esta página documenta todos os tokens reconhecidos pelo analisador léxico do compilador (definidos em `src/lexer/lexer.l` e declarados em `src/parser/parser.y`).

---

## 1. Palavras-chave de Tipo

| Token   | Lexema   | Descrição              |
|---------|----------|------------------------|
| `INT`   | `int`    | Tipo inteiro           |
| `FLOAT` | `float`  | Ponto flutuante simples |
| `DOUBLE`| `double` | Ponto flutuante duplo  |
| `CHAR`  | `char`   | Caractere              |
| `BOOL`  | `bool`   | Booleano               |
| `VOID`  | `void`   | Sem retorno (funções)  |

---

## 2. Palavras-chave de Controle de Fluxo

| Token      | Lexema     | Descrição                      |
|------------|------------|--------------------------------|
| `IF`       | `if`       | Condicional                    |
| `ELSE`     | `else`     | Ramo alternativo do `if`       |
| `FOR`      | `for`      | Laço com inicialização e passo |
| `WHILE`    | `while`    | Laço com condição              |
| `BREAK`    | `break`    | Interrompe laço mais interno   |
| `CONTINUE` | `continue` | Vai para a próxima iteração    |
| `RETURN`   | `return`   | Retorna valor de uma função    |

---

## 3. Palavra-chave Especial

| Token  | Lexema | Descrição                              |
|--------|--------|----------------------------------------|
| `MAIN` | `main` | Nome obrigatório da função de entrada  |

---

## 4. Operadores Lógicos (palavras-chave)

| Token | Lexema | Descrição        |
|-------|--------|------------------|
| `AND` | `and`  | E lógico         |
| `OR`  | `or`   | Ou lógico        |
| `NOT` | `not`  | Negação lógica   |

---

## 5. Operadores Aritméticos

| Token   | Lexema | Descrição       |
|---------|--------|-----------------|
| `PLUS`  | `+`    | Adição          |
| `MINUS` | `-`    | Subtração       |
| `MULT`  | `*`    | Multiplicação   |
| `DIV`   | `/`    | Divisão         |
| `MOD`   | `%`    | Módulo (resto)  |

---

## 6. Operadores Relacionais

| Token | Lexema | Descrição       |
|-------|--------|-----------------|
| `EQ`  | `==`   | Igual a         |
| `NEQ` | `!=`   | Diferente de    |
| `LT`  | `<`    | Menor que       |
| `GT`  | `>`    | Maior que       |
| `LE`  | `<=`   | Menor ou igual  |
| `GE`  | `>=`   | Maior ou igual  |

---

## 7. Operadores de Atribuição

| Token     | Lexema | Descrição                      |
|-----------|--------|--------------------------------|
| `ASSIGN`  | `=`    | Atribuição simples             |
| `PLUSEQ`  | `+=`   | Atribuição com adição          |
| `MINUSEQ` | `-=`   | Atribuição com subtração       |
| `MULTEQ`  | `*=`   | Atribuição com multiplicação   |
| `DIVEQ`   | `/=`   | Atribuição com divisão         |
| `MODEQ`   | `%=`   | Atribuição com módulo          |

---

## 8. Operadores Unários

| Token    | Lexema | Descrição                              |
|----------|--------|----------------------------------------|
| `INC`    | `++`   | Incremento (prefixo ou pós-fixo)       |
| `DEC`    | `--`   | Decremento (prefixo ou pós-fixo)       |
| `NOT_OP` | `!`    | Negação lógica (símbolo, como em C)    |

> **Nota:** `++` e `--` são tratados como operadores pós-fixos quando aparecem na forma `IDENTIFIER INC` / `IDENTIFIER DEC` na gramática (regra `unary_expression` do parser), e como prefixos nas demais posições.

---

## 9. Delimitadores e Pontuação

| Token       | Lexema | Descrição              |
|-------------|--------|------------------------|
| `LPAREN`    | `(`    | Parêntese esquerdo     |
| `RPAREN`    | `)`    | Parêntese direito      |
| `LBRACE`    | `{`    | Chave esquerda         |
| `RBRACE`    | `}`    | Chave direita          |
| `SEMICOLON` | `;`    | Ponto e vírgula        |
| `COMMA`     | `,`    | Vírgula                |

---

## 10. Literais

| Token             | Padrão (regex Flex)                                               | Exemplo      | Valor em `yylval`   |
|-------------------|-------------------------------------------------------------------|--------------|---------------------|
| `INTEGER_LITERAL` | `[0-9]+`                                                          | `42`         | `yylval.intval`     |
| `FLOAT_LITERAL`   | `([0-9]*\.[0-9]+\|[0-9]+\.)[eE][+-]?[0-9]+)?[fF]`               | `3.14f`      | `yylval.floatval`   |
| `DOUBLE_LITERAL`  | `([0-9]*\.[0-9]+\|[0-9]+\.)[eE][+-]?[0-9]+)?`                   | `2.718`      | `yylval.doubleval`  |
| `CHAR_LITERAL`    | `'([^\\']|\\.)'`                                                  | `'a'`, `'\n'`| `yylval.intval` (ASCII) |
| `BOOL_LITERAL`    | `true` / `false`                                                  | `true`       | `yylval.intval` (1 ou 0) |

> **Nota sobre `char`:** O lexer converte o literal de caractere para seu valor ASCII inteiro e o armazena em `yylval.intval`. Sequências de escape suportadas: `\n` (10), `\t` (9), `\r` (13), `\0` (0).

---

## 11. Identificadores

| Token        | Padrão (regex Flex)         | Exemplo       | Valor em `yylval`  |
|--------------|-----------------------------|---------------|--------------------|
| `IDENTIFIER` | `[a-zA-Z_][a-zA-Z0-9_]*`   | `soma`, `x1`  | `yylval.strval` (alocado com `strdup`) |

---

## 12. Token Especial de Erro

| Token           | Condição                                   | Ação                                           |
|-----------------|--------------------------------------------|------------------------------------------------|
| `UNKNOWN_TOKEN` | Qualquer caractere não reconhecido (`.`)   | Imprime erro em `stderr` e retorna o token     |

Mensagem de erro emitida:
```
Erro léxico: '<caractere>' na linha <N>
```

---

## 13. Elementos Ignorados

Os seguintes padrões são reconhecidos pelo lexer mas **não geram tokens** — são silenciosamente descartados:

| Padrão                    | Descrição                |
|---------------------------|--------------------------|
| `[ \t\r]+`                | Espaços e tabulações     |
| `\n`                      | Nova linha (incrementa `line`) |
| `//.*`                    | Comentário de linha      |
| `/*` ... `*/`             | Comentário de bloco      |

---

## 14. Union de Valores Semânticos (`yylval`)

O parser usa a seguinte `%union` para transportar valores dos tokens:

```c
%union {
    int    intval;    // INTEGER_LITERAL, BOOL_LITERAL, CHAR_LITERAL, tipo de token
    float  floatval;  // FLOAT_LITERAL
    double doubleval; // DOUBLE_LITERAL
    char   charval;   // (reservado)
    char  *strval;    // IDENTIFIER (alocado com strdup, liberado pelo parser)

    // Nós da AST (usados pelos não-terminais)
    ASTNode      *ast_node;
    BlockNode    *block_node;
    FuncDeclNode *func_decl_node;
    ProgramNode  *prog_node;
    VarDeclNode  *var_decl_node;

    std::vector<ASTNode*>    *node_list;
    std::vector<VarDeclNode*>*param_list;
}
```
