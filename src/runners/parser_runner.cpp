#include "parser_runner.hpp"
#include <iostream>

// Declaração do parser gerado pelo Bison
extern int yyparse();

int run_parser_mode()
{
    int parse_result = yyparse();

    if (parse_result == 0)
    {
        std::cout << "Fim da analise sintática.\n";
        return 0;
    }

    std::cerr << "Falha na analise sintática.\n";
    return 1;
}
