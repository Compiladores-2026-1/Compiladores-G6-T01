#include "runners.hpp"
#include <iostream>

extern int yyparse();
extern ProgramNode *root_ast;

int run_semantic_mode(bool debugMode)
{
    int parse_result = yyparse();
    if (parse_result == 0)
    {
        std::cout << "Analise sintática concluída com sucesso.\n";
        std::cout << "============================================================\n";
        std::cout << "Análise Semântica:\n";
        if (root_ast != NULL)
        {
            SemanticVisitor semantic_checker(debugMode);
            root_ast->accept(&semantic_checker);

            if (debugMode)
            {
                semantic_checker.printSymbolTable();
            }
        }
        else
        {
            std::cout << "AST esta vazia.\n";
        }
        std::cout << "========================================================\n";
        return 0;
    }

    std::cerr << "Falha na analise sintática.\n";
    return 1;
}
