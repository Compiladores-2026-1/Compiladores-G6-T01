#include "optimizer.hpp"
#include <string>

// Função auxiliar para verificar se a string é um número
bool Optimizer::isNumber(const std::string &str)
{
    if (str.empty())
        return false;
    return str.find_first_not_of("0123456789.-") == std::string::npos;
}

// CICLO PRINCIPAL DE OTIMIZAÇÃO
std::vector<TACInstruction> Optimizer::optimize(const std::vector<TACInstruction> &instructions)
{
    std::vector<TACInstruction> optimized = instructions;
    bool changed;

    do
    {
        changed = false;
        size_t original_size = optimized.size();

        // 1. Dobragem de Constantes (Constant Folding)
        optimized = constantFolding(optimized);

        // 2. Propagação de Cópia (Copy Propagation)
        optimized = copyPropagation(optimized);

        // 3. Otimização de Janela (Peephole)
        optimized = peepholeOptimization(optimized);

        // 4. Eliminação de Código Morto (Dead Code Elimination)
        std::vector<TACInstruction> afterDCE = deadCodeElimination(optimized);

        // Verificação se houve alteração para continuar o ciclo
        if (afterDCE.size() != original_size)
        {
            changed = true;
        }
        else
        {
            for (size_t i = 0; i < optimized.size(); i++)
            {
                if (optimized[i].arg1 != afterDCE[i].arg1 ||
                    optimized[i].arg2 != afterDCE[i].arg2 ||
                    optimized[i].op != afterDCE[i].op ||
                    optimized[i].res != afterDCE[i].res)
                {
                    changed = true;
                    break;
                }
            }
        }

        optimized = afterDCE;

    } while (changed);

    return optimized;
}

// 1. CONSTANT FOLDING
std::vector<TACInstruction> Optimizer::constantFolding(const std::vector<TACInstruction> &instructions)
{
    std::vector<TACInstruction> result;

    for (const auto &inst : instructions)
    {
        if ((inst.op == TACOp::ADD || inst.op == TACOp::SUB ||
             inst.op == TACOp::MUL || inst.op == TACOp::DIV || inst.op == TACOp::MOD ||
             inst.op == TACOp::EQ || inst.op == TACOp::NEQ || inst.op == TACOp::LT ||
             inst.op == TACOp::GT || inst.op == TACOp::LE || inst.op == TACOp::GE) &&
            isNumber(inst.arg1) && isNumber(inst.arg2))
        {

            int val1 = std::stoi(inst.arg1);
            int val2 = std::stoi(inst.arg2);
            int resVal = 0;

            switch (inst.op)
            {
            // Matemáticos
            case TACOp::ADD:
                resVal = val1 + val2;
                break;
            case TACOp::SUB:
                resVal = val1 - val2;
                break;
            case TACOp::MUL:
                resVal = val1 * val2;
                break;
            case TACOp::DIV:
                if (val2 != 0)
                    resVal = val1 / val2;
                break;
            case TACOp::MOD:
                if (val2 != 0)
                    resVal = val1 % val2;
                break;

            // Relacionais (Booleanos: 1 = True, 0 = False)
            case TACOp::EQ:
                resVal = (val1 == val2) ? 1 : 0;
                break;
            case TACOp::NEQ:
                resVal = (val1 != val2) ? 1 : 0;
                break;
            case TACOp::LT:
                resVal = (val1 < val2) ? 1 : 0;
                break;
            case TACOp::GT:
                resVal = (val1 > val2) ? 1 : 0;
                break;
            case TACOp::LE:
                resVal = (val1 <= val2) ? 1 : 0;
                break;
            case TACOp::GE:
                resVal = (val1 >= val2) ? 1 : 0;
                break;
            default:
                break;
            }

            TACInstruction optInst;
            optInst.op = TACOp::ASSIGN;
            optInst.res = inst.res;
            optInst.arg1 = std::to_string(resVal);
            optInst.arg2 = "";
            result.push_back(optInst);
        }
        else
        {
            result.push_back(inst);
        }
    }
    return result;
}

