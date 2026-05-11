#pragma once
#include "tac.hpp"
#include <vector>
#include <string>
#include <map>

class CodeGen
{
public:
    // Recebe o TAC Otimizado e imprime o código Assembly x86-64
    void generate(const std::vector<TACInstruction> &instructions);

private:
    std::map<std::string, int> stack_offsets; // Mapeia "t1", "x" para um offset como -4, -8
    int current_stack_size = 0;
    std::vector<std::string> param_registers = {"%edi", "%esi", "%edx", "%ecx", "%r8d", "%r9d"};
    int param_counter = 0;

    bool isNumber(const std::string &str);

    // Retorna a representação em Assembly de um argumento (ex: "$5", "-4(%rbp)", "global_var(%rip)")
    std::string getOperand(const std::string &arg);

    // Varre a função atual para reservar espaço na stack para as variáveis locais
    void computeLocalVariables(const std::vector<TACInstruction> &instructions, size_t start_idx);
};
