#include "codegen.hpp"
#include <iostream>

bool CodeGen::isNumber(const std::string &str)
{
    if (str.empty())
        return false;
    return str.find_first_not_of("0123456789.-") == std::string::npos;
}

std::string CodeGen::getOperand(const std::string &arg)
{
    if (isNumber(arg))
    {
        // Truncar números com ponto flutuante para inteiros para evitar erros no GCC
        if (arg.find('.') != std::string::npos)
        {
            int int_val = static_cast<int>(std::stod(arg));
            return "$" + std::to_string(int_val);
        }
        return "$" + arg; // Literal normal (ex: $5)
    }

    // Se a variável estiver mapeada na Stack local da função
    if (stack_offsets.find(arg) != stack_offsets.end())
    {
        return std::to_string(stack_offsets[arg]) + "(%rbp)";
    }

    // Se não for número nem variável local, é uma variável global
    return arg + "(%rip)";
}

void CodeGen::computeLocalVariables(const std::vector<TACInstruction> &instructions, size_t start_idx)
{
    stack_offsets.clear();
    current_stack_size = 0;

    for (size_t i = start_idx; i < instructions.size(); i++)
    {
        if (instructions[i].op == TACOp::FUNC_END)
            break;

        auto allocateVar = [&](const std::string &var)
        {
            if (!var.empty() && !isNumber(var) && stack_offsets.find(var) == stack_offsets.end())
            {
                current_stack_size -= 4; // Int = 4 bytes
                stack_offsets[var] = current_stack_size;
            }
        };

        // Regra especial para o CALL: o retorno precisa de espaço na Stack!
        if (instructions[i].op == TACOp::CALL)
        {
            allocateVar(instructions[i].res);
        }
        else if (instructions[i].op != TACOp::PARAM && instructions[i].op != TACOp::LABEL)
        {
            allocateVar(instructions[i].res);
            allocateVar(instructions[i].arg1);
            allocateVar(instructions[i].arg2);
        }
    }

    // Alinhar a stack a 16 bytes (Requisito estrito do System V ABI para x86-64)
    int padding = (-current_stack_size) % 16;
    if (padding != 0)
    {
        current_stack_size -= (16 - padding);
    }
}

