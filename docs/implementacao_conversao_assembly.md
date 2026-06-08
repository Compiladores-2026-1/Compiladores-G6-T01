# Implementação da Geração de Código Final (Assembly)

> **Status:** Esta fase ainda não foi implementada. O modo `--codegen` está declarado em `src/main.cpp` mas comentado (linha 153). A arquitetura planejada está descrita em [Geração de Código Final (TAC para Assembly)](conversao_assembly.md).

---

## 1. Onde a Implementação Ficará

Quando implementada, a geração de Assembly seguirá o mesmo padrão das fases anteriores:

| Arquivo (planejado)              | Responsabilidade                                         |
|----------------------------------|----------------------------------------------------------|
| `src/codegen/codegen.cpp`        | Tradutor de lista TAC para Assembly x86-64               |
| `include/codegen.hpp`            | Declaração pública do gerador                            |
| `src/runners/codegen_runner.cpp` | Implementação de `run_codegen_mode(bool debugMode)`      |

---

## 2. Abordagem Planejada

O gerador receberá a lista de `TACInstruction` produzida pela fase anterior e percorrerá cada instrução, emitindo o Assembly correspondente para um `std::ostream` (arquivo `.s` ou `stdout`).

```cpp
class AssemblyGenerator {
    std::ostream &out;
    // mapa: nome da variável → offset no frame (ex: "x" → -8)
    std::unordered_map<std::string, int> var_offsets;
    int frame_size = 0;

public:
    AssemblyGenerator(std::ostream &o) : out(o) {}
    void generate(const std::vector<TACInstruction> &instructions);
};
```

---

## 3. Fases Internas do Gerador

### 3.1. Alocação de Frame

Antes de emitir o corpo de uma função, o gerador precisa calcular o tamanho do frame de pilha:

1. Percorrer todas as instruções da função.
2. Para cada temporária (`t0`, `t1`, ...) e variável local, alocar 8 bytes no frame.
3. Emitir `subq $N, %rsp` com `N` = tamanho total alinhado a 16 bytes.

### 3.2. Resolução de Operandos

Cada operando TAC (variável, temporária ou literal) é traduzido para um operando Assembly:

| Operando TAC | Assembly           |
|--------------|--------------------|
| Literal `5`  | `$5`               |
| Variável `x` | `-8(%rbp)` (offset do frame) |
| Temporária `t0` | `-16(%rbp)` (offset do frame) |

### 3.3. Instrução por Instrução

Cada `TACOp` é mapeado para um pequeno bloco de instruções. Ver mapeamento completo em [Geração de Código Final (TAC para Assembly)](conversao_assembly.md).

---

## 4. Ativação do Modo `--codegen`

Para ativar o modo, será necessário descomentar em `src/main.cpp`:

```cpp
// case MODE_CODEGEN:
//     exit_code = run_codegen_mode(debug_symtable);
//     break;
```

E implementar `run_codegen_mode` em `src/runners/codegen_runner.cpp`:

```cpp
int run_codegen_mode(bool debugMode) {
    int parse_result = yyparse();
    if (parse_result == 0 && root_ast != nullptr) {
        SemanticVisitor semantic(debugMode);
        root_ast->accept(&semantic);

        TACGenerator tac_gen;
        root_ast->accept(&tac_gen);

        AssemblyGenerator asm_gen(std::cout);
        asm_gen.generate(tac_gen.getInstructions());
    }
    return parse_result;
}
```

---

## 5. Referências

- [Geração de Código Final (TAC para Assembly)](conversao_assembly.md) — arquitetura e mapeamento de instruções
- [Implementação da Geração de Código Intermediário](implementacao_conversao_tac.md) — fase anterior na cadeia
