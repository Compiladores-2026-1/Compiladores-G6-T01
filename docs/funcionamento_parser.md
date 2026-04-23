# Funcionamento do Analisador Sintático (Parser)

## 1. Introdução

O **Analisador Sintático** (Parser) é a segunda fase do compilador para C-- Strict. Sua responsabilidade é validar que a sequência de *tokens* gerada pelo analisador léxico segue a gramática formal da linguagem, construindo uma representação interna denominada **Árvore de Sintaxe Abstrata (AST - Abstract Syntax Tree)**.

### 1.1. Objetivo

O parser possui dois objetivos principais:

1. **Validação Sintática:** Garantir que o código respeita as regras gramaticais da linguagem C-- Strict
2. **Construção de AST:** Criar uma estrutura em árvore que representa a hierarquia lógica do programa

### 1.2. Ferramenta Utilizada

O parser é implementado através do **Bison (GNU Parser Generator)**, que utiliza a técnica de análise **LALR(1)** para parsing. O arquivo de especificação da gramática é `src/parser/parser.y`.

---

## 2. Arquitetura Geral do Parser

### 2.1. Fluxo de Entrada e Saída

```
Tokens do Lexer
    │
    ├─► Parser (Bison)
    │   ├─► Verifica regras gramaticais
    │   ├─► Detecta erros sintáticos
    │   └─► Constrói AST
    │
    └─► AST (Árvore de Sintaxe Abstrata)
        ou
        Mensagens de Erro
```

### 2.2. Componentes Principais

1. **Declarações de Tokens:** Definem todos os tokens reconhecidos pelo lexer
2. **Regras Gramaticais:** Especificam como os tokens podem ser combinados
3. **Ações Semânticas:** Código associado a cada regra para construção da AST
4. **Tratamento de Erros:** Mecanismos para recuperação de erros sintáticos

---

## 3. Tokens e Tipos Semânticos

### 3.1. Tipos de Dados Semânticos

O parser utiliza a seguinte união de tipos para armazenar valores dos tokens:

```c
%union {
    int intval;           // Valores inteiros
    float floatval;       // Valores ponto flutuante
    double doubleval;     // Valores double precision
    char charval;         // Caracteres
    char *strval;         // Strings (identificadores)
}
```

### 3.2. Tokens de Palavra-Chave

| Token | Significado |
|-------|-------------|
| `INT`, `FLOAT`, `DOUBLE`, `CHAR`, `BOOL`, `VOID` | Especificadores de tipo |
| `IF`, `ELSE` | Estruturas condicionais |
| `FOR`, `WHILE` | Estruturas de iteração |
| `BREAK`, `CONTINUE` | Controle de fluxo em loops |
| `RETURN` | Instrução de retorno |
| `MAIN` | Função principal |

### 3.3. Tokens de Operador

| Token | Símbolo | Tipo |
|-------|---------|------|
| `PLUS` | `+` | Aritmético |
| `MINUS` | `-` | Aritmético |
| `MULT` | `*` | Aritmético |
| `DIV` | `/` | Aritmético |
| `MOD` | `%` | Aritmético |
| `ASSIGN` | `=` | Atribuição |
| `PLUSEQ` | `+=` | Atribuição composta |
| `MINUSEQ` | `-=` | Atribuição composta |
| `MULTEQ` | `*=` | Atribuição composta |
| `DIVEQ` | `/=` | Atribuição composta |
| `MODEQ` | `%=` | Atribuição composta |

### 3.4. Tokens de Relação e Lógica

| Token | Símbolo | Significado |
|-------|---------|-------------|
| `EQ` | `==` | Igual a |
| `NEQ` | `!=` | Diferente de |
| `LT` | `<` | Menor que |
| `GT` | `>` | Maior que |
| `LE` | `<=` | Menor ou igual que |
| `GE` | `>=` | Maior ou igual que |
| `AND` | `and` | E lógico |
| `OR` | `or` | OU lógico |
| `NOT` | `not` | NÃO lógico |
| `NOT_OP` | `!` | Negação lógica |

