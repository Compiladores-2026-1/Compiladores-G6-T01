#include "runners.hpp"
#include <iostream>

// Declaração do parser gerado pelo Bison
extern int yyparse();

int run_parser_mode()
{
    // yyparse() retorna 0 se o parsing for concluído com sucesso
    int parse_result = yyparse();

    if (parse_result == 0)
    {
        std::cout << "\nAnálise sintática concluída com sucesso.\n";
        return 0;
    }

    // yyerror já costuma imprimir detalhes do erro, aqui apenas confirmamos a falha
    std::cerr << "\nFalha na analise sintática.\n";
    return 1;
}
