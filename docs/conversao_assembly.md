# Geração de Código Final (TAC para Assembly x86-64)

> **Status:** Esta fase ainda não foi implementada. O código referente aos modos `--tac`, `--opt` e `--codegen` está comentado em `src/main.cpp` (linhas 149–157). Este documento descreve a arquitetura planejada com base nos demais documentos do projeto.

---

## 1. Objetivo

A geração de código final traduz cada instrução TAC (Código de Três Endereços) para instruções Assembly x86-64 que podem ser montadas pelo GCC/GAS e executadas diretamente na máquina alvo.

---

## 2. Arquitetura Alvo: x86-64

O compilador tem como alvo a arquitetura **x86-64** (AMD64/Intel 64), gerada para ser montada com o **GCC** em modo Linux.

### Registradores Gerais (uso previsto)

| Registrador | Uso convencional (System V AMD64 ABI)         |
|-------------|-----------------------------------------------|
| `rax`       | Valor de retorno de função; acumulador         |
| `rdi`, `rsi`, `rdx`, `rcx`, `r8`, `r9` | Argumentos de função (1º ao 6º) |
| `rsp`       | Ponteiro de pilha (*stack pointer*)           |
| `rbp`       | Ponteiro de base do frame (*frame pointer*)   |
| `r10`–`r15` | Registradores temporários                     |

Como C-- Strict não usa alocação dinâmica, toda a memória de variáveis locais é gerenciada pela **pilha**.

---

## 3. Mapeamento TAC → Assembly

A tradução segue um mapeamento direto de cada `TACOp` para instruções Assembly. Abaixo, a correspondência planejada:

### 3.1. Operações Aritméticas

| TAC                  | Assembly x86-64 (exemplo)          |
|----------------------|------------------------------------|
| `t0 = a + b`         | `mov rax, [a]` / `add rax, [b]` / `mov [t0], rax` |
| `t0 = a - b`         | `mov rax, [a]` / `sub rax, [b]` / `mov [t0], rax` |
| `t0 = a * b`         | `mov rax, [a]` / `imul rax, [b]` / `mov [t0], rax` |
| `t0 = a / b`         | `mov rax, [a]` / `cqo` / `idiv qword [b]` / `mov [t0], rax` |
| `t0 = a % b`         | `mov rax, [a]` / `cqo` / `idiv qword [b]` / `mov [t0], rdx` |

### 3.2. Operações Relacionais

Produzem `0` ou `1` (booleano). Usam instruções de comparação e `setcc`:

| TAC              | Assembly x86-64                                    |
|------------------|----------------------------------------------------|
| `t0 = a == b`    | `cmp [a], [b]` / `sete al` / `movzx [t0], al`     |
| `t0 = a != b`    | `cmp [a], [b]` / `setne al` / `movzx [t0], al`    |
| `t0 = a < b`     | `cmp [a], [b]` / `setl al` / `movzx [t0], al`     |
| `t0 = a > b`     | `cmp [a], [b]` / `setg al` / `movzx [t0], al`     |
| `t0 = a <= b`    | `cmp [a], [b]` / `setle al` / `movzx [t0], al`    |
| `t0 = a >= b`    | `cmp [a], [b]` / `setge al` / `movzx [t0], al`    |

### 3.3. Controle de Fluxo

| TAC                         | Assembly x86-64       |
|-----------------------------|-----------------------|
| `goto L3`                   | `jmp L3`              |
| `ifFalse t0 goto L3`        | `cmp [t0], 0` / `je L3` |
| `L3:`                       | `L3:`                 |

### 3.4. Funções

| TAC                        | Assembly x86-64 (System V ABI)                |
|----------------------------|-----------------------------------------------|
| `.begin_func soma`         | `soma:` / `push rbp` / `mov rbp, rsp`         |
| `.end_func soma`           | `pop rbp` / `ret`                             |
| `param x`                  | Coloca argumento em `rdi`, `rsi`, etc.        |
| `t0 = call soma, 2`        | `call soma` / `mov [t0], rax`                 |
| `return t0`                | `mov rax, [t0]` / `pop rbp` / `ret`           |
| `a = get_param 0`          | `mov [a], rdi`                                |

### 3.5. Atribuição

| TAC            | Assembly x86-64                       |
|----------------|---------------------------------------|
| `x = t0`       | `mov rax, [t0]` / `mov [x], rax`      |

---

## 4. Layout do Frame de Pilha

Cada função alocará um frame fixo na pilha para suas variáveis locais:

```
     alta  +------------------+
           |  arg antigo      |  ← caller empilha args além do 6º
           +------------------+
           |  ret addr        |  ← call empilha endereço de retorno
           +------------------+
rbp →      |  rbp salvo       |  ← push rbp
           +------------------+
           |  var local 1     |  ← [rbp - 8]
           +------------------+
           |  var local 2     |  ← [rbp - 16]
           +------------------+
rsp →      |  ...             |
     baixa +------------------+
```

---

## 5. Exemplo Completo

Programa C-- Strict:
```c
int soma(int a, int b) {
    return a + b;
}

int main() {
    int r = soma(3, 4);
    return 0;
}
```

TAC intermediário (ver [Geração de Código Intermediário](conversao_tac.md)):
```
.begin_func soma
  a = get_param 0
  b = get_param 1
  t0 = a + b
  return t0
.end_func soma

.begin_func main
  param 3
  param 4
  t0 = call soma, 2
  r = t0
  return 0
.end_func main
```

Assembly x86-64 planejado (AT&T syntax):
```asm
    .text
    .globl main

soma:
    pushq   %rbp
    movq    %rsp, %rbp
    subq    $16, %rsp
    movq    %rdi, -8(%rbp)      # a = get_param 0
    movq    %rsi, -16(%rbp)     # b = get_param 1
    movq    -8(%rbp), %rax
    addq    -16(%rbp), %rax     # t0 = a + b
    popq    %rbp
    ret                         # return t0

main:
    pushq   %rbp
    movq    %rsp, %rbp
    subq    $8, %rsp
    movq    $3, %rdi            # param 3
    movq    $4, %rsi            # param 4
    call    soma                # t0 = call soma, 2
    movq    %rax, -8(%rbp)      # r = t0
    movq    $0, %rax
    popq    %rbp
    ret                         # return 0
```

---

## 6. Integração com o CMake / GCC

A fase de geração planejada produzirá um arquivo `.s` que será montado e linkado com GCC:

```bash
gcc -o programa programa.s
```
