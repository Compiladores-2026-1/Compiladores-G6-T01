#include "runners.hpp"
#include <iostream>
#include <vector>

extern int yyparse();
extern ProgramNode *root_ast;

int run_codegen_mode(bool debugMode)
{
    int parse_result = yyparse();

    if (parse_result == 0)
    {
        if (root_ast != NULL)
        {
            // 1. Gera o TAC
            TACVisitor tac_visitor;
            root_ast->accept(&tac_visitor);

            // 2. Otimiza o TAC
            Optimizer optimizer;
            std::vector<TACInstruction> opt_insts = optimizer.optimize(tac_visitor.getInstructions());

            // 3. Gera o código Assembly a partir do TAC otimizado
            std::cout << "# ================= GERADO POR COMPILADOR ===============\n";
            CodeGen x86_generator;
            x86_generator.generate(opt_insts);
        }
        else
        {
            std::cout << "AST esta vazia. Nenhum código Assembly gerado.\n";
        }
        return 0;
    }

    std::cerr << "Falha na analise sintatica. Assembly não gerado.\n";
    return 1;
}
