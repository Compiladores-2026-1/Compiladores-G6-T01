# Compilador C-- Strict

> Projeto da disciplina de **Compiladores** — Grupo 6, Trabalho 1

Um compilador para a linguagem **C-- Strict**, um subconjunto simplificado da linguagem C, desenvolvido com Flex, Bison e C++17.

---

## Sumário

- [Visão Geral](#visão-geral)
- [A Linguagem C-- Strict](#a-linguagem-c---strict)
- [Arquitetura do Compilador](#arquitetura-do-compilador)
- [Pré-requisitos](#pré-requisitos)
- [Como Compilar](#como-compilar)
- [Como Usar](#como-usar)
- [Estrutura do Projeto](#estrutura-do-projeto)
- [Testes](#testes)
- [Documentação](#documentação)

---

## Visão Geral

Este projeto implementa um compilador completo para a linguagem C-- Strict, seguindo as etapas clássicas de compilação:

1. **Análise Léxica** — tokenização do código-fonte via Flex
2. **Análise Sintática** — parsing via Bison (gramática LR)
3. **Construção de AST** — representação em árvore sintática abstrata com padrão Visitor
4. **Análise Semântica** — verificação de tipos e escopos com tabela de símbolos
5. **Geração de Código** — tradução final para x86-64 Assembly e compilação do executável binário

---

## A Linguagem C-- Strict

C-- Strict é um subconjunto simplificado de C com as seguintes características:

### Tipos de Dados

| Tipo     | Descrição                 |
| -------- | ------------------------- |
| `int`    | Inteiro                   |
| `float`  | Ponto flutuante simples   |
| `double` | Ponto flutuante duplo     |
| `char`   | Caractere                 |
| `bool`   | Booleano (`true`/`false`) |
| `void`   | Sem retorno (funções)     |

### Construções Suportadas

- Declaração de variáveis globais e locais (com inicialização opcional)
- Declaração e definição de funções (obrigatório ter `main()`)
- Comentários de linha (`//`) e de bloco (`/* */`)
- Estruturas de controle: `if/else`, `while`, `for`
- Comandos de desvio: `break`, `continue`, `return`
- Chamadas de função com argumentos

### Operadores

| Categoria       | Operadores                        |
| --------------- | --------------------------------- |
| Aritméticos     | `+`, `-`, `*`, `/`, `%`           |
| Relacionais     | `==`, `!=`, `<`, `>`, `<=`, `>=`  |
| Lógicos         | `and`, `or`, `not`, `!`           |
| Atribuição      | `=`, `+=`, `-=`, `*=`, `/=`, `%=` |
| Unários/Postfix | `++`, `--`, `-` (negação unária)  |

### Exemplo de Programa C-- Strict

```c
int fatorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * fatorial(n - 1);
}

int main() {
    int resultado = fatorial(5);
    return 0;
}
```

### Limitações

- Sem literais de string
- Sem alocação dinâmica de memória (sem `malloc`/`free`)
- Sem ponteiros
- Apenas memória em pilha (stack)

---

## Arquitetura do Compilador

```
Código-fonte (.cmm)
        │
        ▼
┌───────────────────┐
│  Análise Léxica   │  ← Flex (lexer.l)
│  (Tokenização)    │
└────────┬──────────┘
         │  tokens
         ▼
┌───────────────────┐
│ Análise Sintática │  ← Bison (parser.y)
│    (Parsing)      │
└────────┬──────────┘
         │  AST
         ▼
┌───────────────────┐
│  Análise Semântica│  ← SemanticVisitor + SymbolTable
│ (Tipos e Escopos) │
└────────┬──────────┘
         │  AST validada
         ▼
┌───────────────────┐
│ Geração de Código │  ← x86-64 Assembly (Concluído)
│   (Assembly)      │
└───────────────────┘
```

### Componentes Principais

| Componente           | Localização                   | Descrição                                          |
| -------------------- | ----------------------------- | -------------------------------------------------- |
| Analisador Léxico    | `src/lexer/lexer.l`           | Especificação Flex — reconhece tokens da linguagem |
| Analisador Sintático | `src/parser/parser.y`         | Gramática Bison — constrói a AST                   |
| AST                  | `src/ast/`, `include/ast.hpp` | 18 classes de nós com padrão Visitor               |
| Tabela de Símbolos   | `src/symtable/`               | Gerenciamento de escopos aninhados                 |
| Análise Semântica    | `src/semantic/`               | Verificação de tipos e uso de variáveis            |
| Runners              | `src/runners/`                | Implementam os modos de execução do compilador     |

---

## Pré-requisitos

- **CMake** 3.16 ou superior
- **GCC** ou **Clang** com suporte a C++17
- **Flex** (analisador léxico)
- **Bison** (analisador sintático)

### Instalação das dependências (Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install cmake g++ flex bison
```

### Instalação das dependências (macOS)

```bash
brew install cmake flex bison
```

> **Nota (macOS):** O Homebrew instala versões mais recentes do Flex e Bison em diretórios diferentes do sistema. Pode ser necessário ajustar o `PATH`:
> ```bash
> export PATH="$(brew --prefix bison)/bin:$(brew --prefix flex)/bin:$PATH"
> ```

---

## Como Compilar

```bash
# Clone o repositório
git clone <url-do-repositorio>
cd Compiladores-G6-T01

# Crie o diretório de build
mkdir build && cd build

# Configure com CMake (modo Debug habilita os testes)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Compile
make -j$(nproc)
```

O executável gerado será `build/compilador`.

### Build de Release

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

---

## Como Usar

O compilador opera de maneira análoga a grandes ferramentas da indústria (como o GCC). Por padrão, ao passar um arquivo de entrada, o projeto será compilado por completo e o binário executável será montado e linkado de forma automática usando as ferramentas padrão do sistema.

```bash
./compilador [opcoes] arquivo_entrada
```

### Comportamento Padrão e Arquivos Automáticos

- Se nenhuma flag de modo for informada, o compilador irá processar o seu código até o final (Assembly), chamará o gcc para linkar as saídas e **gerará um binário executável definitivo** (por padrão `a.out`).
- Quando uma flag limitadora de compilação for usada (ex: `-S`, `-fdump-ast`), e o caminho de saída `-o` **não for fornecido**, o compilador **salvará o resultado da etapa automaticamente** em um arquivo usando o nome base da entrada (ex: `programa.ast`, `programa.s`, `programa.tac`).

### Principais Opções

| Flag                      | Descrição                                                                                                           |
| ------------------------- | ------------------------------------------------------------------------------------------------------------------- |
| `-o <arquivo>`            | Coloca a saída explicitamente no `<arquivo>` designado                                                              |
| `-E, --lexer`             | Executa apenas o Lexer e gera os tokens (`.lex`)                                                                    |
| `-fsyntax-only, --parser` | Executa apenas o Parser e valida a sintaxe (`.parser`)                                                              |
| `-fdump-ast, --ast`       | Gera a Árvore Sintática Abstrata (AST) no arquivo (`.ast`)                                                          |
| `--semantic`              | Executa a análise semântica (`.semantic`)                                                                           |
| `-fdump-tac, --tac`       | Gera o Código de Três Endereços (TAC) no arquivo (`.tac`)                                                           |
| `-O, --opt`               | Executa a otimização de código TAC (`.opt`)                                                                         |
| `-S, --codegen`           | Gera apenas o código Assembly final e para (`.s`)                                                                   |
| `--symtable`              | Imprime a tabela de símbolos (funciona em conjunto com as fases de semantica, tac, opt, codegen e binario)           |
| `-h, --help`              | Exibe o menu de ajuda do compilador                                                                                 |

### Exemplos de Execução

```bash
# Compila tudo e gera o executável "a.out" (pronto para rodar via ./a.out)
./compilador programa.cmm

# Compila tudo e gera o executável com o nome "meu_programa"
./compilador programa.cmm -o meu_programa

# Executa apenas até o assembly e salva o resultado automaticamente em "programa.s"
./compilador -S programa.cmm

# Executa apenas até o assembly e salva em "minha_saida.s"
./compilador -S programa.cmm -o minha_saida.s

# Faz a análise léxica e salva os tokens em "programa.lex"
./compilador -E programa.cmm

# Exibe a análise de código de três endereços em "programa.tac"
./compilador -fdump-tac programa.cmm

# Análise semântica incluindo tabela de símbolos explicitamente salva em "analise.txt"
./compilador --semantic --symtable programa.cmm -o analise.txt
```

### Verificando a Execução do Programa

Como a linguagem C-- Strict não possui no momento funções embutidas de saída (como um `print` ou `printf`), não haverá texto no terminal ao rodar o executável gerado. 

Para saber se o programa executou corretamente e verificar o valor que a função `main()` retornou, você deve inspecionar o **código de saída** (exit status) do último comando executado no terminal. A forma de fazer isso varia dependendo de qual terminal você está usando:

**1. Bash ou Zsh (padrão Linux/macOS):**
```bash
./meu_programa
echo $?
```

**2. Fish Shell:**
```fish
./meu_programa
echo $status
```

**3. PowerShell (Windows):**
```powershell
.\meu_programa.exe
echo $LastExitCode
```

**4. Prompt de Comando / CMD (Windows):**
```cmd
meu_programa.exe
echo %ERRORLEVEL%
```

Se a sua função `main` chegou ao fim com sucesso rodando `return 0;`, o terminal imprimirá `0` na tela!

---

## Estrutura do Projeto

```
Compiladores-G6-T01/
├── CMakeLists.txt              # Configuração raiz do CMake
├── src/
│   ├── main.cpp                # Ponto de entrada e parsing de argumentos
│   ├── CMakeLists.txt
│   ├── lexer/
│   │   ├── lexer.l             # Especificação Flex
│   │   └── CMakeLists.txt
│   ├── parser/
│   │   ├── parser.y            # Gramática Bison
│   │   └── CMakeLists.txt
│   ├── ast/
│   │   ├── ast.cpp             # Implementação dos nós da AST
│   │   ├── ASTPrinter.cpp      # Impressão da AST
│   │   └── CMakeLists.txt
│   ├── symtable/
│   │   ├── symtable.cpp        # Tabela de símbolos com escopos
│   │   └── CMakeLists.txt
│   ├── semantic/
│   │   ├── semantic.cpp        # Visitante semântico
│   │   └── CMakeLists.txt
│   └── runners/
│       ├── lexer_runner.cpp    # Modo --lexer
│       ├── parser_runner.cpp   # Modo --parser
│       ├── ast_runner.cpp      # Modo --ast
│       ├── semantic_runner.cpp # Modo --semantic
│       └── CMakeLists.txt
├── include/
│   ├── ast.hpp                 # Definições de todos os nós da AST e interface Visitor
│   ├── ASTPrinter.hpp
│   ├── tokens.hpp              # Wrapper para parser.hpp (gerado pelo Bison)
│   ├── semantic.hpp            # Declaração do SemanticVisitor
│   ├── symtable.hpp            # Declaração do SymbolTable e Symbol
│   └── runners.hpp             # Assinaturas das funções dos runners
├── tests/
│   ├── CMakeLists.txt          # Configuração do GoogleTest (FetchContent)
│   ├── lexer/
│   │   ├── teste_léxico.cpp    # Testes de análise léxica
│   │   └── teste_lexico.txt    # Entradas de teste
│   └── parser/
│       ├── teste_sintatico_parser.cpp  # Testes de análise sintática
│       ├── teste_sintatico_parser.cmm  # Programa C-- de teste
│       └── teste_parser.cmm            # Programa C-- abrangente
├── docs/                       # Documentação do projeto (em português)
│   ├── Gramática.md
│   ├── descricao_compilador.md
│   ├── ferramentas.md
│   ├── funcionamento_parser.md
│   └── ...
├── mkdocs.yml                  # Configuração do site de documentação (MkDocs)
└── pyproject.toml              # Dependências Python (ferramentas de documentação)
```

---

## Testes

Os testes utilizam o framework **GoogleTest** e são baixados automaticamente via CMake `FetchContent`.

> Os testes são habilitados apenas em modo **Debug**.

```bash
cd build

# Executar todos os testes
ctest --output-on-failure

# Ou executar o binário de testes diretamente
./testes
```

### Cobertura dos Testes

| Módulo            | Arquivo de Teste                          |
| ----------------- | ----------------------------------------- |
| Análise Léxica    | `tests/lexer/teste_léxico.cpp`            |
| Análise Sintática | `tests/parser/teste_sintatico_parser.cpp` |

---

## Documentação

A documentação completa do projeto está disponível no diretório [`docs/`](docs/) e inclui:

- **[Gramática](docs/Gramática.md)** — Especificação BNF da linguagem C-- Strict
- **[Descrição do Compilador](docs/descricao_compilador.md)** — Visão geral da arquitetura
- **[Ferramentas](docs/ferramentas.md)** — Detalhes sobre Flex, Bison e C++
- **[Funcionamento do Parser](docs/funcionamento_parser.md)** — Detalhes da análise sintática

A documentação também pode ser servida localmente com MkDocs:

```bash
# Instalar dependências Python (com Poetry)
poetry install

# Servir a documentação localmente
poetry run mkdocs serve
```

---

## Grupo

Projeto desenvolvido para a disciplina de **Compiladores** — Grupo 6, Trabalho 1.

| Nome           | GitHub                                               |
| -------------- | ---------------------------------------------------- |
| Samuel Caetano | [@samuelncaetano](https://github.com/samuelncaetano) |
| Marjorie Mitzi | [@Marjoriemitzi](https://github.com/Marjoriemitzi)   |
| Thiago Accioly | [@Acciolyy](https://github.com/Acciolyy)             |
| Julia Massuda  | [@JuliaMassuda](https://github.com/JuliaMassuda)     |
| João Rodrigues | [@JpRodrigues2](https://github.com/JpRodrigues2)     |
