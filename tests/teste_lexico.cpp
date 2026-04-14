#include <gtest/gtest.h>

#include <cstdlib> 

extern "C"
{
#include "lexer/tokens.h"

    typedef struct yy_buffer_state *YY_BUFFER_STATE;
    int yylex(void);
    int yylex_destroy(void);
    YY_BUFFER_STATE yy_scan_string(const char *str);
    void yy_delete_buffer(YY_BUFFER_STATE buffer);
    extern char *yytext; // Importação da string lida atual para poder inspecioná-la nos testes
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
    // O lexer aloca memoria para identificadores;
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

TEST(LexerTest, IdentificaTokensComplexos) {
    auto buffer = yy_scan_string("int x = 10; x++; x += 2;");
    
    // Valida a sequência de tokens
    EXPECT_EQ(yylex(), INT);
    EXPECT_EQ(yylex(), IDENTIFIER);
    EXPECT_EQ(yylex(), ASSIGN);
    EXPECT_EQ(yylex(), INTEGER_LITERAL);
    EXPECT_EQ(yylex(), SEMICOLON);

    // Valida operadores 
    EXPECT_EQ(yylex(), IDENTIFIER);
    EXPECT_EQ(yylex(), INC); // Para o '++'
    EXPECT_EQ(yylex(), SEMICOLON);

    yy_delete_buffer(buffer);
    yylex_destroy();
}

TEST(LexerTest, ValidacaoCompletaCStrict) {
    auto buffer = yy_scan_string("int x = 10; x++; x += 5; true;");
    ASSERT_NE(buffer, nullptr);

    // Teste: int x = 10;
    EXPECT_EQ(yylex(), INT);
    EXPECT_EQ(yylex(), IDENTIFIER);
    EXPECT_EQ(yylex(), ASSIGN);
    EXPECT_EQ(yylex(), INTEGER_LITERAL);
    EXPECT_EQ(yylex(), SEMICOLON);

    // Teste: x++; 
    EXPECT_EQ(yylex(), IDENTIFIER);
    EXPECT_EQ(yylex(), INC); 
    EXPECT_EQ(yylex(), SEMICOLON);

    // Teste: x += 5; 
    EXPECT_EQ(yylex(), IDENTIFIER);
    EXPECT_EQ(yylex(), PLUSEQ);
    EXPECT_EQ(yylex(), INTEGER_LITERAL);
    EXPECT_EQ(yylex(), SEMICOLON);

    // Teste: Booleano
    EXPECT_EQ(yylex(), BOOL_LITERAL);
    EXPECT_EQ(yylval.intval, 1); 

    yy_delete_buffer(buffer);
    yylex_destroy();
}

// Teste de Operadores de Atribuição e Aritmética
TEST(LexerTest, AtribuicoesEArimetica) {
    auto buffer = yy_scan_string("x -= 3; y *= 2; z /= 4;");
    
    // x -= 3;
    EXPECT_EQ(yylex(), IDENTIFIER);
    EXPECT_EQ(yylex(), MINUSEQ);
    EXPECT_EQ(yylex(), INTEGER_LITERAL);
    EXPECT_EQ(yylex(), SEMICOLON);

    // y *= 2;
    EXPECT_EQ(yylex(), IDENTIFIER);
    EXPECT_EQ(yylex(), MULTEQ);
    EXPECT_EQ(yylex(), INTEGER_LITERAL);
    EXPECT_EQ(yylex(), SEMICOLON);

    yy_delete_buffer(buffer);
    yylex_destroy();
}

// Teste de Estruturas de Controle e Comparação
TEST(LexerTest, FluxoEComparacao) {
    auto buffer = yy_scan_string("if (a >= b) { return true; }");
    
    EXPECT_EQ(yylex(), IF);
    EXPECT_EQ(yylex(), LPAREN);
    EXPECT_EQ(yylex(), IDENTIFIER);
    EXPECT_EQ(yylex(), GE); // Greater or Equal
    EXPECT_EQ(yylex(), IDENTIFIER);
    EXPECT_EQ(yylex(), RPAREN);
    EXPECT_EQ(yylex(), LBRACE);
    EXPECT_EQ(yylex(), RETURN);
    EXPECT_EQ(yylex(), BOOL_LITERAL);
    EXPECT_EQ(yylex(), SEMICOLON);
    EXPECT_EQ(yylex(), RBRACE);

    yy_delete_buffer(buffer);
    yylex_destroy();
}

// Teste de Comentários e Espaços (Garantir que são ignorados)
TEST(LexerTest, IgnoraComentarios) {
    auto buffer = yy_scan_string("int x; // isso eh um comentario\n x = 5; /* bloco */");
    
    EXPECT_EQ(yylex(), INT);
    EXPECT_EQ(yylex(), IDENTIFIER);
    EXPECT_EQ(yylex(), SEMICOLON);
    
    // O lexer deve pular o comentário e ir direto para o 'x'
    EXPECT_EQ(yylex(), IDENTIFIER);
    EXPECT_EQ(yylex(), ASSIGN);
    EXPECT_EQ(yylex(), INTEGER_LITERAL);
    EXPECT_EQ(yylex(), SEMICOLON);

    yy_delete_buffer(buffer);
    yylex_destroy();
}

// Teste de Tokens Incorretos (Sad Path)
TEST(LexerTest, IdentificaTokensIncorretos) {
    // Injetamos dois símbolos não suportados pela linguagem C-- Strict: @ e $
    auto buffer = yy_scan_string("int @ $ x;");
    
    EXPECT_EQ(yylex(), INT);
    
    // O Lexer deve avisar o teste sobre a "sujeira" e apontar o caractere exato
    EXPECT_EQ(yylex(), UNKNOWN_TOKEN); 
    EXPECT_STREQ(yytext, "@");

    EXPECT_EQ(yylex(), UNKNOWN_TOKEN);
    EXPECT_STREQ(yytext, "$");
    
    // E deve conseguir continuar analisando o que é válido em seguida
    EXPECT_EQ(yylex(), IDENTIFIER); // Pega o 'x'
    EXPECT_EQ(yylex(), SEMICOLON); // Pega o ';'

    yy_delete_buffer(buffer);
    yylex_destroy();
}

// Teste de Borda: Token Incorreto Colado ao Fim do Arquivo (EOF)
TEST(LexerTest, TokenIncorretoNoFinal) {
    auto buffer = yy_scan_string("int x @");
    
    EXPECT_EQ(yylex(), INT);
    EXPECT_EQ(yylex(), IDENTIFIER);
    
    // Testa se o compilador sobrevive sem Segmentation Fault ao esbarrar num erro no apagar das luzes
    EXPECT_EQ(yylex(), UNKNOWN_TOKEN);
    EXPECT_STREQ(yytext, "@");
    
    // Lexer deve encerrar com segurança retornando 0
    EXPECT_EQ(yylex(), 0);

    yy_delete_buffer(buffer);
    yylex_destroy();
}