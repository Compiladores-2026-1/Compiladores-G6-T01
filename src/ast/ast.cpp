#include "ast.hpp"

// ========================================================
// Implementação das Classes da AST
// ========================================================

ProgramNode::~ProgramNode()
{
    for (auto n : declarations)
        delete n;
}

BlockNode::~BlockNode()
{
    for (auto n : statements)
        delete n;
}

VarDeclNode::VarDeclNode(int t, std::string n, ASTNode *init) : type_token(t), name(n), init_expr(init) {}
VarDeclNode::~VarDeclNode() { delete init_expr; }

FuncDeclNode::FuncDeclNode(int ret, std::string n, BlockNode *b) : return_type(ret), name(n), body(b) {}
FuncDeclNode::~FuncDeclNode()
{
    for (auto p : params)
        delete p;
    delete body;
}

AssignNode::AssignNode(std::string n, int op, ASTNode *e) : name(n), assign_op(op), expr(e) {}
AssignNode::~AssignNode() { delete expr; }

BinOpNode::BinOpNode(int op, ASTNode *l, ASTNode *r) : op_token(op), left(l), right(r) {}
BinOpNode::~BinOpNode()
{
    delete left;
    delete right;
}

UnOpNode::UnOpNode(int op, ASTNode *e) : op_token(op), expr(e) {}
UnOpNode::~UnOpNode() { delete expr; }

IdNode::IdNode(std::string n) : name(n) {}

LiteralNode::LiteralNode(int type, std::string val) : type_token(type), value_str(val) {}

IfNode::IfNode(ASTNode *cond, ASTNode *tb, ASTNode *fb) : condition(cond), true_block(tb), false_block(fb) {}
IfNode::~IfNode()
{
    delete condition;
    delete true_block;
    delete false_block;
}

WhileNode::WhileNode(ASTNode *cond, ASTNode *b) : condition(cond), body(b) {}
WhileNode::~WhileNode()
{
    delete condition;
    delete body;
}

ForNode::ForNode(ASTNode *i, ASTNode *c, ASTNode *s, ASTNode *b) : init(i), condition(c), step(s), body(b) {}
ForNode::~ForNode()
{
    delete init;
    delete condition;
    delete step;
    delete body;
}

ReturnNode::ReturnNode(ASTNode *e) : expr(e) {}
ReturnNode::~ReturnNode() { delete expr; }

FuncCallNode::FuncCallNode(std::string n) : name(n) {}
FuncCallNode::~FuncCallNode()
{
    for (auto a : args)
        delete a;
}

// ========================================================
// Implementação do Método accept para cada Nó da AST
// ========================================================

void ProgramNode::accept(Visitor *v) { v->visit(this); }
void BlockNode::accept(Visitor *v) { v->visit(this); }
void VarDeclNode::accept(Visitor *v) { v->visit(this); }
void FuncDeclNode::accept(Visitor *v) { v->visit(this); }
void AssignNode::accept(Visitor *v) { v->visit(this); }
void BinOpNode::accept(Visitor *v) { v->visit(this); }
void UnOpNode::accept(Visitor *v) { v->visit(this); }
void IdNode::accept(Visitor *v) { v->visit(this); }
void LiteralNode::accept(Visitor *v) { v->visit(this); }
void IfNode::accept(Visitor *v) { v->visit(this); }
void WhileNode::accept(Visitor *v) { v->visit(this); }
void ForNode::accept(Visitor *v) { v->visit(this); }
void ReturnNode::accept(Visitor *v) { v->visit(this); }
void FuncCallNode::accept(Visitor *v) { v->visit(this); }
void BreakNode::accept(Visitor *v) { v->visit(this); }
void ContinueNode::accept(Visitor *v) { v->visit(this); }
