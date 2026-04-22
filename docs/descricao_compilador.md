# Descrição Compilador para C-- Strict para x86-64

## 1. Visão Geral do Compilador

O compilador para **C-- Strict** é um sistema de tradução de código-fonte que transforma um subconjunto simplificado da linguagem C em código **Assembly x86-64**. O projeto foi desenvolvido como ferramenta educacional para o estudo de compiladores, integrando as principais fases de processamento de linguagens de programação.

### 1.1. Objetivo Principal

Compilar programas escritos em C-- Strict para código de máquina executável, preservando a semântica da linguagem através de transformações sistemáticas em cada fase do compilador.

### 1.2. Arquitetura Geral

O compilador segue a arquitetura clássica em fases:

```
┌─────────────────────────────────────────────────────────────┐
│                    Código-Fonte C-- Strict                  │
└────────────────────────┬────────────────────────────────────┘
                         │
        ┌────────────────▼────────────────┐
        │    Análise Léxica (Flex)        │ → Tokens
        └────────────────┬────────────────┘
                         │
        ┌────────────────▼────────────────┐
        │   Análise Sintática (Bison)     │ → AST
        └────────────────┬────────────────┘
                         │
        ┌────────────────▼────────────────┐
        │  Análise Semântica (C++)        │ → AST Validada
        └────────────────┬────────────────┘
                         │
        ┌────────────────▼────────────────┐
        │  Geração de Código Intermediário│ → TAC
        │         (C++ backend)           │
        └────────────────┬────────────────┘
                         │
        ┌────────────────▼────────────────┐
        │    Geração de Código Final      │ → Assembly x86-64
        │         (C++ backend)           │
        └────────────────┬────────────────┘
                         │
        ┌────────────────▼────────────────┐
        │  Montagem e Linkagem (GCC)      │ → Executável
        └────────────────┬────────────────┘
                         │
        ┌────────────────▼────────────────┐
        │       Código de Máquina         │
        └─────────────────────────────────┘
```

---

## 2. Componentes Principais

### 2.1. Frontend do Compilador

#### 2.1.1. Análise Léxica (Flex)

A análise léxica é a primeira fase do compilador, responsável por transformar o código-fonte em uma sequência de *tokens*.

**Responsabilidades:**
- Ler o arquivo de entrada caractere por caractere
- Reconhecer palavras-chave, identificadores, literais e operadores
- Ignorar comentários e espaços em branco
- Gerar tokens com seus respectivos tipos e valores

**Saída:** Lista de tokens com informações de localização (linha e coluna)

**Arquivo correspondente:** `lexer/lexer.l`

#### 2.1.2. Análise Sintática (Bison)

A análise sintática valida a estrutura gramatical do programa, construindo uma árvore de sintaxe abstrata (AST).

**Responsabilidades:**
- Validar que os tokens seguem a gramática BNF da linguagem
- Detectar erros sintáticos e reportar sua localização
- Construir a representação interna (AST) do programa
- Implementar mecanismos de recuperação de erros

**Saída:** Árvore de Sintaxe Abstrata (AST)

**Arquivo correspondente:** `parser/parser.y`

### 2.2. Backend do Compilador (C++)

O backend em C++ implementa as fases semânticas e de geração de código, operando diretamente na AST.

#### 2.2.1. Análise Semântica

**Responsabilidades:**
- Verificar a compatibilidade de tipos em operações
- Validar escopos e declarações de variáveis
- Detectar usos de variáveis não-declaradas
- Implementar tabela de símbolos

#### 2.2.2. Geração de Código Intermediário (TAC - Three Address Code)

**Responsabilidades:**
- Converter a AST em uma representação intermediária de três endereços
- Simplificar operações complexas em operações atômicas
- Preparar o código para otimizações posteriores
- Facilitar a tradução para linguagens de máquina distintas

**Exemplo de conversão:**

```c
// C-- Strict
int resultado = (a + b) * (c - d);

// TAC
temp1 = a + b
temp2 = c - d
resultado = temp1 * temp2
```

#### 2.2.3. Geração de Código Assembly x86-64

**Responsabilidades:**
- Traduzir operações TAC em instruções x86-64
- Gerenciar o espaço de pilha (stack) para variáveis locais
- Implementar convenções de chamada (calling convention)
- Otimizar o uso de registradores
- Gerar código pronto para linkagem

---

## 3. Arquitetura x86-64 e Convenções

### 3.1. Registradores Disponíveis

O compilador utiliza registradores de propósito geral da arquitetura x86-64:

