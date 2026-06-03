# Restrições da Linguagem para Parsing

## 1. Visão Geral

O compilador **C-- Strict** implementa um subconjunto rigorosamente restrito da linguagem C. As limitações listadas neste documento são restrições **formalmente codificadas na gramática** (`src/parser/parser.y`) ou herdadas da especificação léxica (`src/lexer/lexer.l`). Construções ausentes não são reconhecidas pelo parser — qualquer tentativa de usá-las resultará em erro sintático.

---

## 2. Restrições Estruturais do Programa

### 2.1. Função `main` Obrigatória

Todo programa C-- Strict **deve** conter uma função `main`. O símbolo inicial da gramática é:

```bnf
program
    : main_function
    | global_declaration_list main_function
    ;
```

Não existe alternativa sem `main_function`. Um arquivo sem a função `main` é sintaticamente inválido.

### 2.2. Assinatura Fixa da Função `main`

A regra para `main_function` é:

```bnf
main_function
    : INT MAIN LPAREN RPAREN block
    ;
```

Isso impõe três restrições inegociáveis:

| Restrição | Justificativa |
|-----------|---------------|
| Tipo de retorno obrigatoriamente `int` | A única alternativa na produção usa `INT` |
| Sem parâmetros | `LPAREN RPAREN` sem `optional_parameters` |
| Deve ter corpo (`block`) | Sem alternativa com `SEMICOLON` |

Variações comuns do C padrão como `int main(int argc, char *argv[])` **não são suportadas**.

### 2.3. Declarações Globais Apenas Antes de `main`

A gramática só permite `global_declaration_list` **antes** da `main_function`. Não é possível intercalar declarações globais após a função `main`.

---

## 3. Restrições de Tipos

### 3.1. Apenas Tipos Primitivos

O especificador de tipo está restrito a:

```bnf
type_specifier
    : INT | FLOAT | DOUBLE | CHAR | BOOL | VOID
    ;
```

Os seguintes tipos do C padrão **não existem** na gramática:

- `unsigned`, `signed`, `short`, `long`, `long long`
- `struct`, `union`, `enum`
- Tipos ponteiro (`int *`, `char *`, etc.)
- `typedef`
- Tipos definidos pelo usuário

### 3.2. Sem Suporte a Arrays

A gramática não possui nenhuma produção com `[` ou `]`. Declarações como `int v[10]` ou acessos como `v[i]` são **sintaticamente inválidos**. O lexer não define token para `[` ou `]`.

### 3.3. Sem Suporte a Ponteiros

Não há produções para os operadores `*` (derreferência) ou `&` (endereço). O `*` existe apenas como operador multiplicativo em `multiplicative_expression`. Declarações com ponteiros (`int *p`) são **inválidas**.

---

## 4. Restrições em Funções

### 4.1. Sem Funções Variádicas

A gramática de parâmetros não inclui `...` (reticências):

```bnf
parameter_list
    : parameter
    | parameter_list COMMA parameter
    ;

parameter
    : type_specifier IDENTIFIER
    ;
```

Funções como `printf(const char *fmt, ...)` são impossíveis de declarar.

### 4.2. Sem Funções Anônimas ou Ponteiros para Funções

Não existem produções para declaração de ponteiros para função nem para expressões lambda.

### 4.3. Parâmetros Somente por Valor

Cada parâmetro é da forma `type_specifier IDENTIFIER`. Não há mecanismo para passagem por referência ou ponteiro.

---

## 5. Restrições em Expressões

### 5.1. Atribuição Apenas para Identificadores Simples

A produção de atribuição é:

```bnf
assignment_expression
    : logical_or_expression
    | IDENTIFIER assignment_operator assignment_expression
    ;
```

O lado esquerdo de uma atribuição **deve ser um `IDENTIFIER` simples**. Não é possível atribuir a:

- Elementos de array (`v[i] = 5`)
- Campos de struct (`obj.campo = 5`)
- Expressões derreferenciadas (`*p = 5`)

### 5.2. Sem Operador Ternário

O operador condicional `? :` não possui produção na gramática.

### 5.3. Sem Operador Vírgula como Expressão

A vírgula existe apenas como separador em listas de parâmetros, declarações múltiplas e listas de argumentos. Não há suporte para o operador vírgula do C (`expr1, expr2`).

### 5.4. Sem Operadores Bit a Bit

Os operadores `&`, `|`, `^`, `~`, `<<`, `>>` não possuem tokens definidos no lexer nem produções na gramática.

### 5.5. Sem Operador `sizeof`

A palavra-chave `sizeof` não é um token reconhecido.

### 5.6. Pós-Incremento/Decremento Restrito a Identificadores

```bnf
unary_expression
    : IDENTIFIER INC
    | IDENTIFIER DEC
    ;
```

O pós-incremento e o pós-decremento aceitam apenas `IDENTIFIER` como operando. Expressões como `(a + b)++` são inválidas.

