# Testes de Geração de Código Intermediário (TAC)

> **Status:** Esta fase ainda não foi implementada. Os testes serão criados quando a geração de TAC estiver disponível. Consulte [Implementação da Geração de Código Intermediário](implementacao_conversao_tac.md) para o estado atual.

---

## 1. Objetivo

Os testes desta fase têm dois objetivos:

1. **Validação positiva:** confirmar que programas C-- Strict válidos geram a sequência TAC correta para cada construção da linguagem.
2. **Validação negativa:** confirmar que o gerador não produz TAC para programas que falharam na análise semântica.

---

## 2. Estratégia de Teste

Os testes serão escritos em **GoogleTest** seguindo o mesmo padrão dos testes léxicos e sintáticos já existentes.

Cada teste:

1. Injeta código C-- Strict via `yy_scan_string`.
2. Executa o parsing e a análise semântica.
3. Chama o gerador de TAC.
4. Compara a lista de `TACInstruction` gerada com a esperada.

```cpp
TEST(TACTest, ExprессaoAritmetica) {
    auto buffer = yy_scan_string("int main() { int x = 3 + 4; return 0; }");
    // ... parse + semântica + TAC
    // Verifica instruções geradas:
    // t0 = 3 + 4
    // x = t0
    // return 0
    yy_delete_buffer(buffer);
    yylex_destroy();
}
```

---

## 3. Cenários Planejados

### 3.1. Expressões

| Cenário | Construção C-- | Instruções TAC esperadas |
|---|---|---|
| Adição simples | `int x = a + b;` | `t0 = a + b` / `x = t0` |
| Expressão composta | `int r = (a + b) * c;` | `t0 = a + b` / `t1 = t0 * c` / `r = t1` |
| Operador unário negação | `int r = -x;` | `t0 = 0 - x` / `r = t0` |
| Pré-incremento | `++x;` | `t0 = x + 1` / `x = t0` |
| Pós-incremento | `x++;` | `t0 = x` / `t1 = x + 1` / `x = t1` |

### 3.2. Controle de Fluxo

| Cenário | Construção C-- | Estrutura TAC esperada |
|---|---|---|
| `if` simples | `if (x > 0) { y = 1; }` | `JUMPF` + `LABEL` |
| `if/else` | `if (x > 0) { ... } else { ... }` | `JUMPF` + `JUMP` + 2 `LABEL`s |
| `while` | `while (i < n) { ... }` | `LABEL` + `JUMPF` + `JUMP` |
| `for` | `for (int k = 0; k < 10; k++) { ... }` | equivalente ao `while` |

### 3.3. Funções

| Cenário | Construção C-- | Estrutura TAC esperada |
|---|---|---|
| Definição sem parâmetros | `int f() { return 1; }` | `FUNC_BEGIN` + `RETURN` + `FUNC_END` |
| Chamada com argumentos | `int r = soma(a, b);` | `PARAM a` + `PARAM b` + `CALL soma, 2` |
| Parâmetros recebidos | `int soma(int a, int b)` | `GET_PARAM 0` + `GET_PARAM 1` |

---

## 4. Como Executar (quando implementado)

```bash
cd build && ./testes --gtest_filter=TACTest.*
```
