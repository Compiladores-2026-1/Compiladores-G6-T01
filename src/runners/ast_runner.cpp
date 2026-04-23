#include "runners.hpp"
#include <iostream>

extern int yyparse();
extern ProgramNode *root_ast;

int run_ast_mode()
{
    int parse_result = yyparse();
    if (parse_result == 0)
    {
        std::cout << "Analise sintática concluída com sucesso.\n";
        std::cout << "============================================================\n";
        std::cout << "Árvore de Sintaxe Abstrata (AST):\n";
        if (root_ast != NULL)
        {
            imprimirAST(root_ast); // Imprime a árvore gerada
        }
        else
        {
            std::cout << "AST esta vazia.\n";
        }
        std::cout << "============================================================\n";
        return 0;
    }

    std::cerr << "Falha na analise sintática.\n";
    return 1;
}
