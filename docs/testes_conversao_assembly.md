# Testes de Geração de Código Final (Assembly)

> **Status:** Esta fase ainda não foi implementada. Os testes serão criados quando a geração de Assembly estiver disponível. Consulte [Implementação da Geração de Código Final](implementacao_conversao_assembly.md) para o estado atual.

---

## 1. Objetivo

Os testes desta fase verificam que o código Assembly x86-64 gerado é sintaticamente correto, montável pelo GCC e produz o comportamento esperado em execução.

---

## 2. Estratégia de Teste

A estratégia combina dois níveis:

### 2.1. Testes de Estrutura (GoogleTest)

Verificam o texto Assembly gerado sem compilar ou executar:

- Presença de diretivas obrigatórias (`.text`, `.globl main`)
- Emissão correta do prólogo/epílogo de função (`push rbp`, `mov rbp, rsp`, `pop rbp`, `ret`)
- Uso correto dos registradores de argumento (`rdi`, `rsi`, etc.) para chamadas de função
- Labels de controle de fluxo gerados (`L0:`, `L1:`, ...)

```cpp
TEST(AssemblyTest, PrologoEpilogoFuncao) {
    // Compila "int main() { return 0; }"
    // Verifica que o assembly contém:
    EXPECT_TRUE(assembly.contains("push %rbp"));
    EXPECT_TRUE(assembly.contains("mov %rsp, %rbp"));
    EXPECT_TRUE(assembly.contains("pop %rbp"));
    EXPECT_TRUE(assembly.contains("ret"));
}
```

### 2.2. Testes de Execução (integração)

Para cada programa de teste `.cmm`:

1. Gera o Assembly com `./compilador --codegen programa.cmm > programa.s`
2. Monta com `gcc -o programa programa.s`
3. Executa e verifica o código de saída (`$?`)

```bash
./compilador --codegen tests/assembly/retorna_zero.cmm > /tmp/out.s
gcc -o /tmp/prog /tmp/out.s
/tmp/prog
echo $?   # Esperado: 0
```

---

## 3. Cenários Planejados

### 3.1. Programas mínimos

| Arquivo de teste | Programa C-- | Comportamento esperado |
|---|---|---|
| `retorna_zero.cmm` | `int main() { return 0; }` | Exit code 0 |
| `retorna_constante.cmm` | `int main() { return 42; }` | Exit code 42 |

### 3.2. Operações aritméticas

| Arquivo de teste | Descrição | Saída esperada |
|---|---|---|
| `aritmetica_basica.cmm` | `return 3 + 4;` | Exit code 7 |
| `precedencia.cmm` | `return 2 + 3 * 4;` | Exit code 14 |

### 3.3. Controle de fluxo

| Arquivo de teste | Descrição | Saída esperada |
|---|---|---|
| `if_simples.cmm` | `if (1 > 0) return 1; return 0;` | Exit code 1 |
| `while_contador.cmm` | Loop que conta de 0 a 4 | Exit code 4 |

### 3.4. Funções

| Arquivo de teste | Descrição | Saída esperada |
|---|---|---|
| `chamada_funcao.cmm` | `return soma(3, 4);` | Exit code 7 |
| `recursao.cmm` | `return fatorial(5);` | Exit code 120 (mod 256) |

---

## 4. Como Executar (quando implementado)

```bash
# Testes unitários de estrutura
cd build && ./testes --gtest_filter=AssemblyTest.*

# Testes de execução
cd build && ctest -R assembly --output-on-failure
```
