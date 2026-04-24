#pragma once
#include "ast.hpp"
#include "symtable.hpp"
#include <iostream>

class SemanticVisitor : public Visitor
{
private:
    SymbolTable symTable;
    bool debugMode;

public:
    SemanticVisitor(bool debug = false) : debugMode(debug) {}
    ~SemanticVisitor() override = default;

    void visit(ProgramNode *node) override;
    void visit(BlockNode *node) override;
    void visit(FuncDeclNode *node) override;
    void visit(VarDeclNode *node) override;
    void visit(AssignNode *node) override;
    void visit(BinOpNode *node) override;
    void visit(UnOpNode *node) override;
    void visit(IdNode *node) override;
    void visit(LiteralNode *node) override;
    void visit(IfNode *node) override;
    void visit(WhileNode *node) override;
    void visit(ForNode *node) override;
    void visit(ReturnNode *node) override;
    void visit(FuncCallNode *node) override;
    void visit(BreakNode *node) override;
    void visit(ContinueNode *node) override;
    void printSymbolTable() const;
};
