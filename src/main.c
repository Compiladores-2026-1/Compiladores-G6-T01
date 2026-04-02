#include <stdio.h>

int yyparse(void);

int main(void)
{
    printf("Aperte Ctrl + D para sair.\n");
    int parse_result = yyparse();

    if (parse_result == 0)
    {
        puts("Analise concluida com sucesso.");
        return 0;
    }

    puts("Falha na analise.");
    return 1;
}
