# Testes de Análise Sintática

**Projeto:** Compilador C-- Strict
**Fase:** Análise Sintática (Parser)
**Ferramentas:** Bison, Flex, GoogleTest

---

## 1. Objetivo

O analisador sintático é validado em dois eixos complementares:

* **Validação positiva (happy path):** confirma que programas C-- Strict bem-formados são aceitos pela gramática e que `yyparse()` retorna `0`.
* **Validação negativa (sad path):** confirma que entradas com erros sintáticos são rejeitadas, `yyparse()` retorna valor diferente de `0` e o compilador não trava.

## 2. Testes de Validação Negativa (Sad Path)

### 2.1 Abordagem

Os testes são escritos em GoogleTest e usam `yy_scan_string` para injetar a entrada direto em memória, dispensando arquivos em disco. Cada cenário fica isolado em seu próprio `TEST`, com reset de estado do lexer antes da execução e limpeza de buffer ao final.

Modelo de cada teste:

```cpp
TEST(ParserSadPath, NomeDoCenario) {
    auto buffer = yy_scan_string("<entrada inválida>");
    line = 1;
    EXPECT_NE(yyparse(), 0);
    yy_delete_buffer(buffer);
    yylex_destroy();
}
```

Arquivo: `tests/parser/teste_sintatico_incorreto.cpp`.

### 2.2 Cenários cobertos

| Teste | Entrada | Regra gramatical exercitada |
| :--- | :--- | :--- |
| `FaltaPontoEVirgula` | `int main() { int x = 10 }` | Terminador `SEMICOLON` obrigatório no fim de comando |
| `ParenteseNaoFechado` | `int main() { if (a > b { } }` | Pareamento de `LPAREN`/`RPAREN` em condição de `if` |
| `ChaveNaoFechada` | `int main() { int x = 1;` | Pareamento de `LBRACE`/`RBRACE` em bloco |
| `ElseSemIf` | `int main() { else { } }` | `ELSE` só é válido vinculado a um `IF` |
| `OperadorBinarioSemOperando` | `int main() { int x = 1 + ; }` | `additive_expression` exige operando à direita |
| `VirgulaExtraEmChamada` | `int main() { f(a,,b); }` | `argument_list` não admite argumento vazio |
| `DeclaracaoSemTipo` | `x = 5;` | Declaração global exige `type_specifier` |
| `ForCabecalhoIncompleto` | `int main() { for(;;` | Cabeçalho do `for` precisa das três cláusulas |
| `ProgramaSemMain` | `int soma(int a) { return a; }` | Símbolo inicial `program` exige `main_function` |
| `IfSemCondicao` | `int main() { if () { } }` | `IF LPAREN expression RPAREN` — expressão obrigatória |

### 2.3 Limitações conhecidas

* **Vazamentos de memória em caminhos de erro.** A gramática aloca nós da AST (`new ProgramNode`, `new VarDeclNode`, `strdup` de identificadores etc.) e só os libera nas reduções bem-sucedidas. Quando `yyparse()` aborta por erro sintático, essas alocações ficam órfãs. Não afeta a validade dos testes (cada execução é curta e o SO recupera a memória), mas seria reportado por ferramentas como AddressSanitizer/Valgrind. Fica registrado como débito técnico para futura limpeza no `parser.y`.
* **Mensagens de erro no `stderr`.** Cada teste sad-path dispara `yyerror`, que imprime no `stderr`. As mensagens "Erro sintático na linha N: syntax error" durante a execução dos testes são esperadas e não indicam falha.

## 3. Testes de Validação Positiva (Happy Path)

Ver [Relatório de Testes Sintáticos](relatorio_testes_sintaticos.md).

## 4. Como executar

A partir da raiz do projeto:

```bash
cmake -S . -B build
cmake --build build --target testes
cd build && ./testes --gtest_filter=ParserSadPath.*
```

Ou via CTest para executar toda a suíte:

```bash
cd build && ctest --output-on-failure
```
