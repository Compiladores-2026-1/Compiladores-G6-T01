# Arquitetura Técnica e Regras de Escopo

## 1. Visão Geral da Arquitetura

O compilador para C-- Strict é organizado em módulos independentes que se comunicam através de estruturas de dados bem definidas. Cada módulo corresponde a uma fase da compilação e é implementado em seu próprio diretório dentro de `src/`.

```
src/
├── lexer/          ← Análise léxica (Flex)
├── parser/         ← Análise sintática (Bison)
├── ast/            ← Definição e impressão da AST
├── symtable/       ← Tabela de símbolos com escopos
├── semantic/       ← Análise semântica (Visitor)
├── runners/        ← Ponto de entrada para cada fase
└── main.cpp        ← Entrada do compilador
```

---

## 2. Módulos e Responsabilidades

### 2.1. Análise Léxica — `src/lexer/lexer.l`

Implementada com **Flex**. Lê o código-fonte caractere a caractere e produz uma sequência de tokens reconhecidos por expressões regulares.

**Entrada:** arquivo `.cmm` (código-fonte C-- Strict)  
**Saída:** stream de tokens para o parser (via `yylex()`)

**Tokens reconhecidos:**

| Categoria | Exemplos |
|---|---|
| Palavras-chave | `int`, `float`, `bool`, `if`, `while`, `for`, `return` |
| Identificadores | `x`, `soma`, `resultado` |
| Literais | `42`, `3.14f`, `'a'`, `true` |
| Operadores aritméticos | `+`, `-`, `*`, `/`, `%` |
| Operadores relacionais | `==`, `!=`, `<`, `>`, `<=`, `>=` |
| Operadores lógicos | `and`, `or`, `not` |
| Atribuição composta | `+=`, `-=`, `*=`, `/=`, `%=` |
| Delimitadores | `(`, `)`, `{`, `}`, `;`, `,` |

---

### 2.2. Análise Sintática — `src/parser/parser.y`

Implementada com **Bison** (LALR(1)). Valida a estrutura gramatical e constrói a **Árvore de Sintaxe Abstrata (AST)**.

**Entrada:** tokens produzidos pelo lexer  
**Saída:** ponteiro para a raiz da AST (`ProgramNode*`)

A gramática define a hierarquia de precedência de operadores (da menor para a maior):

| Precedência | Operadores |
|---|---|
| 1 (menor) | `or` |
| 2 | `and` |
| 3 | `==`, `!=`, `<`, `>`, `<=`, `>=` |
| 4 | `+`, `-` |
| 5 | `*`, `/`, `%` |
| 6 (maior) | `-` unário, `not`, `++`, `--` |

---

### 2.3. Árvore de Sintaxe Abstrata — `include/ast.hpp`

A AST é composta por nós que representam cada construção da linguagem. Todos implementam o padrão **Visitor**, expondo o método `accept(Visitor*)`.

| Nó | Representa |
|---|---|
| `ProgramNode` | Raiz do programa |
| `FuncDeclNode` | Declaração de função |
| `VarDeclNode` | Declaração de variável |
| `BlockNode` | Bloco de código `{ }` |
| `AssignNode` | Atribuição `x = expr` |
| `BinOpNode` | Operação binária `a + b` |
| `UnOpNode` | Operação unária `-x`, `++x` |
| `IfNode` | Condicional `if / else` |
| `WhileNode` | Loop `while` |
| `ForNode` | Loop `for` |
| `ReturnNode` | Instrução `return` |
| `FuncCallNode` | Chamada de função `f(args)` |
| `IdNode` | Uso de identificador |
| `LiteralNode` | Valor literal (`42`, `true`) |
| `BreakNode` | Instrução `break` |
| `ContinueNode` | Instrução `continue` |

---

### 2.4. Tabela de Símbolos — `src/symtable/`

Estrutura central para rastreamento de variáveis e funções durante a análise semântica. Implementada como uma **pilha de escopos**, onde cada escopo é um mapa de nome → símbolo.

```cpp
class SymbolTable {
    std::vector<std::unordered_map<std::string, Symbol>> scopes;
};
```

Cada símbolo armazena:

| Campo | Tipo | Descrição |
|---|---|---|
| `name` | `string` | Nome do identificador |
| `type_token` | `int` | Token do tipo (`INT`, `FLOAT`, etc.) |
| `category` | `SymbolCategory` | `VAR`, `FUNC` ou `PARAM` |
| `scope_level` | `int` | Nível de aninhamento onde foi declarado |

---

### 2.5. Análise Semântica — `src/semantic/`

Implementada como um **Visitor** que percorre a AST e usa a tabela de símbolos para verificar a consistência do programa.

**Verificações realizadas:**

- Redeclaração de variável no mesmo escopo
- Uso de variável não declarada
- Atribuição a uma função
- Chamada a identificador que não é função
- Chamada a função desconhecida
- Parâmetros duplicados em declaração de função

---

### 2.6. Padrão Visitor

Todas as fases que percorrem a AST (semântica, geração de TAC, impressão) implementam a interface `Visitor`:

