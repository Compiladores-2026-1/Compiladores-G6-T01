# Descrição das Ferramentas e Tecnologias

O desenvolvimento do compilador para a linguagem **C-- Strict** (versão simplificada) utiliza um conjunto de ferramentas clássicas de construção de compiladores.O objetivo central é a tradução de um subconjunto da linguagem C para código de máquina **Assembly x86-64**.

## 1. Ferramentas de Automação

### Flex (Fast Lexical Analyzer Generator)
Responsável pela **Análise Léxica** do compilador.
* **Função**: Identifica os *tokens* da linguagem (como `INT`, `IF`, `ID` e operadores) através de expressões regulares.
* **Papel no Projeto**: Gera o analisador que quebra o código-fonte em unidades básicas para processamento posterior.

### Bison (GNU Parser Generator)
Responsável pela **Análise Sintática**.
* **Função**: Utiliza a gramática BNF (Backus-Naur Form) para validar a estrutura lógica do programa.
* **Papel no Projeto**: Garante que o código respeite as regras da linguagem, como a estrutura da função `main` e a precedência de operações.



## 2. Core e Backend

### Linguagem C++
A lógica principal do compilador é desenvolvida em **C++**.
**Tradução**: O backend em C++ percorre a árvore sintática gerada pelo Bison para realizar a emissão de código **Assembly x86-64**.
**Gestão de Memória**: O uso de C++ permite o controle estrito da **Pilha (Stack)**, uma característica fundamental do C-- Strict, que não suporta alocação dinâmica (Heap).

### GCC (GNU Compiler Collection)
Utilizado para a etapa final de **Montagem e Linkagem**.
* O GCC compila o código C++ do projeto e os arquivos gerados pelo Flex/Bison para criar o executável final chamado `compilador`.

## 3. Gestão e Ambiente

### GNU Make
Ferramenta de **automação de compilação**.
* Através de um arquivo `Makefile`, o projeto coordena a execução do Bison, Flex e GCC em uma sequência única e eficiente.

### Ambiente de Desenvolvimento
O projeto é desenvolvido em ambiente **Linux (Ubuntu/Debian)** ou via **WSL (Windows Subsystem for Linux)**.
* Este ambiente fornece o suporte nativo para as ferramentas `build-essential`, garantindo a compatibilidade com os registradores e a arquitetura x86-64.
