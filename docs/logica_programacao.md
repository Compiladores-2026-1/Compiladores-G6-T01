## 1. Introdução

Este documento especifica a lógica de programação e os fundamentos teóricos da linguagem **C-- Strict (versão simplificada)**. A linguagem foi projetada com uma gramática reduzida, possuindo objetivos educacionais e arquitetônicos bem definidos. Seu foco principal abrange:

- **Implementação direta em assembly x86-64:** A sintaxe e a semântica foram desenhadas para permitir uma tradução direta para código de máquina, facilitando o estudo e o desenvolvimento de compiladores.
- **Ausência de estruturas complexas:** Conceitos avançados como ponteiros, arrays, alocação dinâmica (heap) e objetos foram intencionalmente omitidos para manter o compilador e a linguagem minimalistas.
- **Previsibilidade semântica:** Cada instrução possui um comportamento estrito e previsível, sem abstrações ou operações implícitas de alto nível.

A linguagem adota o paradigma **imperativo estruturado**, com execução estritamente sequencial e controle explícito de fluxo.

---

## 2. O Paradigma de Programação

A arquitetura do C-- Strict baseia-se na combinação de dois conceitos fundamentais da ciência da computação.

### 2.1. Paradigma Imperativo

No paradigma imperativo, o código-fonte especifica os passos exatos que a máquina deve executar para alcançar um resultado. O modelo é baseado na alteração contínua de estado.

- **Mutabilidade e Estado:** O programa possui um estado global e local mantido na memória. As instruções são executadas para modificar esse estado.

```c
// Exemplo de mutabilidade e alteração de estado
int x;
x = 5;       // Estado inicial de x
x = x + 1;   // O estado de x é mutado de forma imperativa para 6
```

### 2.2. Paradigma Estruturado

O paradigma estruturado é uma disciplina de programação que restringe e organiza o fluxo de controle do programa. Todo o fluxo lógico no C-- Strict é construído de forma hierárquica, utilizando exclusivamente três estruturas fundamentais:

1. **Sequência:** Execução linear das instruções.
2. **Seleção:** Avaliação de condições lógicas para tomada de decisões (`if` / `else`).
3. **Iteração:** Repetição controlada de um bloco de código (`while` / `for`).

---

## 3. Estrutura de um Programa

Todo programa em C-- Strict deve seguir uma arquitetura previsível, composta por declarações de escopo global e definições de funções.

### 3.1. A Função `main`

É obrigatória a existência de exatamente **uma função chamada `main`**. A execução do programa inicia-se no começo desta função e encerra-se quando o seu bloco é finalizado ou uma instrução `return` é executada.

```c
// Exemplo da estrutura básica de um programa
int contador_global = 0; // Segmento Global

int main() {             // Ponto de entrada
    int status = 1;      // Variável na Pilha (Stack)
    return 0;            // Encerramento do programa
}
```

---

## 4. Tipagem Estática e Variáveis

Uma variável é uma abstração para um endereço de memória, definida por quatro propriedades: identificador, tipo, valor e escopo.

### 4.1. Tipagem Estática

A linguagem emprega um sistema de tipagem estática. O tipo de uma variável é determinado em tempo de compilação e permanece inalterado durante toda a execução. Operações entre tipos incompatíveis geram erros no processo de compilação.

### 4.2. Tipos Suportados

A linguagem suporta tipos primitivos básicos:

```c
int numero = -10;       // Valores inteiros
float pi = 3.14;        // Valores de ponto flutuante
char letra = 'A';       // Caracteres isolados
bool condicao = true;   // Valores booleanos (true ou false)
```

_Nota interna:_ O C-- Strict trata o número `0` como `false` e qualquer valor diferente de zero como `true`.

---

## 5. Expressões e Avaliação

Uma **expressão** é uma combinação de variáveis, literais e operadores que é avaliada pelo processador para produzir um único valor. A avaliação respeita estritamente a precedência matemática.

```c
// Exemplo de expressões aritméticas e relacionais
int a = 5;
int b = 10;
int resultado = (a + b) * 2; // O parêntese altera a precedência natural
bool eh_maior = resultado > 20; // Expressão relacional resultando em booleano
```

---

## 6. Controle de Fluxo

As estruturas de controle definem os desvios e iterações na execução das instruções.

### 6.1. Seleção: Condicionais (`if` / `else`)

Avalia uma expressão lógica para determinar o caminho de execução.

```c
int saldo = 100;

if (saldo > 0) {
    saldo = saldo - 10;
} else {
    saldo = 0;
}
```

### 6.2. Iteração: Repetições (Loops)

Estruturas para execução contínua de blocos lógicos.

- **`while`**: Executa enquanto a condição for avaliada como verdadeira.

```c
int i = 0;
while (i < 5) {
    i = i + 1;
}
```

- **`for`**: Consolida inicialização, condição e incremento/passo na mesma estrutura.

```c
int total = 0;
for (int i = 0; i < 10; i++) {
    total = total + i;
}
```

### 6.3. Interrupção de Fluxo

- **`break`**: Causa a saída imediata da iteração mais interna.
- **`continue`**: Interrompe a iteração atual e salta para a avaliação da próxima condição lógica.

---

## 7. Escopo de Variáveis (Visibilidade)

O escopo define a visibilidade temporal e espacial de uma variável.

- **Escopo Global:** Variáveis definidas fora de funções. São acessíveis globalmente.
- **Escopo Local:** Variáveis definidas dentro de blocos (delimitados por `{}`). Existem apenas durante a execução daquele bloco.
- **Sobrescrita (Shadowing):** Variáveis locais com o mesmo identificador de variáveis em escopos superiores assumem precedência.

```c
int x = 10; // Variável Global

int main() {
    int x = 5; // Variável Local. Ocorre "shadowing" da variável global

    if (true) {
        int x = 2; // Escopo de bloco fechado.
        // Neste ponto, x vale 2
    }

    // Neste ponto, x volta a valer 5
    return 0;
}
```

---

## 8. Funções e Modularidade

Funções são sub-rotinas isoladas que encapsulam blocos lógicos, permitindo a modularização e o reuso de código.

### 8.1. Definição e Invocação

A cada invocação de função, um novo contexto de execução (Stack Frame) é alocado, garantindo que os parâmetros e variáveis locais não interfiram em outras execuções.

```c
// Definição da função modular
int calcular_area(int base, int altura) {
    int area = base * altura;
    return area; // Retorna o valor e destrói o Stack Frame local
}

int main() {
    // Invocação da função e atribuição do resultado
    int retangulo = calcular_area(10, 5);
    return 0;
}
```

---

## 9. Limitações Intencionais

O C-- Strict possui restrições arquitetônicas elaboradas para garantir determinismo e facilidade de compilação. A linguagem **não suporta**:

- **Ponteiros e Aritmética de Memória:** Mitiga o risco de violações de segmento (_Segmentation Faults_) causadas por acessos indevidos à memória.
- **Alocação Dinâmica (Heap):** A ausência de operadores como `malloc` ou `new` delega a gerência de dados exclusivamente à pilha de execução (Stack), prevenindo vazamentos de memória (_memory leaks_).

Estas limitações estabelecem o **determinismo**: a propriedade matemática que assegura que, sob entradas idênticas, a execução do código produzirá inevitavelmente os mesmos resultados, sem efeitos colaterais gerados por manipulações complexas de estado na memória.

---