### 3.5. Tokens de Incremento e Decremento

| Token | Símbolo |
|-------|---------|
| `INC` | `++` |
| `DEC` | `--` |

### 3.6. Tokens de Delimitador

| Token | Símbolo |
|-------|---------|
| `LPAREN` | `(` |
| `RPAREN` | `)` |
| `LBRACE` | `{` |
| `RBRACE` | `}` |
| `SEMICOLON` | `;` |
| `COMMA` | `,` |

---

## 4. Gramática em BNF (Backus-Naur Form)

### 4.1. Estrutura de Programa

```bnf
program
    : main_function
    | global_declaration_list main_function
    ;
```

**Significado:** Um programa consiste em uma função `main` obrigatória, opcionalmente precedida por declarações globais.

### 4.2. Declarações Globais

```bnf
global_declaration_list
    : global_declaration
    | global_declaration_list global_declaration
    ;

global_declaration
    : type_specifier IDENTIFIER global_declaration_tail
    ;

global_declaration_tail
    : LPAREN optional_parameters RPAREN block           /* Função com definição */
    | LPAREN optional_parameters RPAREN SEMICOLON      /* Declaração de função */
    | global_variable_tail SEMICOLON                    /* Variável global */
    ;
```

**Significado:** Declarações globais podem ser funções (com ou sem implementação) ou variáveis globais.

### 4.3. Função Main

```bnf
main_function
    : INT MAIN LPAREN RPAREN block
    ;
```

**Significado:** A função `main` deve ser do tipo `int`, sem parâmetros, e deve ter um bloco de implementação.

### 4.4. Especificadores de Tipo

```bnf
type_specifier
    : INT | FLOAT | DOUBLE | CHAR | BOOL | VOID
    ;
```

**Significado:** C-- Strict suporta seis tipos primitivos.

### 4.5. Parâmetros de Função

```bnf
optional_parameters
    : parameter_list
    | /* empty */
    ;

parameter_list
    : parameter parameter_list_tail
    ;

parameter_list_tail
    : COMMA parameter parameter_list_tail
    | /* empty */
    ;

parameter
    : type_specifier IDENTIFIER
    ;
```

**Significado:** Funções podem ter zero ou mais parâmetros, cada um com tipo e identificador.

### 4.6. Blocos de Código

```bnf
block
    : LBRACE local_declaration_list statement_list RBRACE
    ;
```

**Significado:** Um bloco contém declarações locais opcionais seguidas de uma lista de instruções.

### 4.7. Declarações Locais

```bnf
local_declaration_list
    : local_declaration local_declaration_list
    | /* empty */
    ;

local_declaration
    : type_specifier IDENTIFIER LPAREN optional_parameters RPAREN SEMICOLON
    | type_specifier declaration_item_list SEMICOLON
    ;

declaration_item_list
    : IDENTIFIER optional_initializer declaration_item_list_tail
    ;

declaration_item_list_tail
    : COMMA IDENTIFIER optional_initializer declaration_item_list_tail
    | /* empty */
    ;
```

**Significado:** Variáveis locais podem ser declaradas com inicialização opcional. Múltiplas variáveis podem ser declaradas em uma única linha.

### 4.8. Inicialização

```bnf
optional_initializer
    : ASSIGN expression
    | /* empty */
    ;
```

**Significado:** Variáveis podem ser inicializadas ou deixadas sem inicialização.

---

## 5. Instruções (Statements)

### 5.1. Estrutura Geral

```bnf
statement_list
    : statement statement_list
    | /* empty */
    ;

statement
    : matched_statement
    | unmatched_statement
    ;
```

### 5.2. Instruções Emparelhadas (Matched)

```bnf
matched_statement
    : block
    | jump_statement
    | expression_statement
    | WHILE LPAREN expression RPAREN matched_statement
    | FOR LPAREN optional_for_initializer SEMICOLON optional_for_condition 
            SEMICOLON optional_for_step RPAREN matched_statement
    | IF LPAREN expression RPAREN matched_statement ELSE matched_statement
    ;
```