// 2. PEEPHOLE OPTIMIZATION
std::vector<TACInstruction> Optimizer::peepholeOptimization(const std::vector<TACInstruction> &instructions)
{
    std::vector<TACInstruction> result;
    size_t n = instructions.size();

    for (size_t i = 0; i < n; ++i)
    {
        TACInstruction inst = instructions[i];

        // Simplificação Algébrica
        if (inst.op == TACOp::ADD)
        {
            if (inst.arg1 == "0")
            {
                inst.op = TACOp::ASSIGN;
                inst.arg1 = inst.arg2;
                inst.arg2 = "";
            }
            else if (inst.arg2 == "0")
            {
                inst.op = TACOp::ASSIGN;
                inst.arg2 = "";
            }
        }
        else if (inst.op == TACOp::SUB)
        {
            if (inst.arg2 == "0")
            {
                inst.op = TACOp::ASSIGN;
                inst.arg2 = "";
            }
        }
        else if (inst.op == TACOp::MUL)
        {
            if (inst.arg1 == "1")
            {
                inst.op = TACOp::ASSIGN;
                inst.arg1 = inst.arg2;
                inst.arg2 = "";
            }
            else if (inst.arg2 == "1")
            {
                inst.op = TACOp::ASSIGN;
                inst.arg2 = "";
            }
            else if (inst.arg1 == "0" || inst.arg2 == "0")
            {
                inst.op = TACOp::ASSIGN;
                inst.arg1 = "0";
                inst.arg2 = "";
            }
        }
        else if (inst.op == TACOp::DIV)
        {
            if (inst.arg2 == "1")
            {
                inst.op = TACOp::ASSIGN;
                inst.arg2 = "";
            }
        }

        // Eliminação de Saltos Redundantes
        if (inst.op == TACOp::JUMP && i + 1 < n)
        {
            TACInstruction nextInst = instructions[i + 1];
            if (nextInst.op == TACOp::LABEL && inst.res == nextInst.res)
            {
                continue;
            }
        }

        result.push_back(inst);
    }
    return result;
}

// 3. COPY E CONSTANT PROPAGATION
std::vector<TACInstruction> Optimizer::copyPropagation(const std::vector<TACInstruction> &instructions)
{
    std::vector<TACInstruction> result;
    std::map<std::string, std::string> copies;

    for (TACInstruction inst : instructions)
    {
        // Barreiras de Fluxo de Controle
        if (inst.op == TACOp::LABEL || inst.op == TACOp::FUNC_BEGIN ||
            inst.op == TACOp::JUMP || inst.op == TACOp::JUMPF || inst.op == TACOp::CALL)
        {
            copies.clear();
        }

        // Substituição (Propagar a cópia ou a constante)
        if (!inst.arg1.empty() && copies.find(inst.arg1) != copies.end())
        {
            inst.arg1 = copies[inst.arg1];
        }
        if (!inst.arg2.empty() && copies.find(inst.arg2) != copies.end())
        {
            inst.arg2 = copies[inst.arg2];
        }

        // Atualização do mapa de cópias
        if (!inst.res.empty())
        {
            copies.erase(inst.res);

            for (auto it = copies.begin(); it != copies.end();)
            {
                if (it->second == inst.res)
                {
                    it = copies.erase(it);
                }
                else
                {
                    ++it;
                }
            }

            // Permite propagar números e variáveis
            if (inst.op == TACOp::ASSIGN && !inst.arg1.empty())
            {
                copies[inst.res] = inst.arg1;
            }
        }

        result.push_back(inst);
    }
    return result;
}

// 4. DEAD CODE ELIMINATION
std::vector<TACInstruction> Optimizer::deadCodeElimination(const std::vector<TACInstruction> &instructions)
{
    std::set<std::string> used_variables;

    // 1ª Passagem: Descobrir quem é lido
    for (const auto &inst : instructions)
    {
        if (!inst.arg1.empty() && inst.op != TACOp::CALL && inst.op != TACOp::FUNC_BEGIN && inst.op != TACOp::FUNC_END)
        {
            if (!isNumber(inst.arg1))
                used_variables.insert(inst.arg1);
        }

        if (!inst.arg2.empty() && inst.op != TACOp::CALL)
        {
            if (!isNumber(inst.arg2))
                used_variables.insert(inst.arg2);
        }

        if (inst.op == TACOp::PARAM || inst.op == TACOp::RETURN)
        {
            if (!inst.res.empty() && !isNumber(inst.res))
            {
                used_variables.insert(inst.res);
            }
        }
    }

    // 2ª Passagem: Eliminar instruções que escrevem em variáveis nunca lidas
    std::vector<TACInstruction> result;
    for (const auto &inst : instructions)
    {
        bool is_pure_compute = (inst.op == TACOp::ADD || inst.op == TACOp::SUB ||
                                inst.op == TACOp::MUL || inst.op == TACOp::DIV ||
                                inst.op == TACOp::MOD || inst.op == TACOp::EQ ||
                                inst.op == TACOp::NEQ || inst.op == TACOp::LT ||
                                inst.op == TACOp::GT || inst.op == TACOp::LE ||
                                inst.op == TACOp::GE || inst.op == TACOp::AND ||
                                inst.op == TACOp::OR || inst.op == TACOp::ASSIGN);

        if (is_pure_compute && !inst.res.empty())
        {
            if (used_variables.find(inst.res) == used_variables.end())
            {
                // Apaga apenas variáveis temporárias criadas pelo compilador
                if (inst.res[0] == 't')
                {
                    continue;
                }
            }
        }
        result.push_back(inst);
    }
    return result;
}
