#pragma once
#include "tac.hpp"
#include <vector>
#include <string>
#include <map>
#include <set>

class Optimizer
{
public:
    // Aplica todas as otimizações configuradas num ciclo até estabilizar
    std::vector<TACInstruction> optimize(const std::vector<TACInstruction> &instructions);

private:
    // Otimização 1: Dobragem de Constantes (Constant Folding)
    std::vector<TACInstruction> constantFolding(const std::vector<TACInstruction> &instructions);

    // Otimização 2: Janela (Peephole Optimization)
    std::vector<TACInstruction> peepholeOptimization(const std::vector<TACInstruction> &instructions);

    // Otimização 3: Propagação de Cópia e Constantes (Copy Propagation)
    std::vector<TACInstruction> copyPropagation(const std::vector<TACInstruction> &instructions);

    // Otimização 4: Eliminação de Código Morto (Dead Code Elimination)
    std::vector<TACInstruction> deadCodeElimination(const std::vector<TACInstruction> &instructions);

    // Função auxiliar
    bool isNumber(const std::string &str);
};
