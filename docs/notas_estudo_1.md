# 1. Capítulo – Definição Formal da gramática livre de contexto

## 1.1. Objetivos de aprendizagem

Ao final deste capítulo, o aluno deverá ser capaz de:

- **Compreender rigorosamente** a definição formal de uma Gramática Livre de Contexto (CFG).
- **Diferenciar** símbolos terminais e não-terminais sob o ponto de vista formal.
- **Interpretar** o papel das produções e do símbolo inicial na construção de sentenças.
- **Determinar a linguagem gerada** por uma CFG através de derivações.
- **Relacionar** a definição matemática de CFG com sua representação em árvores sintáticas.

## 1.2. Conceitos teóricos

### 1.2.1. Linguagem como conjunto de cadeias

Antes de definirmos a gramática, precisamos entender o que ela se propõe a criar. Na teoria de linguagens formais, uma **linguagem** é um conjunto (muitas vezes infinito) de cadeias de caracteres.

- **$\Sigma$ (Alfabeto):** Um conjunto finito de símbolos básicos (ex: $\{0, 1\}$ ou $\{a, b, c\}$).
- **$\Sigma^*$ (Fecho de Kleene):** O conjunto de todas as combinações possíveis de símbolos de $\Sigma$, incluindo a cadeia vazia ($\varepsilon$).
- **$L$ (Linguagem):** Um subconjunto de $\Sigma^*$ ($L \subseteq \Sigma^*$), em compiladores, a linguagem corresponde ao **conjunto de todos os programas sintaticamente válidos** de uma linguagem de programação.

#### 1.2.1.1. Exemplo na linguagem C

| Componente Formal   | Exemplo na linguagem C                                               |
| ------------------- | -------------------------------------------------------------------- |
| Alfabeto ($\Sigma$) | Tokens como `int`, `while`, `if`, `+`, `{`, `}`, `ID`, `NUM`.        |
| Linguagem ($L$)     | O conjunto infinito de todos os programas C sintaticamente corretos. |

### 1.2.2. Definição formal de CFG

Uma **Gramática Livre de Contexto** é um quádruplo:

$$
G = \langle V, \Sigma, P, S \rangle
$$

onde cada componente segue rigorosamente as regras da teoria dos conjuntos:

- **$V$**: É um conjunto finito de **símbolos não-terminais**
    - _variáveis que representam estruturas_;
    - _“espaços reservados” que serão substituídos_.
- **$\Sigma$**: É um conjunto finito de **símbolos terminais** (símbolos que formam as cadeias finais da linguagem), tal que $V \cap \Sigma = \varnothing$
    - _A interseção entre $V$ e $\Sigma$ é o conjunto vazio; ou seja, um símbolo não pode ser terminal e não-terminal ao mesmo tempo_.
- **$P$**: É um conjunto finito de **produções**, da forma: $A \rightarrow \alpha$, com $A \in V$ e $\alpha \in (V \cup \Sigma)^*$
    - _$A$ pertence ao conjunto de não-terminais_;
    - _$\alpha$ pertence ao conjunto de todas as cadeias possíveis formadas pela união de terminais e não-terminais, incluindo a cadeia vazia_.
- **$S$**: É o **símbolo inicial**, o ponto de partida da gramática, onde $S \in V$
    - _$S$ pertence ao conjunto de não-terminais; ele é o ponto de partida obrigatório_.

### 1.2.3. Linguagem gerada por uma CFG

A **linguagem gerada** por uma gramática $G$, denotada por $L(G)$, é o conjunto de todas as cadeias que a gramática consegue _“fabricar”_ usando apenas símbolos que o usuário final vê (terminais).

Formalmente:

$$
L(G) = \{ w \in \Sigma^* \mid S \Rightarrow^* w \}
$$

**Tradução da fórmula:**

- $w \in \Sigma^*$: $w$ é uma cadeia que pertence ao conjunto de todas as combinações possíveis de terminais (**$w$ contém apenas elementos do alfabeto final, ex: `int x = 10;`**).
- $S \Rightarrow^* w$: Existe um caminho de derivação que parte de $S$ e chega em $w$ em zero ou mais passos ($\Rightarrow^*$) (**$w$ é derivável a partir do símbolo inicial**).

### 1.2.4. Componentes fundamentais de uma CFG

Uma CFG é composta por quatro elementos fundamentais:

- Um conjunto de **símbolos não-terminais**, que representam categorias sintáticas.
- Um conjunto de **símbolos terminais**, que correspondem aos tokens da linguagem.
- Um conjunto de **produções**, que definem regras de substituição.
- Um **símbolo inicial**, que representa a categoria sintática principal da linguagem.

Cada um desses elementos possui papel distinto e bem definido, explorado formalmente a seguir.

### 1.2.5. Terminais vs. Não-Terminais

| Característica        | Símbolos Terminais ($\Sigma$)                | Símbolos Não-Terminais ($V$)              |
| --------------------- | -------------------------------------------- | ----------------------------------------- |
| Definição             | Tokens da entrada (código fonte).            | Estruturas sintáticas abstratas.          |
| Substituição          | **Não** podem ser substituídos (são finais). | **Devem** ser substituídos via produções. |
| Localização na árvore | Aparecem apenas nas **folhas**.              | Aparecem como **nós internos** e raiz.    |

### 1.2.6. Produções

Uma produção define como um não-terminal pode ser reescrito. O termo _livre de contexto_ decorre do fato de que:

