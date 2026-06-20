#include "runners.hpp"

#include <iostream>
#include <vector>

extern int yyparse();
extern ProgramNode *root_ast;

int run_opt_mode(bool debugMode)
{
    // 1. Roda o parser para construir a AST na variável root_ast
    int parse_result = yyparse();

    if (parse_result == 0)
    {
        if (root_ast != NULL)
        {
            // Executa a analise semantica antes do TAC/OPT
            SemanticVisitor semantic_checker(debugMode);
            root_ast->accept(&semantic_checker);
            if (debugMode)
            {
                semantic_checker.printSymbolTable();
            }

            // 2. Instancia o Visitor do TAC e gera as instruções a partir da AST
            TACVisitor tac_visitor;
            root_ast->accept(&tac_visitor);

            // 3. Imprime o TAC original para comparação
            std::cout << "\n==================== TAC ORIGINAL ==================\n";
            tac_visitor.printTAC();

            // 4. Instancia o Otimizador e otimiza a lista de instruções
            Optimizer optimizer;
            std::vector<TACInstruction> opt_insts = optimizer.optimize(tac_visitor.getInstructions());

            // 5. Atualiza o TACVisitor com as instruções otimizadas
            tac_visitor.setInstructions(opt_insts);

            // 6. Imprime o código TAC já otimizado
            std::cout << "\n=================== TAC OTIMIZADO ==================\n";
            tac_visitor.printTAC();
        }
        else
        {
            std::cout << "AST esta vazia. Nenhuma otimização gerada.\n";
        }
        return 0;
    }

    std::cerr << "Falha na analise sintática. Otimização não gerada.\n";
    return 1;
}
