#include <stdio.h>
#include <stdlib.h>

/* Declaração da função do lexer gerada pelo Flex */
extern int yylex(void);
/* Variável global do Flex para o texto do token atual */
extern char *yytext;
/* Declaração de yylval */

int main(void)
{
    int token;

    printf("Digite a entrada para testar o lexer (pressione Ctrl+D para sair):\n");

    /* Loop para processar tokens até o fim da entrada (EOF) */
    while ((token = yylex()) != 0)
    { /* yylex() retorna 0 no EOF */
        printf("Token: %d, Texto: '%s'\n", token, yytext);
    }

    printf("Fim da análise léxica.\n");
    return 0;
}
