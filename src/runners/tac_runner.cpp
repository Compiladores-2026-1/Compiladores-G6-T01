#include "runners.hpp"
#include <iostream>

extern int yyparse();
extern ProgramNode *root_ast;

int run_tac_mode(bool debugMode)
{
    // 1. Roda o parser para construir a AST na variável root_ast
    int parse_result = yyparse();

    if (parse_result == 0)
    {
        if (root_ast != NULL)
        {
            // Executa a analise semantica antes do TAC
            SemanticVisitor semantic_checker(debugMode);
            root_ast->accept(&semantic_checker);
            if (debugMode)
            {
                semantic_checker.printSymbolTable();
            }

            // 2. Instancia o Visitor do TAC
            TACVisitor tac_visitor;

            // 3. Percorre a AST gerando as instruções TAC
            root_ast->accept(&tac_visitor);

            // 4. Imprime o código gerado no terminal
            tac_visitor.printTAC();
        }
        else
        {
            std::cout << "AST esta vazia. Nenhum código TAC gerado.\n";
        }
        return 0;
    }

    std::cerr << "Falha na analise sintática. TAC nao gerado.\n";
    return 1;
}
