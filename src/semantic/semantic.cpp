#include "semantic.hpp"

void SemanticVisitor::visit(ProgramNode *node)
{
    for (auto decl : node->declarations)
    {
        if (decl)
            decl->accept(this);
    }
}

void SemanticVisitor::visit(BlockNode *node)
{
    symTable.enterScope();

    for (auto stmt : node->statements)
    {
        if (stmt)
            stmt->accept(this);
    }

    // Imprime a tabela antes de destruir as variáveis locais deste bloco
    if (debugMode)
    {
        std::cout << "\n--- Saindo do escopo (Nível " << symTable.getCurrentLevel() << ") ---\n";
        symTable.printTable();
    }
    symTable.exitScope();
}

void SemanticVisitor::visit(FuncDeclNode *node)
{
    Symbol *existing = symTable.lookup(node->name);

    if (existing)
    {
        if (existing->category != SymbolCategory::FUNC)
        {
            std::cerr << "Erro Semântico: '" << node->name << "' já declarado como variável.\n";
            return;
        }

        if (node->body != nullptr)
        {
        }
    }
    else
    {
        symTable.insert(node->name, node->return_type, SymbolCategory::FUNC);
    }

    if (node->body)
    {
        symTable.enterScope();
        for (auto param : node->params)
        {
            if (!symTable.insert(param->name, param->type_token, SymbolCategory::PARAM))
            {
                std::cerr << "Erro Semântico: Parâmetro duplicado '" << param->name << "'.\n";
            }
        }
        node->body->accept(this);
        symTable.exitScope();
    }
}

void SemanticVisitor::visit(VarDeclNode *node)
{
    if (!symTable.insert(node->name, node->type_token, SymbolCategory::VAR))
    {
        std::cerr << "Erro Semântico: Redeclaração da variável '" << node->name << "' no mesmo escopo.\n";
    }

    if (node->init_expr)
    {
        node->init_expr->accept(this);
    }
}

void SemanticVisitor::visit(AssignNode *node)
{
    Symbol *sym = symTable.lookup(node->name);

    if (!sym)
    {
        std::cerr << "Erro Semântico: Variável '" << node->name << "' não declarada.\n";
    }
    else if (sym->category == SymbolCategory::FUNC)
    {
        std::cerr << "Erro Semântico: '" << node->name << "' é uma função. Não é possível atribuir valor a ela.\n";
    }

    if (node->expr)
    {
        node->expr->accept(this);
    }
}

void SemanticVisitor::visit(IfNode *node)
{
    if (node->condition)
        node->condition->accept(this);
    if (node->true_block)
        node->true_block->accept(this);
    if (node->false_block)
        node->false_block->accept(this);
}

void SemanticVisitor::visit(WhileNode *node)
{
    if (node->condition)
        node->condition->accept(this);
    if (node->body)
        node->body->accept(this);
}

void SemanticVisitor::visit(ForNode *node)
{
    symTable.enterScope();

    if (node->init)
        node->init->accept(this);
    if (node->condition)
        node->condition->accept(this);
    if (node->step)
        node->step->accept(this);
    if (node->body)
        node->body->accept(this);

    symTable.exitScope();
}

void SemanticVisitor::visit(ReturnNode *node)
{
    if (node->expr)
    {
        node->expr->accept(this);
    }
}

void SemanticVisitor::visit(BinOpNode *node)
{
    if (node->left)
        node->left->accept(this);
    if (node->right)
        node->right->accept(this);
}

void SemanticVisitor::visit(UnOpNode *node)
{
    if (node->expr)
        node->expr->accept(this);
}

void SemanticVisitor::visit(IdNode *node)
{
    Symbol *sym = symTable.lookup(node->name);

    if (!sym)
    {
        std::cerr << "Erro Semântico: Uso de variável não declarada: '" << node->name << "'.\n";
    }
}

void SemanticVisitor::visit(LiteralNode *node)
{
}

void SemanticVisitor::visit(FuncCallNode *node)
{
    Symbol *sym = symTable.lookup(node->name);

    if (!sym)
    {
        std::cerr << "Erro Semântico: Chamada de função desconhecida: '" << node->name << "'.\n";
    }
    else if (sym->category != SymbolCategory::FUNC)
    {
        std::cerr << "Erro Semântico: O identificador '" << node->name << "' não é uma função.\n";
    }

    for (auto arg : node->args)
    {
        if (arg)
            arg->accept(this);
    }
}

void SemanticVisitor::visit(BreakNode *node) {}
void SemanticVisitor::visit(ContinueNode *node) {}

void SemanticVisitor::printSymbolTable() const
{
    symTable.printTable();
}