```cpp
class Visitor {
public:
    virtual void visit(ProgramNode*)   = 0;
    virtual void visit(FuncDeclNode*)  = 0;
    virtual void visit(VarDeclNode*)   = 0;
    virtual void visit(BlockNode*)     = 0;
    virtual void visit(AssignNode*)    = 0;
    virtual void visit(BinOpNode*)     = 0;
    virtual void visit(UnOpNode*)      = 0;
    virtual void visit(IfNode*)        = 0;
    virtual void visit(WhileNode*)     = 0;
    virtual void visit(ForNode*)       = 0;
    virtual void visit(ReturnNode*)    = 0;
    virtual void visit(FuncCallNode*)  = 0;
    virtual void visit(IdNode*)        = 0;
    virtual void visit(LiteralNode*)   = 0;
    virtual void visit(BreakNode*)     = 0;
    virtual void visit(ContinueNode*)  = 0;
};
```

Isso permite adicionar novas fases sem modificar os nós da AST.

---

## 3. Fluxo de Dados Entre Módulos

```
lexer.l  ──(tokens)──▶  parser.y  ──(AST)──▶  SemanticVisitor
                                                      │
                                               SymbolTable
                                                      │
                                               TACVisitor ──(instruções TAC)──▶ CodeGenerator
                                                                                      │
                                                                               Assembly x86-64
```

Cada fase transforma sua entrada em uma representação mais próxima do código de máquina, sem retorno: o pipeline é unidirecional.

---

## 4. Regras de Escopo da Linguagem

### 4.1. Estrutura de Escopos

C-- Strict usa **escopo estático léxico**: o escopo de uma variável é determinado pela posição no código-fonte, não pelo fluxo de execução.

A tabela de símbolos mantém uma pilha de escopos. Ao entrar em um bloco, um novo escopo é empilhado; ao sair, ele é desempilhado e todas as variáveis declaradas nele são descartadas.

```
Nível 0 — Escopo global (funções globais e variáveis globais)
  Nível 1 — Corpo de função (parâmetros + variáveis locais)
    Nível 2 — Bloco interno (if, while, for, bloco anônimo)
      Nível 3 — Bloco mais profundo
```

### 4.2. Criação e Destruição de Escopos

| Construção | Abre escopo | Fecha escopo |
|---|---|---|
| Corpo de função `{ }` | sim | sim |
| Bloco `if` / `else` | sim | sim |
| Corpo de `while` | sim | sim |
| Cabeçalho + corpo de `for` | sim | sim |

O `for` é especial: o escopo abre **antes** da inicialização, de modo que a variável declarada em `for (int i = 0; ...)` existe apenas dentro do loop.

### 4.3. Busca de Símbolos (Lookup)

A busca de um identificador percorre a pilha de escopos **do mais interno para o mais externo**, retornando o primeiro símbolo encontrado:

```cpp
// Percorre da pilha do topo para a base
for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
    auto found = it->find(name);
    if (found != it->end())
        return &(found->second);
}
return nullptr;  // não encontrado em nenhum escopo
```

Isso significa que uma variável em escopo interno **oculta (shadowing)** uma variável de mesmo nome em escopo externo. A variável externa continua existindo mas fica inacessível enquanto a interna estiver no escopo.

### 4.4. Shadowing — Ocultamento de Variáveis

```c
int x = 10;

int main() {
    int x = 20;   // ← este x oculta o x global dentro da função
    if (true) {
        int x = 30; // ← este x oculta o x da função dentro do bloco
    }
    // aqui x == 20 novamente
}
```

O compilador não emite aviso ao ocorrer shadowing: o comportamento é aceito.

### 4.5. Redeclaração no Mesmo Escopo

Declarar o mesmo nome duas vezes **no mesmo escopo** é um erro semântico:

```c
int x = 1;
int x = 2;  // Erro: Redeclaração da variável 'x' no mesmo escopo.
```

A verificação usa `lookupCurrentScope`, que consulta apenas o escopo atual (topo da pilha), sem subir para escopos externos.

### 4.6. Escopo de Funções

Funções são registradas no **escopo global** (nível 0). Ao processar o corpo de uma função, dois escopos são abertos em sequência:

1. **Escopo dos parâmetros** — criado pelo `SemanticVisitor` ao visitar `FuncDeclNode`, onde os parâmetros formais são inseridos
2. **Escopo do corpo** — criado pelo `BlockNode` ao entrar no bloco `{ }` da função

```
Nível 0: soma (FUNC)
  Nível 1: a (PARAM), b (PARAM)   ← escopo de parâmetros
    Nível 2: resultado (VAR)       ← variáveis locais do bloco
```

### 4.7. Uso de Variável Antes da Declaração

Não é permitido usar uma variável antes de declará-la. O `SemanticVisitor` verifica cada uso de `IdNode` chamando `lookup`; se retornar `nullptr`, emite erro:

```c
int main() {
    y = 5;    // Erro: Uso de variável não declarada: 'y'.
    int y;
}
```

### 4.8. Resumo das Regras

| Regra | Comportamento |
|---|---|
| Tipo de escopo | Estático léxico |
| Variável antes da declaração | Erro semântico |
| Redeclaração no mesmo escopo | Erro semântico |
| Shadowing (escopo interno oculta externo) | Permitido, sem aviso |
| Funções declaradas em | Escopo global (nível 0) |
| Parâmetros formais | Escopo nível 1 (dentro da função) |
| Variáveis de loop `for` | Escopadas ao corpo do loop |
| Escopo de bloco `if` / `else` | Cada bloco tem seu próprio escopo |
