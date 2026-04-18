#include <gtest/gtest.h>
#include <cstdio>

extern "C" {
    #include "lexer/tokens.h"
    int yyparse(void);
    extern FILE *yyin;
    extern int line;
    int yylex_destroy(void);
}

// O código que você me mandou vai aqui!
TEST(ParserTest, ArquivoAbrangente) {
    // 1. Abre o arquivo .cmm
    FILE* fp = fopen("tests/parser/teste_sintatico_parser.cmm", "r");
    ASSERT_NE(fp, nullptr) << "Não foi possível abrir o arquivo de teste!";
    
    // 2. Prepara o ambiente
    yyin = fp;
    line = 1;

    // 3. Roda o Parser
    int result = yyparse();
    
    // 4. Limpa tudo
    fclose(fp);
    yylex_destroy();
    
    // 5. Verifica se passou
    EXPECT_EQ(result, 0);
}