| Registrador | Tamanho | Função | Uso no Compilador |
|-------------|--------|--------|-------------------|
| `rax` | 64-bit | Acumulador | Operações gerais, valor de retorno |
| `rcx` | 64-bit | Contador | Loop counter, operandos shif |
| `rdx` | 64-bit | Dados | Operações gerais |
| `rsi` | 64-bit | Source Index | Operações gerais |
| `rdi` | 64-bit | Destination Index | Operações gerais |
| `rbp` | 64-bit | Frame Base | Ponteiro de stack frame |
| `rsp` | 64-bit | Stack Pointer | Topo da pilha |
| `r8-r15` | 64-bit | General Purpose | Operações gerais |

### 3.2. Segmentação de Memória

O código gerado utiliza três segmentos principais:

```asm
section .data        # Dados inicializados (variáveis globais)
section .bss         # Dados não inicializados (variáveis não inicializadas)
section .text        # Código executável
```

### 3.3. Convenção de Chamada (Calling Convention)

O compilador implementa a convenção **System V AMD64 ABI**:

- **Argumentos passados por registrador:** `rdi`, `rsi`, `rdx`, `rcx`, `r8`, `r9`
- **Valor de retorno:** `rax`
- **Registradores preservados:** `rbx`, `rbp`, `r12-r15`
- **Registradores temporários:** `rax`, `rcx`, `rdx`, `rsi`, `rdi`, `r8-r11`
- **Stack alignment:** O stack deve estar alinhado a 16 bytes

---

## 4. Fluxo de Compilação

### 4.1. Etapas Sequenciais

```
Código Fonte
    │
    ├─► Preprocessing (comentários e espaços)
    │
    ├─► Análise Léxica (Flex)
    │   └─► Tokenização
    │
    ├─► Análise Sintática (Bison)
    │   └─► Construção de AST
    │
    ├─► Análise Semântica (C++)
    │   └─► Verificação de tipos e escopos
    │
    ├─► Geração de TAC (C++)
    │   └─► Código intermediário
    │
    ├─► Geração de Assembly (C++)
    │   └─► Tradução para x86-64
    │
    ├─► Montagem (GCC Assembler)
    │   └─► Arquivo objeto (.o)
    │
    ├─► Linkagem (GCC Linker)
    │   └─► Executável final
    │
    └─► Executável
```

### 4.2. Exemplo Prático Completo

Dado um programa simples em C-- Strict:

```c
int main() {
    int x = 5;
    int y = 3;
    int z = x + y;
    return z;
}
```

**Tokens gerados pela Análise Léxica:**
```
INT MAIN ( ) { INT x = INT_LITERAL(5) ; INT y = INT_LITERAL(3) ; INT z = x + y ; RETURN z ; }
```

**AST gerado pela Análise Sintática:**
```
Program
├─ Function: main
   └─ Body
      ├─ Declaration: x = 5
      ├─ Declaration: y = 3
      ├─ Declaration: z = (x + y)
      └─ Return: z
```

**TAC gerado:**
```
temp1 = 5
x = temp1
temp2 = 3
y = temp2
temp3 = x
temp4 = y
temp5 = temp3 + temp4
z = temp5
return z
```

**Assembly x86-64 gerado:**
```asm
.global main
main:
    push rbp
    mov rbp, rsp
    mov dword [rbp-4], 5      # x = 5
    mov dword [rbp-8], 3      # y = 3
    mov eax, dword [rbp-4]    # temp3 = x
    mov ecx, dword [rbp-8]    # temp4 = y
    add eax, ecx              # temp5 = temp3 + temp4
    mov dword [rbp-12], eax   # z = temp5
    mov eax, dword [rbp-12]   # return z
    pop rbp
    ret
```

---

## 5. Tabela de Símbolos e Escopos

### 5.1. Escopo Global

Variáveis declaradas fora de qualquer função possuem escopo global e são alocadas no segmento `.data` ou `.bss`.

```c
int contador_global = 0;  // Escopo global
int valor_nao_init;       // Escopo global, não inicializado

int main() {
    // contador_global é acessível aqui
}
```

### 5.2. Escopo Local

Variáveis declaradas dentro de uma função possuem escopo local, limitado ao bloco onde foram declaradas. São alocadas na pilha (stack).

```c
int main() {
    int variavel_local = 10;  // Escopo local à função main
    
    if (variavel_local > 0) {
        int temp = 5;         // Escopo local ao bloco if
    }
    // temp não é acessível aqui
}
```

### 5.3. Tabela de Símbolos

A tabela de símbolos é mantida durante toda a compilação para:
- Rastrear declarações de variáveis
- Verificar redeclarações
- Associar identificadores a tipos
- Armazenar informações de endereço de memória

