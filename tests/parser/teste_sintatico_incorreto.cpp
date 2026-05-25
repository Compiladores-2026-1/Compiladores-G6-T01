#include <gtest/gtest.h>

#include "tokens.hpp"

typedef struct yy_buffer_state *YY_BUFFER_STATE;
int yyparse(void);
int yylex_destroy(void);
YY_BUFFER_STATE yy_scan_string(const char *str);
void yy_delete_buffer(YY_BUFFER_STATE buffer);
extern int line;

namespace {

void resetar_estado_lexer()
{
    line = 1;
}

void encerrar_buffer(YY_BUFFER_STATE buffer)
{
    yy_delete_buffer(buffer);
    yylex_destroy();
}

} // namespace

TEST(ParserSadPath, FaltaPontoEVirgula)
{
    auto buffer = yy_scan_string("int main() { int x = 10 }");
    resetar_estado_lexer();
    EXPECT_NE(yyparse(), 0);
    encerrar_buffer(buffer);
}

TEST(ParserSadPath, ParenteseNaoFechado)
{
    auto buffer = yy_scan_string("int main() { if (a > b { } }");
    resetar_estado_lexer();
    EXPECT_NE(yyparse(), 0);
    encerrar_buffer(buffer);
}

TEST(ParserSadPath, ChaveNaoFechada)
{
    auto buffer = yy_scan_string("int main() { int x = 1;");
    resetar_estado_lexer();
    EXPECT_NE(yyparse(), 0);
    encerrar_buffer(buffer);
}

TEST(ParserSadPath, ElseSemIf)
{
    auto buffer = yy_scan_string("int main() { else { } }");
    resetar_estado_lexer();
    EXPECT_NE(yyparse(), 0);
    encerrar_buffer(buffer);
}

TEST(ParserSadPath, OperadorBinarioSemOperando)
{
    auto buffer = yy_scan_string("int main() { int x = 1 + ; }");
    resetar_estado_lexer();
    EXPECT_NE(yyparse(), 0);
    encerrar_buffer(buffer);
}

TEST(ParserSadPath, VirgulaExtraEmChamada)
{
    auto buffer = yy_scan_string("int main() { f(a,,b); }");
    resetar_estado_lexer();
    EXPECT_NE(yyparse(), 0);
    encerrar_buffer(buffer);
}

TEST(ParserSadPath, DeclaracaoSemTipo)
{
    auto buffer = yy_scan_string("x = 5;");
    resetar_estado_lexer();
    EXPECT_NE(yyparse(), 0);
    encerrar_buffer(buffer);
}

TEST(ParserSadPath, ForCabecalhoIncompleto)
{
    auto buffer = yy_scan_string("int main() { for(;;");
    resetar_estado_lexer();
    EXPECT_NE(yyparse(), 0);
    encerrar_buffer(buffer);
}

TEST(ParserSadPath, ProgramaSemMain)
{
    auto buffer = yy_scan_string("int soma(int a) { return a; }");
    resetar_estado_lexer();
    EXPECT_NE(yyparse(), 0);
    encerrar_buffer(buffer);
}

TEST(ParserSadPath, IfSemCondicao)
{
    auto buffer = yy_scan_string("int main() { if () { } }");
    resetar_estado_lexer();
    EXPECT_NE(yyparse(), 0);
    encerrar_buffer(buffer);
}
