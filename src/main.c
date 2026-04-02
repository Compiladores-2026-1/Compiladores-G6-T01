#include <stdio.h>
#include <stdlib.h>

/* Declaração da função do lexer gerada pelo Flex */
extern int yylex(void);
/* Variável global do Flex para o texto do token atual */
extern char *yytext;
/* Fluxo de entrada usado pelo Flex */
extern FILE *yyin;
/* Declaração de yylval */

int main(int argc, char **argv)
{
    int token;
    FILE *input = stdin;

    if (argc > 2)
    {
        fprintf(stderr, "Uso: %s [arquivo_entrada]\n", argv[0]);
        return 1;
    }

    if (argc == 2)
    {
        input = fopen(argv[1], "r");
        if (input == NULL)
        {
            perror("Erro ao abrir arquivo de entrada");
            return 1;
        }
    }

    yyin = input;

    if (argc == 1)
    {
        printf("Digite a entrada para testar o lexer (pressione Ctrl+D para sair):\n");
    }

    /* Loop para processar tokens até o fim da entrada (EOF) */
    while ((token = yylex()) != 0)
    { /* yylex() retorna 0 no EOF */
        printf("Token: %d, Texto: %s\n", token, yytext);
    }

    if (input != stdin)
    {
        fclose(input);
    }

    printf("Fim da análise léxica.\n");
    return 0;
}