---

## 6. Geração e Otimização de Código

### 6.1. Alocação de Variáveis na Stack

O compilador utiliza a pilha (stack) para armazenar variáveis locais. O espaço é alocado dinamicamente durante a execução, seguindo a convenção x86-64.

**Layout de Stack Frame:**

```
[rbp + 8]     ← Endereço de retorno
[rbp]         ← Base do frame anterior
[rbp - 8]     ← Primeira variável local
[rbp - 16]    ← Segunda variável local
...
[rsp]         ← Topo do stack (stack pointer)
```

### 6.2. Operações Aritméticas

Operações aritméticas são traduzidas diretamente para instruções x86-64:

| C-- Strict | Assembly x86-64 |
|-----------|-----------------|
| `a + b` | `add rax, rbx` |
| `a - b` | `sub rax, rbx` |
| `a * b` | `imul rax, rbx` |
| `a / b` | `idiv rbx` |
| `a % b` | `idiv rbx` (usa `rdx`) |

### 6.3. Operações Lógicas e Comparações

```asm
cmp rax, rbx        # Compara rax com rbx
je label            # Jump if Equal
jne label           # Jump if Not Equal
jl label            # Jump if Less
jle label           # Jump if Less or Equal
jg label            # Jump if Greater
jge label           # Jump if Greater or Equal
```

---

## 7. Tratamento de Erros e Mensagens

O compilador reporta erros em diferentes fases com informações precisas:

### 7.1. Erros Léxicos

Caracteres não reconhecidos ou inválidos.

```
Erro Léxico - Linha 5: Caractere inválido '@'
```

### 7.2. Erros Sintáticos

Violações da gramática da linguagem.

```
Erro Sintático - Linha 10: Esperado ';' após declaração
```

### 7.3. Erros Semânticos

Incompatibilidades de tipo, escopos inválidos, etc.

```
Erro Semântico - Linha 15: Variável 'x' não foi declarada
Erro Semântico - Linha 20: Incompatibilidade de tipos em atribuição
```

---

## 8. Otimizações Implementadas

### 8.1. Alocação de Registradores

O backend tenta alocar variáveis em registradores quando possível, reduzindo acessos à memória.

### 8.2. Eliminação de Código Morto

Código que nunca será executado é detectado e removido durante a geração de código intermediário.

### 8.3. Folding de Constantes

Expressões com operandos constantes são pré-avaliadas em tempo de compilação:

```c
// Entrada
int resultado = 5 + 3;

// Saída (já folded)
int resultado = 8;
```

---

## 9. Ferramentas e Dependências

### 9.1. Ferramentas de Build

- **Flex:** Análise Léxica (v2.6+)
- **Bison:** Análise Sintática (v3.0+)
- **GCC:** Compilação e Linkagem (v9.0+)
- **CMake:** Sistema de Build (v3.10+)

### 9.2. Bibliotecas

- **C++ Standard Library:** Estruturas de dados e algoritmos

### 9.3. Ambiente de Desenvolvimento

- **Sistema Operacional:** Linux/macOS
- **Arquitetura:** x86-64

---

## 10. Limitações e Restrições

### 10.1. Características Não Suportadas

- **Ponteiros:** Não há suporte para pointers ou referências
- **Arrays:** Estruturas de dados complexas não são suportadas
- **Alocação Dinâmica:** Sem heap ou `malloc`
- **Objetos e Classes:** Nenhum suporte a POO
- **Funções Recursivas:** Não há suporte a chamadas recursivas
- **Strings:** Apenas caracteres individuais
- **Bibliotecas Externas:** Sem linkagem com bibliotecas do SO

### 10.2. Restrições de Tipo

- Tipos básicos: `int`, `float`, `char`, `bool`
- Sem conversão implícita entre tipos incompatíveis
- Operações sobre tipos devem ser explícitas

---

## 11. Fluxo de Entrada e Saída

### 11.1. Entrada

O compilador aceita um arquivo de código-fonte C-- Strict como entrada:

```bash
./compilador programa.cmm
```

### 11.2. Saída

O compilador gera:
1. **Arquivo Assembly:** `programa.s` (código intermediário legível)
2. **Arquivo Objeto:** `programa.o` (gerado pelo assembler)
3. **Executável:** `programa` (resultado final após linkagem)

---

## 12. Conclusão

O compilador para C-- Strict representa uma implementação educacional completa de um sistema de tradução, demonstrando os conceitos fundamentais da engenharia de compiladores. Através de suas fases bem-definidas e sua orientação para x86-64, oferece uma base sólida para compreender como linguagens de alto nível são transformadas em código de máquina executável.
