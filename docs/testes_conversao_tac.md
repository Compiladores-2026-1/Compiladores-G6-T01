# Testes de Geração de Código Intermediário (TAC)

> **Status:** Implementados. A fase de TAC é validada por **scripts de comparação** que conferem o TAC gerado pelo compilador contra arquivos de saída esperada (*golden files*). Os testes rodam via CTest e, portanto, também na [Integração Contínua](integracao_continua.md).

---

## 1. Objetivo

Garantir que, para cada construção da linguagem C-- Strict, o compilador gere exatamente a sequência de instruções TAC esperada — e que qualquer mudança não intencional no gerador seja detectada automaticamente.

---

## 2. Estratégia de Teste (golden files)

A abordagem é de **teste por comparação de saída**:

1. Cada caso de teste é um arquivo `.cmm` em `tests/tac/casos/`.
2. Para cada caso há um arquivo de saída esperada em `tests/tac/esperado/<nome>.tac.expected`.
3. O script `tests/tac/run_tac_tests.sh` roda o compilador em modo `--tac` sobre cada caso, captura a saída e a compara, via `diff`, com o arquivo esperado.
4. O script retorna código de erro (e imprime o `diff`) na primeira divergência; retorna `0` se todos os casos baterem.

Esquema:

```
casos/01_expressao.cmm  --(./compilador --tac)-->  saida gerada
                                                       |
                                                     diff
                                                       |
esperado/01_expressao.tac.expected  --------------------
```

---

## 3. Cenários Cobertos

| Caso | Arquivo | Construção exercitada | TAC relevante |
|---|---|---|---|
| Expressão aritmética | `01_expressao.cmm` | precedência e temporárias | `t0 = a + b` / `t1 = t0 * a` |
| Controle de fluxo | `02_if_else.cmm` | `if/else` | `ifFalse t0 goto L0` / `goto L1` / labels |
| Laço | `03_while.cmm` | `while` | label de início / `ifFalse` / `goto` de retorno |
| Funções | `04_funcao.cmm` | definição e chamada | `param` / `t1 = call soma, 2` |

Exemplo do esperado para `01_expressao.cmm`:

```text
======================= TAC ========================

.begin_func main
  a = 2
  b = 3
  t0 = a + b
  t1 = t0 * a
  c = t1
  return c
.end_func main
========================================================
```

---

## 4. Como Executar

Via CTest (forma recomendada, mesma usada na CI):

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
ctest --test-dir build -R tac_golden --output-on-failure
```

Ou diretamente pelo script, passando o binário do compilador:

```bash
./tests/tac/run_tac_tests.sh ./build/compilador
```

---

## 5. Como Adicionar um Novo Caso

1. Crie `tests/tac/casos/<nome>.cmm` com o trecho C-- Strict a testar.
2. Gere a saída esperada e **revise-a** antes de fixá-la como baseline:
   ```bash
   ./build/compilador --tac tests/tac/casos/<nome>.cmm -o tests/tac/esperado/<nome>.tac.expected
   ```
3. Rode `ctest -R tac_golden` para confirmar que o novo caso passa.