**Significado:** Instruções onde cada `IF` está associado a um `ELSE` (evita ambiguidade).

### 5.3. Instruções Não-Emparelhadas (Unmatched)

```bnf
unmatched_statement
    : IF LPAREN expression RPAREN statement
    | IF LPAREN expression RPAREN matched_statement ELSE unmatched_statement
    | WHILE LPAREN expression RPAREN unmatched_statement
    | FOR LPAREN optional_for_initializer SEMICOLON optional_for_condition 
            SEMICOLON optional_for_step RPAREN unmatched_statement
    ;
```

**Significado:** Instruções onde `IF` sem `ELSE` ou com `ELSE` associado a instrução não-emparelhada (evita *dangling else*).

### 5.4. Instruções de Salto

```bnf
jump_statement
    : BREAK SEMICOLON
    | CONTINUE SEMICOLON
    | RETURN optional_expression SEMICOLON
    ;
```

**Significado:** Instruções que alteram o fluxo de controle.

### 5.5. Instruções de Expressão

```bnf
expression_statement
    : optional_expression SEMICOLON
    ;

optional_expression
    : expression
    | /* empty */
    ;
```

**Significado:** Uma expressão seguida de ponto-e-vírgula constitui uma instrução.

### 5.6. Loop For

```bnf
optional_for_initializer
    : expression
    | type_specifier for_declaration_item_list
    | /* empty */
    ;

optional_for_condition
    : expression
    | /* empty */
    ;

optional_for_step
    : expression
    | /* empty */
    ;
```

**Significado:** O loop `for` permite inicialização com expressão ou declaração, condição opcional e passo opcional.

---

## 6. Expressões

### 6.1. Hierarquia de Precedência

O parser implementa expressões com precedência e associatividade corretas:

```
1. Primary (mais alta)      : identificadores, literais, (expressão)
2. Unary                     : ++, --, -, !
3. Multiplicative           : *, /, %
4. Additive                 : +, -
5. Relational               : <, >, <=, >=, ==, !=
6. Logical AND              : &&
7. Logical OR               : ||
8. Assignment (mais baixa)  : =, +=, -=, *=, /=, %=
```

### 6.2. Regras de Expressão

```bnf
expression
    : assignment_expression
    ;

assignment_expression
    : IDENTIFIER assignment_operator assignment_expression
    | logical_or_expression
    ;

assignment_operator
    : ASSIGN | PLUSEQ | MINUSEQ | MULTEQ | DIVEQ | MODEQ
    ;

logical_or_expression
    : logical_and_expression logical_or_expression_tail
    ;

logical_or_expression_tail
    : OR logical_and_expression logical_or_expression_tail
    | /* empty */
    ;

logical_and_expression
    : relational_expression logical_and_expression_tail
    ;

logical_and_expression_tail
    : AND relational_expression logical_and_expression_tail
    | /* empty */
    ;

relational_expression
    : additive_expression relational_expression_tail
    ;

relational_expression_tail
    : relational_operator additive_expression
    | /* empty */
    ;

additive_expression
    : multiplicative_expression additive_expression_tail
    ;

additive_expression_tail
    : PLUS multiplicative_expression additive_expression_tail
    | MINUS multiplicative_expression additive_expression_tail
    | /* empty */
    ;

multiplicative_expression
    : unary_expression multiplicative_expression_tail
    ;

multiplicative_expression_tail
    : MULT unary_expression multiplicative_expression_tail
    | DIV unary_expression multiplicative_expression_tail
    | MOD unary_expression multiplicative_expression_tail
    | /* empty */
    ;

unary_expression
    : unary_operator unary_expression
    | IDENTIFIER INC
    | IDENTIFIER DEC
    | primary_expression
    ;

primary_expression
    : IDENTIFIER optional_call_suffix
    | INTEGER_LITERAL
    | FLOAT_LITERAL
    | DOUBLE_LITERAL
    | BOOL_LITERAL
    | CHAR_LITERAL
    | LPAREN expression RPAREN
    ;
```

