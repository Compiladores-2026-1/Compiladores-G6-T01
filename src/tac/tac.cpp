#include "tac.hpp"

// =========================================================
// Funções Auxiliares de Geração
// =========================================================

std::string TACVisitor::newTemp()
{
    return "t" + std::to_string(temp_counter++);
}

std::string TACVisitor::newLabel()
{
    return "L" + std::to_string(label_counter++);
}

void TACVisitor::emit(TACOp op, std::string res, std::string arg1, std::string arg2)
{
    instructions.push_back({op, res, arg1, arg2});
}

TACOp TACVisitor::tokenToTACOp(int token)
{
    switch (token)
    {
    case PLUS:
        return TACOp::ADD;
    case MINUS:
        return TACOp::SUB;
    case MULT:
        return TACOp::MUL;
    case DIV:
        return TACOp::DIV;
    case MOD:
        return TACOp::MOD;
    case EQ:
        return TACOp::EQ;
    case NEQ:
        return TACOp::NEQ;
    case LT:
        return TACOp::LT;
    case GT:
        return TACOp::GT;
    case LE:
        return TACOp::LE;
    case GE:
        return TACOp::GE;
    case AND:
        return TACOp::AND;
    case OR:
        return TACOp::OR;
    default:
        return TACOp::ADD; // Fallback de segurança
    }
}

// =========================================================
// Implementação dos Visitors
// =========================================================

void TACVisitor::visit(ProgramNode *node)
{
    for (auto decl : node->declarations)
    {
        if (!decl)
            continue;

        // Global sem inicializacao (ex.: "int g;"): emite uma atribuicao a zero para
        // que ela apareca na secao .data e o gerador de Assembly a reconheca como
        // global (e nao como variavel local). Globais com init ja emitem o ASSIGN.
        if (auto *var = dynamic_cast<VarDeclNode *>(decl))
        {
            if (!var->init_expr)
            {
                emit(TACOp::ASSIGN, var->name, "0");
                continue;
            }
        }

        decl->accept(this);
    }
}

void TACVisitor::visit(BlockNode *node)
{
    for (auto stmt : node->statements)
    {
        if (stmt)
            stmt->accept(this);
    }
}

void TACVisitor::visit(FuncDeclNode *node)
{
    if (!node->body)
        return; // É apenas um protótipo, ignoramos no TAC

    // Empacota os nomes dos parâmetros (em ordem) no arg1 do FUNC_BEGIN para que
    // o gerador de Assembly saiba copiar os registradores de argumento (%edi, %esi, ...)
    // para os slots de pilha de cada parâmetro. O dump do TAC ignora esse arg1.
    std::string param_list;
    for (size_t i = 0; i < node->params.size(); i++)
    {
        if (i > 0)
            param_list += ",";
        param_list += node->params[i]->name;
    }

    emit(TACOp::FUNC_BEGIN, node->name, param_list);
    node->body->accept(this);
    emit(TACOp::FUNC_END, node->name);
}

void TACVisitor::visit(VarDeclNode *node)
{
    if (node->init_expr)
    {
        node->init_expr->accept(this);
        emit(TACOp::ASSIGN, node->name, current_result);
    }
}

void TACVisitor::visit(AssignNode *node)
{
    node->expr->accept(this);
    std::string right_val = current_result;

    if (node->assign_op == ASSIGN)
    {
        // Atribuição simples: x = y
        emit(TACOp::ASSIGN, node->name, right_val);
    }
    else
    {
        // Atribuição composta (+=, -=, *=, /=, %=)
        std::string res = newTemp();
        TACOp op;

        switch (node->assign_op)
        {
        case PLUSEQ:
            op = TACOp::ADD;
            break;
        case MINUSEQ:
            op = TACOp::SUB;
            break;
        case MULTEQ:
            op = TACOp::MUL;
            break;
        case DIVEQ:
            op = TACOp::DIV;
            break;
        case MODEQ:
            op = TACOp::MOD;
            break;
        default:
            op = TACOp::ADD;
            break;
        }

        emit(op, res, node->name, right_val);
        emit(TACOp::ASSIGN, node->name, res);
    }

    current_result = node->name;
}

void TACVisitor::visit(IfNode *node)
{
    std::string l_false = newLabel();
    std::string l_end = newLabel();

    node->condition->accept(this);
    emit(TACOp::JUMPF, l_false, current_result);

    node->true_block->accept(this);
    emit(TACOp::JUMP, l_end);

    emit(TACOp::LABEL, l_false);
    if (node->false_block)
    {
        node->false_block->accept(this);
    }
    emit(TACOp::LABEL, l_end);
}

void TACVisitor::visit(WhileNode *node)
{
    std::string l_start = newLabel();
    std::string l_end = newLabel();

    loop_start_labels.push_back(l_start);
    loop_end_labels.push_back(l_end);

    emit(TACOp::LABEL, l_start);

    node->condition->accept(this);
    emit(TACOp::JUMPF, l_end, current_result);

    node->body->accept(this);
    emit(TACOp::JUMP, l_start);

    emit(TACOp::LABEL, l_end);

    loop_start_labels.pop_back();
    loop_end_labels.pop_back();
}

void TACVisitor::visit(ForNode *node)
{
    std::string l_cond = newLabel();
    std::string l_step = newLabel();
    std::string l_end = newLabel();

    if (node->init)
        node->init->accept(this);

    emit(TACOp::LABEL, l_cond);
    if (node->condition)
    {
        node->condition->accept(this);
        emit(TACOp::JUMPF, l_end, current_result);
    }

    // No for, o continue pula para o incremento (step)
    loop_start_labels.push_back(l_step);
    loop_end_labels.push_back(l_end);

    if (node->body)
        node->body->accept(this);

    emit(TACOp::LABEL, l_step);
    if (node->step)
        node->step->accept(this);

    emit(TACOp::JUMP, l_cond);
    emit(TACOp::LABEL, l_end);

    loop_start_labels.pop_back();
    loop_end_labels.pop_back();
}

