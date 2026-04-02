#include <gtest/gtest.h>

#include <cstdlib> // free()

extern "C"
{
// Este include entra dentro de extern "C" porque tokens.h foi pensado para C.
// Assim, tudo o que vier desse header (tipos globais e simbolos externos)
// mantem convencao de nomes C, compativel com o codigo gerado por Flex/Bison.
#include "lexer/tokens.h"

    // YY_BUFFER_STATE e um "handle" para o buffer interno do lexer.
    // "Tipo opaco" significa que o teste usa apenas um ponteiro para esse tipo,
    // sem depender da estrutura interna (encapsulamento).
    // Isso deixa o teste estavel mesmo se a implementacao interna do Flex mudar.
    typedef struct yy_buffer_state *YY_BUFFER_STATE;

    // Le o proximo token da entrada atual e retorna um codigo inteiro
    // (ex.: INT, IDENTIFIER, BOOL_LITERAL...).
    int yylex(void);

    // Libera estruturas globais/alocacoes internas do scanner.
    // Chamar ao fim do teste evita vazamento e interferencia entre casos.
    int yylex_destroy(void);

    // Cria um buffer de leitura a partir de uma string em memoria.
    // E isso que permite testar sem arquivo e sem stdin.
    YY_BUFFER_STATE yy_scan_string(const char *str);

    // Destroi o buffer criado por yy_scan_string.
    // Deve ser chamado antes de finalizar para manter ciclo de vida correto.
    void yy_delete_buffer(YY_BUFFER_STATE buffer);
}

TEST(LexerTest, ReconheceSequenciaBasica)
{
    // Injeta uma string como entrada do lexer (sem precisar de arquivo).
    YY_BUFFER_STATE buffer = yy_scan_string("int x = 10;");
    ASSERT_NE(buffer, nullptr);

    // 1) "int" deve ser reconhecido como palavra-chave de tipo.
    EXPECT_EQ(yylex(), INT);

    // 2) "x" deve ser reconhecido como identificador.
    EXPECT_EQ(yylex(), IDENTIFIER);
    ASSERT_NE(yylval.strval, nullptr);
    EXPECT_STREQ(yylval.strval, "x");
    // O lexer aloca memoria para identificadores; liberamos apos conferir.
    free(yylval.strval);

    // 3) "=" deve virar token de atribuicao.
    EXPECT_EQ(yylex(), ASSIGN);

    // 4) "10" deve virar literal inteiro com valor numerico 10.
    EXPECT_EQ(yylex(), INTEGER_LITERAL);
    EXPECT_EQ(yylval.intval, 10);

    // 5) ";" encerra o comando.
    EXPECT_EQ(yylex(), SEMICOLON);
    // 6) Depois dos tokens, o lexer retorna 0 (fim da entrada).
    EXPECT_EQ(yylex(), 0);

    // Limpeza do estado interno criado para este teste.
    yy_delete_buffer(buffer);
    yylex_destroy();
}

TEST(LexerTest, ReconheceLiteraisBool)
{
    // Entrada com dois literais booleanos em sequencia.
    YY_BUFFER_STATE buffer = yy_scan_string("true false");
    ASSERT_NE(buffer, nullptr);

    // "true" deve produzir BOOL_LITERAL com valor 1.
    EXPECT_EQ(yylex(), BOOL_LITERAL);
    EXPECT_EQ(yylval.intval, 1);

    // "false" deve produzir BOOL_LITERAL com valor 0.
    EXPECT_EQ(yylex(), BOOL_LITERAL);
    EXPECT_EQ(yylval.intval, 0);

    // Sem mais tokens disponiveis.
    EXPECT_EQ(yylex(), 0);

    // Limpeza para nao vazar recursos entre testes.
    yy_delete_buffer(buffer);
    yylex_destroy();
}