### 6.3. Chamadas de Função

```bnf
optional_call_suffix
    : LPAREN optional_argument_list RPAREN
    | /* empty */
    ;

optional_argument_list
    : argument_list
    | /* empty */
    ;

argument_list
    : expression argument_list_tail
    ;

argument_list_tail
    : COMMA expression argument_list_tail
    | /* empty */
    ;
```

**Significado:** Funções podem ser chamadas com zero ou mais argumentos.

---

## 7. Exemplo de Análise Sintática

### 7.1. Código-Fonte

```c
int soma(int a, int b)
{
    return a + b;
}

int main()
{
    int resultado = soma(5, 3);
    return resultado;
}
```

### 7.2. Fluxo de Parsing

```
Input: INT soma ( INT a , INT b ) { RETURN a + b ; } INT MAIN ( ) { ...

1. INT soma ( ... ) { ... }
   └─► Reconhecido como: global_declaration
       └─► type_specifier IDENTIFIER ( parameter_list ) block

2. parameter_list: INT a , INT b
   └─► Dois parâmetros: a (int) e b (int)

3. block: { RETURN a + b ; }
   └─► Contém uma instrução return
       └─► expression: a + b
           └─► additive_expression
               └─► multiplicative_expression (a)
                   PLUS multiplicative_expression (b)

4. INT MAIN ( ) { ... }
   └─► Reconhecido como: main_function

AST resultante:
Program
├─ Function: soma (int, int) -> int
│  └─ Block
│     └─ Return
│        └─ BinaryOp(ADD)
│           ├─ Var(a)
│           └─ Var(b)
└─ Main
   └─ Block
      ├─ VarDecl: resultado = Call(soma, 5, 3)
      └─ Return
         └─ Var(resultado)
```

### 7.3. Saída do Parser

```
[GLOBAL_DECL: soma]
[PARAM: a]
[PARAM: b]
[PARAMS]
[FUNCTION_DEF]
[BLOCK]
[BLOCK]
[RETURN]
[ID: a]
[ID: b]
[PLUS]
[EXPR_STMT]
[MAIN]
[DECL]
[VAR: resultado]
[CALL]
[ID: soma]
[NUM: 5]
[NUM: 3]
[EXPR_STMT]
[RETURN]
[ID: resultado]
[EXPR_STMT]
[BLOCK]
[PROGRAM]
```

---

## 8. Tratamento de Erros Sintáticos

### 8.1. Detecção de Erros

O parser detecta erros quando:

1. **Token inesperado:** Um token diferente do esperado é encontrado
2. **Fim de arquivo prematuro:** O programa termina sem completar uma construção
3. **Regra violada:** A sequência de tokens não corresponde a nenhuma regra

### 8.2. Mensagens de Erro

O compilador fornece mensagens de erro com informações precisas:

```
Erro sintático na linha 15: Esperado ';' após declaração
Erro sintático na linha 20: Token inesperado 'int' em contexto inválido
Erro sintático na linha 25: Esperado ')' para fechar parênteses
```

### 8.3. Função de Erro

```c
void yyerror(const char *s)
{
    fprintf(stderr, "Erro sintático na linha %d: %s\n", line, s);
}
```

A variável global `line` mantém o número da linha atual durante a análise.

---

## 9. Resolução de Ambiguidades (Dangling Else)

### 9.1. O Problema

A construção `if ... if ... else` é ambígua:

```c
if (a > 0)
    if (b > 0)
        printf("positivos");
    else
        printf("a negativo");  // Ambíguo: qual if?
```

### 9.2. Solução: Gramática Estrita

O parser utiliza instruções **matched** e **unmatched** para resolver a ambiguidade:

- **Matched statement:** Todo `if` está associado a um `else`
- **Unmatched statement:** `if` sem `else` correspondente

```bnf
matched_statement
    : IF LPAREN expression RPAREN matched_statement ELSE matched_statement
    ;

unmatched_statement
    : IF LPAREN expression RPAREN statement
    | IF LPAREN expression RPAREN matched_statement ELSE unmatched_statement
    ;
```