---

## 6. Restrições em Estruturas de Controle

### 6.1. `if` e `else` Somente com Expressão Booleana Qualquer

A condição de `if`, `while` e `for` é uma `expression` genérica — qualquer expressão é aceita sintaticamente. A validação semântica de que a condição é utilizável como booleano é responsabilidade da fase semântica.

### 6.2. Sem `switch/case`

Não existem tokens `SWITCH`, `CASE` ou `DEFAULT` nem produções correspondentes.

### 6.3. Sem `do...while`

Não há produção para o laço `do { } while ();`.

### 6.4. `break` e `continue` Sem Rótulo

```bnf
jump_statement
    : BREAK SEMICOLON
    | CONTINUE SEMICOLON
    ;
```

As instruções `break` e `continue` não aceitam rótulos (`break label;`), presentes em algumas linguagens.

### 6.5. `goto` Não Suportado

A palavra-chave `goto` não é um token reconhecido pelo lexer.

---

## 7. Restrições em Declarações

### 7.1. Sem Qualificadores de Tipo

Os qualificadores `const`, `static`, `extern`, `volatile` e `register` não existem na gramática.

### 7.2. Sem Inicialização em Massa

Não há suporte a inicializadores com listas entre chaves:

```c
int v[3] = {1, 2, 3};  /* inválido — sem arrays */
```

### 7.3. Declarações Locais Intercaladas com Instruções

A gramática permite que declarações locais apareçam em qualquer ponto dentro de um bloco, intercaladas com instruções:

```bnf
statement_list
    : statement_list statement
    | statement_list local_declaration
    ;
```

Isso é mais permissivo que o C89/C90 (que exige declarações no início do bloco), mas compatível com C99 e C++.

---

## 8. Restrições Léxicas com Impacto Sintático

### 8.1. Sem Literais String

O lexer (`src/lexer/lexer.l`) não define um token para strings entre aspas duplas (`"texto"`). Portanto, literais string são **sintaticamente inválidos**.

### 8.2. Literais Numéricos

| Tipo | Formato Aceito | Exemplo |
|------|---------------|---------|
| Inteiro | Sequência de dígitos | `42`, `0`, `100` |
| Float | Notação decimal com sufixo `f` ou `F` | `1.5f`, `2.0F` |
| Double | Notação decimal ou exponencial sem sufixo | `3.14`, `1e10`, `.5` |
| Char | Caractere entre aspas simples | `'a'`, `'\n'`, `'\0'` |
| Bool | `true` ou `false` | — |

Formatos hexadecimais (`0xFF`), octais (`077`) e binários (`0b1010`) **não são reconhecidos** pelo lexer.

### 8.3. Sequências de Escape em Char

O lexer reconhece as seguintes sequências de escape em literais `char`:

| Sequência | Valor ASCII |
|-----------|-------------|
| `\n` | 10 (nova linha) |
| `\t` | 9 (tabulação) |
| `\r` | 13 (retorno de carro) |
| `\0` | 0 (nulo) |
| `\\`, `\'`, outros | valor literal do caractere |

---

## 9. Resumo das Restrições

| Construção C | Suportada? | Motivo |
|---|:---:|---|
| Função `main` | Sim (obrigatória) | `main_function` na gramática |
| Tipos primitivos (`int`, `float`, etc.) | Sim | `type_specifier` |
| Declaração múltipla (`int a, b;`) | Sim | `init_declarator_list` |
| Protótipos de função | Sim | `function_declaration` com `SEMICOLON` |
| `if / else` | Sim | `matched/unmatched_statement` |
| `while` | Sim | `matched/unmatched_statement` |
| `for` | Sim | `matched/unmatched_statement` |
| `break` / `continue` | Sim | `jump_statement` |
| `return` | Sim | `jump_statement` |
| Pré/pós-incremento (`++`, `--`) | Sim | `unary_expression` |
| Comentários `//` e `/* */` | Sim | Lexer (ignorados) |
| Arrays (`[]`) | **Não** | Sem token/produção |
| Ponteiros (`*`, `&`) | **Não** | Sem token/produção |
| `struct` / `union` / `enum` | **Não** | Sem token/produção |
| `switch` / `case` | **Não** | Sem token/produção |
| `do ... while` | **Não** | Sem produção |
| `goto` | **Não** | Sem token/produção |
| Operador ternário (`? :`) | **Não** | Sem produção |
| Operadores bit a bit | **Não** | Sem token/produção |
| `sizeof` | **Não** | Sem token/produção |
| Literais string (`"..."`) | **Não** | Sem token no lexer |
| Qualificadores (`const`, `static`) | **Não** | Sem token/produção |
| Literais hexadecimais/octais | **Não** | Sem padrão no lexer |
| Funções variádicas (`...`) | **Não** | Sem produção |