void TACVisitor::visit(ReturnNode *node)
{
    if (node->expr)
    {
        node->expr->accept(this);
        emit(TACOp::RETURN, current_result);
    }
    else
    {
        emit(TACOp::RETURN, "");
    }
}

void TACVisitor::visit(BinOpNode *node)
{
    node->left->accept(this);
    std::string arg1 = current_result;

    node->right->accept(this);
    std::string arg2 = current_result;

    std::string res = newTemp();
    emit(tokenToTACOp(node->op_token), res, arg1, arg2);
    current_result = res;
}

void TACVisitor::visit(UnOpNode *node)
{
    node->expr->accept(this);
    std::string expr_res = current_result;
    std::string res = newTemp();

    if (node->op_token == MINUS)
    {
        // Unário negativo: -x
        emit(TACOp::SUB, res, "0", expr_res);
        current_result = res;
    }
    else if (node->op_token == NOT || node->op_token == NOT_OP)
    {
        // Negação lógica: !x (x == 0)
        emit(TACOp::EQ, res, expr_res, "0");
        current_result = res;
    }
    else if (node->op_token == INC)
    {
        // Pré-incremento: ++x
        emit(TACOp::ADD, res, expr_res, "1");
        emit(TACOp::ASSIGN, expr_res, res);
        current_result = expr_res;
    }
    // OBS: O #define POST_INC e POST_DEC deve estar no topo do ast.hpp
    else if (node->op_token == POST_INC)
    {
        // Pós-incremento: x++
        std::string old_val = newTemp();
        emit(TACOp::ASSIGN, old_val, expr_res);
        emit(TACOp::ADD, res, expr_res, "1");
        emit(TACOp::ASSIGN, expr_res, res);
        current_result = old_val;
    }
    else if (node->op_token == DEC)
    {
        // Pré-decremento: --x
        emit(TACOp::SUB, res, expr_res, "1");
        emit(TACOp::ASSIGN, expr_res, res);
        current_result = expr_res;
    }
    else if (node->op_token == POST_DEC)
    {
        // Pós-decremento: x--
        std::string old_val = newTemp();
        emit(TACOp::ASSIGN, old_val, expr_res);
        emit(TACOp::SUB, res, expr_res, "1");
        emit(TACOp::ASSIGN, expr_res, res);
        current_result = old_val;
    }
}

void TACVisitor::visit(IdNode *node)
{
    current_result = node->name;
}

void TACVisitor::visit(LiteralNode *node)
{
    current_result = node->value_str;
}

void TACVisitor::visit(FuncCallNode *node)
{
    for (auto arg : node->args)
    {
        arg->accept(this);
        emit(TACOp::PARAM, current_result);
    }

    std::string res = newTemp();
    emit(TACOp::CALL, res, node->name, std::to_string(node->args.size()));
    current_result = res;
}

void TACVisitor::visit(BreakNode *node)
{
    if (!loop_end_labels.empty())
    {
        emit(TACOp::JUMP, loop_end_labels.back());
    }
}

void TACVisitor::visit(ContinueNode *node)
{
    if (!loop_start_labels.empty())
    {
        emit(TACOp::JUMP, loop_start_labels.back());
    }
}

// =========================================================
// Impressão e Formatação Final
// =========================================================

std::string TACVisitor::opToString(TACOp op)
{
    switch (op)
    {
    case TACOp::ADD:
        return "+";
    case TACOp::SUB:
        return "-";
    case TACOp::MUL:
        return "*";
    case TACOp::DIV:
        return "/";
    case TACOp::MOD:
        return "%";
    case TACOp::EQ:
        return "==";
    case TACOp::NEQ:
        return "!=";
    case TACOp::LT:
        return "<";
    case TACOp::GT:
        return ">";
    case TACOp::LE:
        return "<=";
    case TACOp::GE:
        return ">=";
    case TACOp::AND:
        return "&&";
    case TACOp::OR:
        return "||";
    default:
        return "?";
    }
}

void TACVisitor::printTAC() const
{
    std::cout << "======================= TAC ========================\n";
    for (const auto &inst : instructions)
    {
        if (inst.op == TACOp::LABEL)
        {
            std::cout << inst.res << ":\n";
        }
        else if (inst.op == TACOp::FUNC_BEGIN)
        {
            std::cout << "\n.begin_func " << inst.res << "\n";
        }
        else if (inst.op == TACOp::FUNC_END)
        {
            std::cout << ".end_func " << inst.res << "\n";
        }
        else if (inst.op == TACOp::ASSIGN)
        {
            std::cout << "  " << inst.res << " = " << inst.arg1 << "\n";
        }
        else if (inst.op == TACOp::JUMP)
        {
            std::cout << "  goto " << inst.res << "\n";
        }
        else if (inst.op == TACOp::JUMPF)
        {
            std::cout << "  ifFalse " << inst.arg1 << " goto " << inst.res << "\n";
        }
        else if (inst.op == TACOp::CALL)
        {
            std::cout << "  " << inst.res << " = call " << inst.arg1 << ", " << inst.arg2 << "\n";
        }
        else if (inst.op == TACOp::PARAM)
        {
            std::cout << "  param " << inst.res << "\n";
        }
        else if (inst.op == TACOp::RETURN)
        {
            std::cout << "  return " << inst.res << "\n";
        }
        else
        {
            std::cout << "  " << inst.res << " = " << inst.arg1 << " "
                      << opToString(inst.op) << " " << inst.arg2 << "\n";
        }
    }
    std::cout << "========================================================\n";
}