void CodeGen::generate(const std::vector<TACInstruction> &instructions)
{
    // ---------------------------------------------------------
    // 1. GERAR SEÇÃO DE DADOS (Variáveis Globais)
    // ---------------------------------------------------------
    std::cout << "  .data\n";
    size_t i = 0;

    // Tudo o que vier antes da primeira função é variável global
    while (i < instructions.size() && instructions[i].op != TACOp::FUNC_BEGIN)
    {
        if (instructions[i].op == TACOp::ASSIGN && !instructions[i].res.empty())
        {
            std::cout << "  .globl " << instructions[i].res << "\n";
            std::cout << instructions[i].res << ":\n";

            std::string val = instructions[i].arg1;
            if (isNumber(val))
            {
                if (val.find('.') != std::string::npos)
                {
                    std::cout << "  .long " << static_cast<int>(std::stod(val)) << "\n";
                }
                else
                {
                    std::cout << "  .long " << val << "\n";
                }
            }
            else
            {
                std::cout << "  .long 0\n";
            }
        }
        i++;
    }

    // ---------------------------------------------------------
    // 2. GERAR SEÇÃO DE CÓDIGO EXECUTÁVEL (.text)
    // ---------------------------------------------------------
    std::cout << "\n  .text\n";
    std::cout << "  .globl main\n\n";

    for (; i < instructions.size(); i++)
    {
        const auto &inst = instructions[i];

        switch (inst.op)
        {
        case TACOp::FUNC_BEGIN:
            std::cout << inst.res << ":\n";
            std::cout << "  pushq %rbp\n";
            std::cout << "  movq %rsp, %rbp\n";

            computeLocalVariables(instructions, i + 1);

            if (current_stack_size < 0)
            {
                std::cout << "  subq $" << -current_stack_size << ", %rsp\n";
            }
            param_counter = 0;
            break;

        case TACOp::FUNC_END:
            std::cout << "  movq %rbp, %rsp\n";
            std::cout << "  popq %rbp\n";
            std::cout << "  ret\n\n";
            break;

        case TACOp::LABEL:
            std::cout << inst.res << ":\n";
            break;

        case TACOp::ASSIGN:
            std::cout << "  movl " << getOperand(inst.arg1) << ", %eax\n";
            std::cout << "  movl %eax, " << getOperand(inst.res) << "\n";
            break;

        case TACOp::ADD:
        case TACOp::SUB:
        case TACOp::MUL:
            std::cout << "  movl " << getOperand(inst.arg1) << ", %eax\n";
            if (inst.op == TACOp::ADD)
                std::cout << "  addl " << getOperand(inst.arg2) << ", %eax\n";
            if (inst.op == TACOp::SUB)
                std::cout << "  subl " << getOperand(inst.arg2) << ", %eax\n";
            if (inst.op == TACOp::MUL)
                std::cout << "  imull " << getOperand(inst.arg2) << ", %eax\n";
            std::cout << "  movl %eax, " << getOperand(inst.res) << "\n";
            break;

        case TACOp::DIV:
        case TACOp::MOD:
            std::cout << "  movl " << getOperand(inst.arg1) << ", %eax\n";
            std::cout << "  cltd\n"; // Expande o sinal de EAX para EDX
            std::cout << "  movl " << getOperand(inst.arg2) << ", %ebx\n";
            std::cout << "  idivl %ebx\n"; // Divide EDX:EAX por EBX
            if (inst.op == TACOp::DIV)
                std::cout << "  movl %eax, " << getOperand(inst.res) << "\n";
            if (inst.op == TACOp::MOD)
                std::cout << "  movl %edx, " << getOperand(inst.res) << "\n";
            break;

        case TACOp::EQ:
        case TACOp::NEQ:
        case TACOp::LT:
        case TACOp::GT:
        case TACOp::LE:
        case TACOp::GE:
            std::cout << "  movl " << getOperand(inst.arg1) << ", %eax\n";
            std::cout << "  cmpl " << getOperand(inst.arg2) << ", %eax\n";
            if (inst.op == TACOp::EQ)
                std::cout << "  sete %al\n";
            if (inst.op == TACOp::NEQ)
                std::cout << "  setne %al\n";
            if (inst.op == TACOp::LT)
                std::cout << "  setl %al\n";
            if (inst.op == TACOp::GT)
                std::cout << "  setg %al\n";
            if (inst.op == TACOp::LE)
                std::cout << "  setle %al\n";
            if (inst.op == TACOp::GE)
                std::cout << "  setge %al\n";
            std::cout << "  movzbl %al, %eax\n";
            std::cout << "  movl %eax, " << getOperand(inst.res) << "\n";
            break;

        case TACOp::JUMP:
            std::cout << "  jmp " << inst.res << "\n";
            break;

        case TACOp::JUMPF:
            std::cout << "  cmpl $0, " << getOperand(inst.arg1) << "\n";
            std::cout << "  je " << inst.res << "\n"; // Salta se falso (0)
            break;

        case TACOp::PARAM:
            if (param_counter < param_registers.size())
            {
                std::cout << "  movl " << getOperand(inst.res) << ", " << param_registers[param_counter] << "\n";
                param_counter++;
            }
            break;

        case TACOp::CALL:
            std::cout << "  call " << inst.arg1 << "\n";
            // Guarda o valor de retorno retornado em %eax
            if (!inst.res.empty())
            {
                std::cout << "  movl %eax, " << getOperand(inst.res) << "\n";
            }
            param_counter = 0; // Reinicia para a próxima chamada
            break;

        case TACOp::RETURN:
            if (!inst.res.empty())
            {
                std::cout << "  movl " << getOperand(inst.res) << ", %eax\n";
            }
            break;

        case TACOp::AND:
        case TACOp::OR:
            std::cout << "  movl " << getOperand(inst.arg1) << ", %eax\n";
            if (inst.op == TACOp::AND)
                std::cout << "  andl " << getOperand(inst.arg2) << ", %eax\n";
            if (inst.op == TACOp::OR)
                std::cout << "  orl " << getOperand(inst.arg2) << ", %eax\n";
            std::cout << "  movl %eax, " << getOperand(inst.res) << "\n";
            break;

        default:
            break;
        }
    }

    // ---------------------------------------------------------
    // 3. DIRETIVAS DO SISTEMA (Segurança de Stack)
    // ---------------------------------------------------------
    std::cout << "\n  .section .note.GNU-stack,\"\",@progbits\n";
}
