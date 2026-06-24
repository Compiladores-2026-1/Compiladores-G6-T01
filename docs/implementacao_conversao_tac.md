# Implementação da Geração de Código Intermediário (TAC)

> **Status:** Implementada. O modo é acionado pela flag `--tac` (ou `-fdump-tac`) em `src/main.cpp`. O gerador percorre a AST validada e produz uma lista de instruções de três endereços. O formato e os exemplos de TAC estão documentados em [Geração de Código Intermediário](conversao_tac.md).

---

## 1. Onde a Implementação Fica

A geração de TAC segue o mesmo padrão das fases anteriores:

| Arquivo                       | Responsabilidade                                        |
|-------------------------------|---------------------------------------------------------|
| `include/tac.hpp`             | Declaração de `TACOp`, `TACInstruction` e `TACVisitor`  |
| `src/tac/tac.cpp`             | Implementação do `TACVisitor` e de `printTAC()`         |
| `src/runners/tac_runner.cpp`  | Implementação de `run_tac_mode(bool debugMode)`         |

---

## 2. Estrutura de Dados

A geração de TAC produz uma lista de instruções `TACInstruction` (ver `include/tac.hpp`):

```cpp
enum class TACOp {
    ADD, SUB, MUL, DIV, MOD,
    EQ, NEQ, LT, GT, LE, GE,
    AND, OR,
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

struct TACInstruction {
    TACOp       op;
    std::string res;
    std::string arg1;
    std::string arg2;
};
```

O `TACVisitor` mantém:
- Um contador de temporárias (`t0`, `t1`, ...) via `newTemp()`.
- Um contador de labels (`L0`, `L1`, ...) via `newLabel()`.
- A lista de instruções geradas (`instructions`).
- Pilhas de labels de início/fim de laço, usadas por `break` e `continue`.

---

## 3. Integração com a AST

O gerador é implementado como um **Visitor** sobre a AST, seguindo o mesmo padrão do `SemanticVisitor`:

```cpp
class TACVisitor : public Visitor {
public:
    void visit(ProgramNode *node)  override;
    void visit(FuncDeclNode *node) override;
    void visit(BinOpNode *node)    override;
    void visit(IfNode *node)       override;
    void visit(WhileNode *node)    override;
    // ... demais nós
    void printTAC() const;
};
```

Cada `visit` emite (via `emit(...)`) as instruções TAC correspondentes ao nó. Para expressões, o gerador guarda em `current_result` o nome da temporária/variável que contém o resultado, permitindo encadear subexpressões.

---

## 4. Ativação do Modo `--tac`

A flag já está ligada em `src/main.cpp`:

```cpp
else if (arg == "-fdump-tac" || arg == "--tac") mode = MODE_TAC;
// ...
case MODE_TAC:
    exit_code = run_tac_mode(debug_symtable);
    break;
```

`run_tac_mode` (em `src/runners/tac_runner.cpp`) roda o parser, executa a análise semântica e então percorre a AST com o `TACVisitor`, imprimindo o resultado:

```cpp
int run_tac_mode(bool debugMode) {
    int parse_result = yyparse();
    if (parse_result == 0 && root_ast != nullptr) {
        SemanticVisitor semantic_checker(debugMode);
        root_ast->accept(&semantic_checker);

        TACVisitor tac_visitor;
        root_ast->accept(&tac_visitor);
        tac_visitor.printTAC();
    }
    return parse_result;
}
```

Exemplo de uso:

```bash
# Imprime o TAC em arquivo (gera programa.tac quando -o e omitido)
./compilador --tac programa.cmm -o programa.tac
```

---

## 5. Referência

Para entender o formato e os exemplos de TAC produzidos, consulte:
- [Geração de Código Intermediário (C-- Strict para TAC)](conversao_tac.md)
- [Testes de Geração de Código Intermediário](testes_conversao_tac.md)
