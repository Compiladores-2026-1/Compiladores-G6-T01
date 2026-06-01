# Geração de Código Intermediário (C-- Strict para TAC)

## 1. O que é o TAC

O **TAC** (*Three-Address Code*, ou Código de Três Endereços) é a representação intermediária usada pelo compilador entre a análise semântica e a geração de Assembly. Cada instrução TAC possui no máximo três operandos: um resultado e até dois argumentos.

```
resultado = arg1 OPERADOR arg2
```

Essa representação simplifica a geração de código final, pois cada instrução TAC mapeia diretamente para um pequeno conjunto de instruções Assembly.

---

## 2. Estrutura de uma Instrução TAC

No compilador, cada instrução TAC é representada pela struct:

```cpp
struct TACInstruction {
    TACOp       op;    // Tipo da operação
    std::string res;   // Resultado ou destino
    std::string arg1;  // Primeiro argumento
    std::string arg2;  // Segundo argumento
};
```

Os campos `res`, `arg1` e `arg2` são strings que podem conter:

- **Variáveis do programa:** `x`, `resultado`, `i`
- **Temporárias geradas:** `t0`, `t1`, `t2`, ... (incrementadas automaticamente)
- **Labels de controle:** `L0`, `L1`, `L2`, ... (incrementadas automaticamente)
- **Literais:** `5`, `3.14`, `true`
- **Nomes de funções:** `soma`, `main`

---

## 3. Conjunto de Instruções Suportadas

### 3.1. Tabela Completa

| Código (`TACOp`) | Sintaxe TAC | Operação | Exemplo |
|---|---|---|---|
| `ADD` | `res = arg1 + arg2` | Adição | `t0 = x + 5` |
| `SUB` | `res = arg1 - arg2` | Subtração | `t1 = a - b` |
| `MUL` | `res = arg1 * arg2` | Multiplicação | `t2 = 3 * c` |
| `DIV` | `res = arg1 / arg2` | Divisão inteira | `t3 = x / 2` |
| `MOD` | `res = arg1 % arg2` | Módulo | `t4 = y % 10` |
| `EQ` | `res = arg1 == arg2` | Igualdade | `t5 = x == 0` |
| `NEQ` | `res = arg1 != arg2` | Diferença | `t6 = a != b` |
| `LT` | `res = arg1 < arg2` | Menor que | `t7 = i < n` |
| `GT` | `res = arg1 > arg2` | Maior que | `t8 = x > 100` |
| `LE` | `res = arg1 <= arg2` | Menor ou igual | `t9 = a <= 50` |
| `GE` | `res = arg1 >= arg2` | Maior ou igual | `t10 = b >= 10` |
| `AND` | `res = arg1 && arg2` | E lógico | `t11 = p && q` |
| `OR` | `res = arg1 \|\| arg2` | Ou lógico | `t12 = r \|\| s` |
| `ASSIGN` | `res = arg1` | Atribuição simples | `x = t0` |
| `JUMP` | `goto label` | Salto incondicional | `goto L5` |
| `JUMPF` | `ifFalse arg1 goto label` | Salto condicional | `ifFalse t0 goto L3` |
| `LABEL` | `label:` | Marcador de rótulo | `L0:` |
| `CALL` | `res = call func, n` | Chamada de função | `t13 = call soma, 2` |
| `PARAM` | `param arg1` | Passagem de argumento | `param x` |
| `GET_PARAM` | `res = get_param idx` | Recebimento de parâmetro | `a = get_param 0` |
| `RETURN` | `return [arg1]` | Retorno de função | `return t0` |
| `FUNC_BEGIN` | `.begin_func nome` | Início de função | `.begin_func main` |
| `FUNC_END` | `.end_func nome` | Fim de função | `.end_func main` |

---

### 3.2. Grupos por Categoria

#### Aritméticas
Operações binárias sobre inteiros. O resultado é armazenado em uma variável temporária.

```
t0 = a + b
t1 = t0 * c
t2 = t1 / 2
t3 = t2 % 10
```

#### Relacionais
Produzem valor booleano (`0` ou `1`) armazenado em temporária. Usadas como condição para `JUMPF`.

```
t4 = x == 0
t5 = i < n
t6 = a != b
```

#### Lógicas
Operações `and` / `or` da linguagem. Os operandos são booleanos (resultado de relacionais ou variáveis bool).

```
t7 = t4 && t5
t8 = t6 || t7
```

#### Controle de fluxo
Combinam `LABEL`, `JUMP` e `JUMPF` para implementar `if`, `while` e `for`.

```
L0:
  t0 = i < n
  ifFalse t0 goto L1
  ...
  goto L0
L1:
```

#### Funções
Sequência de instruções para chamar e declarar funções:

```
param x           ← empilha argumento
param y
t0 = call soma, 2 ← chama função com 2 args, resultado em t0
```

```
.begin_func soma  ← início da função
  a = get_param 0 ← recebe 1º parâmetro
  b = get_param 1 ← recebe 2º parâmetro
  t0 = a + b
  return t0
.end_func soma
```

---

## 4. Exemplos de Conversão C-- Strict → TAC

### 4.1. Expressão Aritmética

```c
int resultado = (a + b) * (c - d);
```

```
t0 = a + b
t1 = c - d
t2 = t0 * t1
resultado = t2
```

### 4.2. Condicional `if / else`

```c
if (x > 0) {
    y = 1;
} else {
    y = 0;
}
```

```
t0 = x > 0
ifFalse t0 goto L0
  y = 1
  goto L1
L0:
  y = 0
L1:
```

### 4.3. Loop `while`

```c
while (i < n) {
    i = i + 1;
}
```

```
L0:
  t0 = i < n
  ifFalse t0 goto L1
  t1 = i + 1
  i = t1
  goto L0
L1:
```

### 4.4. Loop `for`

```c
for (int k = 0; k < 10; k++) {
    soma = soma + k;
}
```

```
  k = 0
L0:
  t0 = k < 10
  ifFalse t0 goto L1
  t1 = soma + k
  soma = t1
  t2 = k
  t3 = k + 1
  k = t3
  goto L0
L1:
```

### 4.5. Operadores Unários

| Expressão C-- | TAC gerado |
|---|---|
| `++x` (prefixo) | `t0 = x + 1` / `x = t0` |
| `--x` (prefixo) | `t0 = x - 1` / `x = t0` |
| `x++` (pós-fixo) | `t0 = x` / `t1 = x + 1` / `x = t1` (resultado = `t0`) |
| `x--` (pós-fixo) | `t0 = x` / `t1 = x - 1` / `x = t1` (resultado = `t0`) |
| `-x` (negação) | `t0 = 0 - x` |
| `!x` / `not x` | `t0 = x == 0` |

### 4.6. Chamada de Função

```c
int r = soma(a, b);
```

```
param a
param b
t0 = call soma, 2
r = t0
```

---

## 5. Variáveis Temporárias e Labels

O gerador de TAC mantém dois contadores globais incrementados automaticamente:

| Tipo | Formato | Exemplo |
|---|---|---|
| Temporária | `t` + número | `t0`, `t1`, `t2`, ... |
| Label | `L` + número | `L0`, `L1`, `L2`, ... |

Temporárias nunca são reutilizadas dentro de uma mesma função, garantindo que cada cálculo intermediário tenha seu próprio registrador virtual.
