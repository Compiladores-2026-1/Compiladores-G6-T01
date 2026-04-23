#pragma once
#include <string>
#include <vector>

// Definição dos tokens para operadores de incremento e decremento pós-fixados
#define POST_INC 1000
#define POST_DEC 1001

// Forward declarations das classes para o Visitor
class ProgramNode;
class BlockNode;
class FuncDeclNode;
class VarDeclNode;
class AssignNode;
class BinOpNode;
class UnOpNode;
class IdNode;
class LiteralNode;
class IfNode;
class WhileNode;
class ForNode;
class ReturnNode;
class FuncCallNode;
class BreakNode;
class ContinueNode;

// Interface do Visitor
class Visitor
{
public:
    virtual ~Visitor() = default;

    virtual void visit(ProgramNode *node) = 0;
    virtual void visit(BlockNode *node) = 0;
    virtual void visit(FuncDeclNode *node) = 0;
    virtual void visit(VarDeclNode *node) = 0;
    virtual void visit(AssignNode *node) = 0;
    virtual void visit(BinOpNode *node) = 0;
    virtual void visit(UnOpNode *node) = 0;
    virtual void visit(IdNode *node) = 0;
    virtual void visit(LiteralNode *node) = 0;
    virtual void visit(IfNode *node) = 0;
    virtual void visit(WhileNode *node) = 0;
    virtual void visit(ForNode *node) = 0;
    virtual void visit(ReturnNode *node) = 0;
    virtual void visit(FuncCallNode *node) = 0;
    virtual void visit(BreakNode *node) = 0;
    virtual void visit(ContinueNode *node) = 0;
};

// Classe Base da AST
class ASTNode
{
public:
    virtual ~ASTNode() = default;
    virtual void accept(Visitor *v) = 0;
};

// Classes Concretas da AST
class ProgramNode : public ASTNode
{
public:
    std::vector<ASTNode *> declarations; // Globais e funções

    ~ProgramNode() override;
    void accept(Visitor *v) override;
};

class BlockNode : public ASTNode
{
public:
    std::vector<ASTNode *> statements;

    ~BlockNode() override;
    void accept(Visitor *v) override;
};

class VarDeclNode : public ASTNode
{
public:
    int type_token;
    std::string name;
    ASTNode *init_expr;

    VarDeclNode(int t, std::string n, ASTNode *init = nullptr);
    ~VarDeclNode() override;
    void accept(Visitor *v) override;
};

class FuncDeclNode : public ASTNode
{
public:
    int return_type;
    std::string name;
    std::vector<VarDeclNode *> params;
    BlockNode *body; // Se for nullptr, é apenas protótipo

    FuncDeclNode(int ret, std::string n, BlockNode *b = nullptr);
    ~FuncDeclNode() override;
    void accept(Visitor *v) override;
};

class AssignNode : public ASTNode
{
public:
    std::string name;
    int assign_op; // =, +=, -=
    ASTNode *expr;

    AssignNode(std::string n, int op, ASTNode *e);
    ~AssignNode() override;
    void accept(Visitor *v) override;
};

class BinOpNode : public ASTNode
{
public:
    int op_token;
    ASTNode *left;
    ASTNode *right;

    BinOpNode(int op, ASTNode *l, ASTNode *r);
    ~BinOpNode() override;
    void accept(Visitor *v) override;
};

class UnOpNode : public ASTNode
{
public:
    int op_token;
    ASTNode *expr;

    UnOpNode(int op, ASTNode *e);
    ~UnOpNode() override;
    void accept(Visitor *v) override;
};

class IdNode : public ASTNode
{
public:
    std::string name;

    IdNode(std::string n);
    void accept(Visitor *v) override;
};

class LiteralNode : public ASTNode
{
public:
    int type_token;        // INT_LITERAL, FLOAT_LITERAL, etc.
    std::string value_str; // Guardamos como string para facilitar a impressão

    LiteralNode(int type, std::string val);
    void accept(Visitor *v) override;
};

class IfNode : public ASTNode
{
public:
    ASTNode *condition;
    ASTNode *true_block;
    ASTNode *false_block; // Pode ser nullptr

    IfNode(ASTNode *cond, ASTNode *tb, ASTNode *fb = nullptr);
    ~IfNode() override;
    void accept(Visitor *v) override;
};

class WhileNode : public ASTNode
{
public:
    ASTNode *condition;
    ASTNode *body;

    WhileNode(ASTNode *cond, ASTNode *b);
    ~WhileNode() override;
    void accept(Visitor *v) override;
};

class ForNode : public ASTNode
{
public:
    ASTNode *init;
    ASTNode *condition;
    ASTNode *step;
    ASTNode *body;

    ForNode(ASTNode *i, ASTNode *c, ASTNode *s, ASTNode *b);
    ~ForNode() override;
    void accept(Visitor *v) override;
};

class ReturnNode : public ASTNode
{
public:
    ASTNode *expr; // Pode ser nullptr

    ReturnNode(ASTNode *e = nullptr);
    ~ReturnNode() override;
    void accept(Visitor *v) override;
};

class FuncCallNode : public ASTNode
{
public:
    std::string name;
    std::vector<ASTNode *> args;

    FuncCallNode(std::string n);
    ~FuncCallNode() override;
    void accept(Visitor *v) override;
};

class BreakNode : public ASTNode
{
public:
    void accept(Visitor *v) override;
};

class ContinueNode : public ASTNode
{
public:
    void accept(Visitor *v) override;
};