- O lado esquerdo da produção é **sempre um único não-terminal**
    - **Lado Esquerdo ($A$):** Deve ser estritamente um **único** símbolo não-terminal.
        - _Na prática:_ Você não pode ter algo como $\alpha A \beta \rightarrow \alpha \gamma \beta$, onde $A$ pode muda sem depender daqueles ao ser redor $\alpha$ e $\beta$, por isso, “livre de contexto”.
- A aplicação da produção **independe do contexto**
    - **Lado Direito ($\alpha$):** Pode ser qualquer combinação de símbolos.
        - Pode ter apenas terminais: $A \rightarrow abc$
        - Pode ter apenas não-terminais: $A \rightarrow BC$
        - Pode ser misto: $A \rightarrow aBb$
        - Pode ser vazio: $A \rightarrow \varepsilon$ (onde $\varepsilon$ representa a ausência de símbolos/cadeia vazia).

Essa restrição distingue CFG de gramáticas sensíveis ao contexto.

## 1.3. Exemplos passo a passo

Considere a CFG:

- **$V$ (símbolos não-terminais)** = $\{ \ S \ \}$
- **$\Sigma$ (símbolos terminais)** = $\{ \ a,\ b \ \}$
- **$P$ (Conjunto de Produções)**:
    - $S \rightarrow aSb$
    - $S \rightarrow \varepsilon$
- **$S$ (símbolo inicial)** = $S$

### 1.3.1. Interpretação

- **A produção (1) $S \rightarrow aSb$** garante que, para cada `a` adicionado à esquerda, um `b` correspondente é adicionado à direita. Isso cria a estrutura aninhada/balanceada.
- **A produção (2) $S \rightarrow \varepsilon$** é a condição de parada. Sem ela, a recursão seria infinita e nunca geraríamos uma cadeia finita de terminais.

### 1.3.2. Cadeias geradas e derivação

Algumas cadeias pertencentes a **$L(G)$**: $\varepsilon, ab, aabb, aaabbb$.

Vejamos como a gramática “fabrica” a cadeia **aabb** passo a passo. Note como os **não-terminais** (_neste caso, apenas $S$_) vão sendo substituídos até restarem apenas terminais:

| Regra                                             | Produção                       |
| ------------------------------------------------- | ------------------------------ |
| 1. Símbolo Inicial                                | $S$                            |
| 2. Aplicando regra 1: $S \rightarrow aSb$         | $\Rightarrow aSb$              |
| 3. Aplicando regra 1: $S \rightarrow aSb$         | $\Rightarrow aaSbb$            |
| 4. Aplicando regra 2: $S \rightarrow \varepsilon$ | $\Rightarrow aa\varepsilon bb$ |
| 6. Resultado final                                | $= aabb$                       |

Este exemplo ilustra como uma CFG pode gerar uma linguagem **infinita** (_podemos aplicar a regra 1 quantas vezes quisermos_), mantendo uma definição **finita** e compacta.

### 1.3.3. Exemplo de Aplicação (C-Like)

Se estivéssemos definindo uma pequena parte da linguagem C para declaração de variáveis, teríamos a seguinte estrutura formal:

- **$V$** = $\{ \ \langle decl\rangle, \ \langle tipo\rangle, \ \langle id\rangle \}$
- **$\Sigma$** = $\{ \ \text{int}, \ \text{float}, \ x, \ y, \ \text{;} \}$
- **$P$**:
    1.  $\langle decl\rangle \rightarrow \langle tipo\rangle \ \langle id\rangle \text{;}$
    2.  $\langle tipo\rangle \rightarrow \text{int} \mid \text{float}$
    3.  $\langle id\rangle \rightarrow x \mid y$
- **$S$** = $\langle decl\rangle$

#### 1.3.3.1. Análise Formal da Gramática

Para garantir que esta estrutura é uma CFG válida, verificamos as definições matemáticas de seus conjuntos e regras:

1. Disjunção de Conjuntos ($V \cap \Sigma = \varnothing$)

A regra de que um símbolo não pode ser terminal e não-terminal ao mesmo tempo é respeitada estritamente:

- $\langle tipo\rangle$ é um conceito abstrato (pertence a **$V$**).
- $int$ é a palavra concreta escrita no código (pertence a **$\Sigma$**).

Eles são categorias disjuntas, garantindo a consistência da definição.

2. Validade das Produções ($P$)

Cada regra em $P$ deve obedecer à forma $A \rightarrow \alpha$, onde $A \in V$ e $\alpha \in (V \cup \Sigma)^*$. Vamos analisar o exemplo sob essa ótica:

- **O Lado Esquerdo ($A \in V$):**
    - Em todas as regras ($\langle decl\rangle$, $\langle tipo\rangle$, $\langle id\rangle$), temos estritamente **um único símbolo não-terminal** à esquerda da seta.
- **O Lado Direito ($\alpha \in (V \cup \Sigma)^*$):**
    - Na regra 1 ($\langle decl\rangle$), $\alpha$ é uma sequência mista de não-terminais e terminais.
    - Na regra 2 ($\langle tipo\rangle$), $\alpha$ é composto apenas por terminais.
    - Na regra 3 ($\langle id\rangle$), $\alpha$ é composto apenas por terminais.
    - Todos os casos são válidos, pois pertencem ao conjunto de todas as cadeias possíveis formadas pela união de $V$ e $\Sigma$.

3. Validade do Símbolo Inicial ($S$)

$S$ pertence ao conjunto de não-terminais sendo o símbolo inicial da gramática.

- Definimos $S = \langle decl\rangle$. Como $\langle decl\rangle \in V$, a definição é satisfeita.
- Isso implica que qualquer derivação nesta gramática deve começar intencionando formar uma “declaração completa”, e não apenas um “tipo” ou um “identificador” isolado.
