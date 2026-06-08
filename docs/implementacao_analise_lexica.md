# Implementação da Análise Léxica

**Arquivo:** `src/lexer/lexer.l`  
**Ferramenta:** Flex (Fast Lexical Analyzer Generator)  
**Saída gerada:** `lexer.cpp` (gerado automaticamente pelo CMake via `FLEX_TARGET`)

---

## 1. Visão Geral

O analisador léxico é implementado com **Flex**, que lê o arquivo de especificação `lexer.l` e gera automaticamente o arquivo `lexer.cpp` contendo a função `yylex()`. Essa função é chamada repetidamente pelo parser (Bison) para consumir o próximo token do fluxo de entrada.

---

## 2. Estrutura do Arquivo `lexer.l`

O arquivo `.l` é dividido em três seções separadas por `%%`:

```
[Seção de Definições]
%%
[Regras]
%%
[Código C++ auxiliar]
```

### 2.1. Seção de Definições

Inclui os cabeçalhos necessários e declara macros de padrões reutilizáveis:

```flex
%option noyywrap

%{
#include "tokens.hpp"
int line = 1;
%}

DIGIT            [0-9]
LETTER           [a-zA-Z_]
IDENTIFIER       {LETTER}({LETTER}|{DIGIT})*
INTEGER_LITERAL  {DIGIT}+
EXPONENT         [eE][+-]?{DIGIT}+
FRACTION         ({DIGIT}+"."{DIGIT}*|"."{DIGIT}+)
DOUBLE_LITERAL   ({FRACTION}({EXPONENT})?|{DIGIT}+{EXPONENT})
FLOAT_LITERAL    ({DOUBLE_LITERAL}[fF])
CHAR_LITERAL     \'([^\\\']|\\.)*\'
WHITESPACE       [ \t\r]+
NEWLINE          \n
COMMENT          "//".*
COMMENT_BLOCK    "/*"([^*]|(\*+[^*/]))*\*+"/"
```

A opção `%option noyywrap` dispensa a implementação da função `yywrap()`, indicando ao Flex que não há múltiplos arquivos de entrada.

A variável global `line` rastreia o número da linha atual e é usada pelo parser para mensagens de erro.

### 2.2. Seção de Regras

Cada regra tem a forma `padrão { ação }`. As regras são avaliadas na ordem em que aparecem e o Flex aplica a regra com o **casamento mais longo** (*maximal munch*).

**Prioridade para palavras-chave:** regras de strings literais (ex: `"int"`) têm precedência sobre identificadores quando o casamento tem o mesmo comprimento, porque aparecem primeiro no arquivo.

#### Espaços, comentários e nova linha

```flex
{WHITESPACE}    { /* ignora */ }
{NEWLINE}       { line++; }
{COMMENT}       { /* ignora */ }
{COMMENT_BLOCK} { /* ignora */ }
```

#### Palavras-chave

Retornam o token correspondente sem nenhum valor semântico:

```flex
"int"    { return INT;    }
"float"  { return FLOAT;  }
"if"     { return IF;     }
"while"  { return WHILE;  }
// ... demais palavras-chave
```

#### Literais booleanos

Armazenam `1` ou `0` em `yylval.intval`:

```flex
"true"  { yylval.intval = 1; return BOOL_LITERAL; }
"false" { yylval.intval = 0; return BOOL_LITERAL; }
```

#### Literais numéricos

```flex
{INTEGER_LITERAL} { yylval.intval    = atoi(yytext);         return INTEGER_LITERAL; }
{FLOAT_LITERAL}   { yylval.floatval  = strtof(yytext, NULL); return FLOAT_LITERAL;   }
{DOUBLE_LITERAL}  { yylval.doubleval = strtod(yytext, NULL); return DOUBLE_LITERAL;  }
```

#### Literal de caractere

O lexer converte o literal para seu valor ASCII inteiro e retorna `INTEGER_LITERAL`. Sequências de escape são tratadas manualmente:

| Escape | Valor |
|--------|-------|
| `\n`   | 10    |
| `\t`   | 9     |
| `\r`   | 13    |
| `\0`   | 0     |
| outros | valor do próprio caractere |

```flex
{CHAR_LITERAL} {
    int ascii_val = (yytext[1] == '\\')
        ? /* trata escape */
        : (int)yytext[1];
    yylval.intval = ascii_val;
    return INTEGER_LITERAL;
}
```

#### Identificadores

Copia o texto reconhecido para `yylval.strval` usando `strdup` (o parser é responsável por liberar a memória):

```flex
{IDENTIFIER} {
    yylval.strval = strdup(yytext);
    return IDENTIFIER;
}
```

#### Erro léxico

Qualquer caractere não reconhecido cai na regra padrão `.`:

```flex
. {
    fprintf(stderr, "Erro léxico: '%s' na linha %d\n", yytext, line);
    return UNKNOWN_TOKEN;
}
```

---

## 3. Integração com o Bison

O arquivo `tokens.hpp` é um wrapper que inclui `parser.hpp` (gerado pelo Bison). Isso garante que todos os `#define` de tokens (`INT`, `FLOAT`, etc.) e a definição de `YYSTYPE` (a `%union` do Bison) estejam disponíveis no código do lexer.

```cpp
// include/tokens.hpp
#pragma once
#include "parser.hpp"
```

Sem esse wrapper, o lexer não teria como nomear os tokens retornados por `yylex()`.

---

## 4. Integração com o CMake

O CMake localiza o Flex e gera automaticamente `lexer.cpp` via a macro `FLEX_TARGET`:

```cmake
find_package(FLEX REQUIRED)
FLEX_TARGET(Lexer lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)
```

O arquivo gerado é compilado como uma biblioteca estática (`lexer_lib`) e linkado ao executável principal.

---

## 5. Runner do Modo `--lexer`

O arquivo `src/runners/lexer_runner.cpp` implementa a função `run_lexer_mode()`, ativada pela flag `--lexer`. Ela chama `yylex()` em loop e imprime o nome de cada token reconhecido:

```cpp
int run_lexer_mode() {
    int token;
    while ((token = yylex()) != 0) {
        std::cout << token_to_string(token) << " ";
    }
    std::cout << "\n\nFim da analise léxica.\n";
    return 0;
}
```

A função auxiliar `token_to_string(int token)` converte o código numérico do token para seu nome textual (`INT`, `IDENTIFIER`, `PLUS`, etc.) usando um `switch`.

---

## 6. Exemplo de Saída

Entrada:
```c
int x = 42;
```

Saída do modo `--lexer`:
```
INT IDENTIFIER ASSIGN INTEGER_LITERAL SEMICOLON

Fim da analise léxica.
```