**Resultado:** O `else` sempre se associa ao `if` mais próximo que ainda não tem `else`.

---

## 10. Recuperação de Erros

### 10.1. Mecanismo de Sincronização

O Bison implementa recuperação de erros através de tokens especiais e sincronização:

1. **Detecção:** Quando um erro é detectado
2. **Descarte:** Tokens são descartados até um ponto de recuperação
3. **Sincronização:** O parser tenta continuar com a próxima construção válida

### 10.2. Pontos de Sincronização

Pontos típicos onde o parser pode se recuperar:

- Final de instrução (`;`)
- Final de bloco (`}`)
- Início de nova declaração

---

## 11. Integração com o Lexer

### 11.1. Interface Lexer-Parser

O parser chama o lexer através da função `yylex()`:

```c
int yylex(void);  /* Declaração do lexer */
extern int line;  /* Variável compartilhada de linha */
```

### 11.2. Loop Principal

```
Parser
  │
  ├─► Solicita token ao lexer (yylex)
  │   └─► Lexer processa caracteres
  │       └─► Retorna token
  │
  ├─► Processa token
  │   ├─► Verifica regra gramatical
  │   ├─► Executa ação semântica
  │   └─► Atualiza estado do parser
  │
  └─► Repete até EOF
```

---

## 12. Limitações e Restrições

### 12.1. Construções Não Suportadas

- **Declarações aninhadas de funções:** Funções dentro de blocos
- **Tipos compostos:** Structs, unions, enums
- **Ponteiros:** Sem suporte a operadores `*` ou `&`
- **Arrays:** Sem suporte a `[]`
- **Funções variádicas:** Sem `...`

### 12.2. Restrições Gramaticais

- **Main obrigatória:** Toda programa deve ter função `main`
- **Tipo int para main:** A função `main` deve retornar `int`
- **Sem recursão:** Não há suporte a chamadas recursivas (restrição semântica)

---

## 13. Algoritmo LALR(1)

### 13.1. O que é LALR(1)?

**LALR(1)** = **L**ook-**A**head **LR(1)**, onde **LR** significa **L**eft-to-right scan, **R**ightmost derivation in reverse

- **Left-to-right scan:** O parser lê os tokens da esquerda para a direita
- **Rightmost derivation in reverse:** O parser constrói a derivação mais à direita em ordem reversa
- **(1):** Usa apenas 1 token de lookahead para decisões

### 13.2. Tabelas de Parsing

O Bison gera tabelas de ação e goto:

- **Tabela de Ação:** Define o que fazer com cada token (shift, reduce, accept, error)
- **Tabela Goto:** Define para qual estado ir após redução

### 13.3. Stack de Análise

```
Stack de Símbolos      Estados          Ações
─────────────────────────────────────────────────
$                      0                shift
$ INT                  0, 1             shift
$ INT IDENTIFIER       0, 1, 2          reduce
$ type_specifier       0, 3             shift
...
```

---

## 14. Otimizações e Considerações

### 14.1. Redução de Conflitos

O Bison tenta minimizar conflitos shift/reduce e reduce/reduce. Qualquer conflito é reportado durante a compilação do parser.

### 14.2. Precedência de Operadores

Embora a precedência seja especificada principalmente na gramática, o Bison permite declarar precedência explicitamente.

### 14.3. Associatividade

- **Left-associative:** `a + b + c` = `(a + b) + c`
- **Right-associative:** `a = b = c` = `a = (b = c)` (suportado pela gramática atual do parser)

---

## 15. Conclusão

O analisador sintático para C-- Strict implementa uma gramática formal bem-definida que permite a análise precisa e eficiente de programas na linguagem. Através da técnica LALR(1) do Bison, o parser constrói uma representação em árvore que serve de base para as fases posteriores da compilação: análise semântica, geração de código intermediário e geração de código final.

A resolução cuidadosa de ambiguidades (como o *dangling else*) e o tratamento robusto de erros garantem que o compilador forneça mensagens úteis aos programadores durante o desenvolvimento.
