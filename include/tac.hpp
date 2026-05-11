#pragma once

#include "tokens.hpp"
#include "ast.hpp"

#include <vector>
#include <string>
#include <iostream>

// Tipos de instruções TAC suportadas
enum class TACOp
{
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    EQ,
    NEQ,
    LT,
    GT,
    LE,
    GE,
    AND,
    OR,
    ASSIGN,     // res = arg1
    JUMP,       // goto res
    JUMPF,      // ifFalse arg1 goto res
    LABEL,      // res:
    CALL,       // res = call arg1, arg2
    PARAM,      // param arg1
    RETURN,     // return arg1
    FUNC_BEGIN, // .begin_func res
    FUNC_END    // .end_func res
};

// Estrutura de uma linha de código TAC
struct TACInstruction
{
    TACOp op;
    std::string res;
    std::string arg1;
    std::string arg2;
};

class TACVisitor : public Visitor
{
private:
    std::vector<TACInstruction> instructions;
    int temp_counter = 0;
    int label_counter = 0;

    // Guarda o nome da variável ou temporário gerado pela última expressão visitada
    std::string current_result;

    // Pilhas para controlar os labels de loops (usados no break e continue)
    std::vector<std::string> loop_start_labels;
    std::vector<std::string> loop_end_labels;

    // Funções geradoras
    std::string newTemp();
    std::string newLabel();
    void emit(TACOp op, std::string res, std::string arg1 = "", std::string arg2 = "");

    // Auxiliar de conversão
    static std::string opToString(TACOp op);
    TACOp tokenToTACOp(int token);

public:
    ~TACVisitor() override = default;

    // Imprime o código TAC gerado
    void printTAC() const;

    // Métodos do Visitor
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
};
