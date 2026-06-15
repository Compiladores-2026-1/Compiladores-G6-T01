# Implementação da Geração de Código Intermediário (TAC)

> **Status:** Esta fase ainda não foi implementada. O modo `--tac` está declarado em `src/main.cpp` mas comentado (linha 149). A estrutura de dados TAC e a conversão planejada estão documentadas em [Geração de Código Intermediário](conversao_tac.md).

---

## 1. Onde a Implementação Ficará

Quando implementada, a geração de TAC seguirá o mesmo padrão das fases anteriores:

| Arquivo (planejado)        | Responsabilidade                                    |
|----------------------------|-----------------------------------------------------|
| `src/tac/tac.cpp`          | Definição da struct `TACInstruction` e `TACOp`      |
| `include/tac.hpp`          | Declaração pública da struct e do gerador           |
| `src/runners/tac_runner.cpp` | Implementação de `run_tac_mode(bool debugMode)` |

---

## 2. Estrutura de Dados Planejada

A geração de TAC produzirá uma lista de instruções `TACInstruction`:

```cpp
enum class TACOp {
    ADD, SUB, MUL, DIV, MOD,
    EQ, NEQ, LT, GT, LE, GE,
    AND, OR,
    ASSIGN,
    JUMP, JUMPF,
    LABEL,
    CALL, PARAM, GET_PARAM,
    RETURN,
    FUNC_BEGIN, FUNC_END
};

struct TACInstruction {
    TACOp       op;
    std::string res;
    std::string arg1;
    std::string arg2;
};
```

O gerador manterá:
- Um contador de temporárias (`t0`, `t1`, ...)
- Um contador de labels (`L0`, `L1`, ...)
- Uma lista de instruções geradas por função

---

## 3. Integração com a AST

O gerador de TAC será implementado como um **Visitor** sobre a AST, seguindo o mesmo padrão já usado pelo `SemanticVisitor`:

```cpp
class TACGenerator : public Visitor {
public:
    void visit(ProgramNode *node)  override;
    void visit(FuncDeclNode *node) override;
    void visit(BinOpNode *node)    override;
    void visit(IfNode *node)       override;
    void visit(WhileNode *node)    override;
    // ... demais nós
};
```

Cada `visit` emitirá instruções TAC correspondentes ao nó visitado. Para expressões, o gerador retornará o nome da temporária que contém o resultado (usando um campo interno como `std::string last_temp`).

---

## 4. Ativação do Modo `--tac`

Para ativar o modo, será necessário descomentar o trecho em `src/main.cpp`:

```cpp
// case MODE_TAC:
//     exit_code = run_tac_mode(debug_symtable);
//     break;
```

E implementar `run_tac_mode` em `src/runners/tac_runner.cpp` seguindo o mesmo padrão do `run_semantic_mode`:

```cpp
int run_tac_mode(bool debugMode) {
    int parse_result = yyparse();
    if (parse_result == 0 && root_ast != nullptr) {
        SemanticVisitor semantic(debugMode);
        root_ast->accept(&semantic);

        TACGenerator tac_gen;
        root_ast->accept(&tac_gen);
        tac_gen.printInstructions();
    }
    return parse_result;
}
```

---

## 5. Referência

Para entender o formato e os exemplos de TAC produzidos, consulte:
- [Geração de Código Intermediário (C-- Strict para TAC)](conversao_tac.md)
