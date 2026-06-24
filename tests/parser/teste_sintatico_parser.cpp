#include <gtest/gtest.h>

#include <cstdio>

#include "tokens.hpp"

int yyparse(void);
int yylex_destroy(void);
extern FILE *yyin;
extern int line;

// Teste happy-path: o parser deve aceitar um programa C-- Strict valido e abrangente,
// retornando 0 (sucesso). O arquivo de entrada cobre funcoes, controle de fluxo,
// expressoes aninhadas e operadores compostos.
TEST(ParserTest, ArquivoAbrangente)
{
    // 1. Abre o arquivo .cmm (caminho relativo a raiz do projeto; ver WORKING_DIRECTORY no CMake)
    FILE *fp = fopen("tests/parser/teste_parser.cmm", "r");
    ASSERT_NE(fp, nullptr) << "Nao foi possivel abrir o arquivo de teste!";

    // 2. Prepara o ambiente do lexer/parser
    yyin = fp;
    line = 1;

    // 3. Roda o parser
    int result = yyparse();

    // 4. Limpa o estado
    fclose(fp);
    yylex_destroy();

    // 5. Verifica que o parsing terminou sem erros sintaticos
    EXPECT_EQ(result, 0);
}
