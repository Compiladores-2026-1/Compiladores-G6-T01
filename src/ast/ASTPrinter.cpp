#include "ASTPrinter.hpp"
#include <iostream>

// ========================================================
// Utilitário: Impressor da AST usando o padrão Visitor
// ========================================================

class ASTPrinter : public Visitor
{
private:
    int level = 0;

    void printIndent()
    {
        for (int i = 0; i < level; ++i)
            std::cout << "  ";
    }

public:
    void visit(ProgramNode *node) override
    {
        printIndent();
        std::cout << "[Program]\n";
        level++;
        for (auto decl : node->declarations)
        {
            if (decl)
                decl->accept(this);
        }
        level--;
    }

    void visit(BlockNode *node) override
    {
        printIndent();
        std::cout << "[Block]\n";
        level++;
        for (auto stmt : node->statements)
        {
            if (stmt)
                stmt->accept(this);
        }
        level--;
    }

    void visit(FuncDeclNode *node) override
    {
        printIndent();
        std::cout << "[FuncDecl] " << node->name << " (Type: " << node->return_type << ")\n";
        level++;
        for (auto param : node->params)
        {
            if (param)
                param->accept(this);
        }
        if (node->body)
            node->body->accept(this);
        level--;
    }

    void visit(VarDeclNode *node) override
    {
        printIndent();
        std::cout << "[VarDecl] " << node->name << " (Type: " << node->type_token << ")\n";
        if (node->init_expr)
        {
            level++;
            node->init_expr->accept(this);
            level--;
        }
    }

    void visit(AssignNode *node) override
    {
        printIndent();
        std::cout << "[Assign] " << node->name << " (Op: " << node->assign_op << ")\n";
        level++;
        node->expr->accept(this);
        level--;
    }

    void visit(BinOpNode *node) override
    {
        printIndent();
        std::cout << "[BinOp] Op: " << node->op_token << "\n";
        level++;
        node->left->accept(this);
        node->right->accept(this);
        level--;
    }

    void visit(UnOpNode *node) override
    {
        printIndent();
        std::cout << "[UnOp] Op: " << node->op_token << "\n";
        level++;
        node->expr->accept(this);
        level--;
    }

    void visit(IdNode *node) override
    {
        printIndent();
        std::cout << "ID: " << node->name << "\n";
    }

    void visit(LiteralNode *node) override
    {
        printIndent();
        std::cout << "Literal: " << node->value_str << "\n";
    }

    void visit(IfNode *node) override
    {
        printIndent();
        std::cout << "[If]\n";
        level++;
        node->condition->accept(this);
        printIndent();
        std::cout << "-> True Branch:\n";
        node->true_block->accept(this);
        if (node->false_block)
        {
            printIndent();
            std::cout << "-> False Branch:\n";
            node->false_block->accept(this);
        }
        level--;
    }

    void visit(WhileNode *node) override
    {
        printIndent();
        std::cout << "[While]\n";
        level++;
        node->condition->accept(this);
        node->body->accept(this);
        level--;
    }

    void visit(ForNode *node) override
    {
        printIndent();
        std::cout << "[For]\n";
        level++;
        if (node->init)
            node->init->accept(this);
        if (node->condition)
            node->condition->accept(this);
        if (node->step)
            node->step->accept(this);
        node->body->accept(this);
        level--;
    }

    void visit(ReturnNode *node) override
    {
        printIndent();
        std::cout << "[Return]\n";
        if (node->expr)
        {
            level++;
            node->expr->accept(this);
            level--;
        }
    }

    void visit(FuncCallNode *node) override
    {
        printIndent();
        std::cout << "[FuncCall] " << node->name << "\n";
        level++;
        for (auto arg : node->args)
        {
            if (arg)
                arg->accept(this);
        }
        level--;
    }

    void visit(BreakNode *node) override
    {
        printIndent();
        std::cout << "[Break]\n";
    }

    void visit(ContinueNode *node) override
    {
        printIndent();
        std::cout << "[Continue]\n";
    }
};

// Função global para facilitar o uso no main.cpp
void imprimirAST(ASTNode *raiz)
{
    if (!raiz)
        return;
    ASTPrinter printer;
    raiz->accept(&printer);
}
