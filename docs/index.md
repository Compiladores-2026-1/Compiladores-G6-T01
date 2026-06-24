# Compilador C-- Strict

Documentação técnica do compilador para a linguagem **C-- Strict**, desenvolvido pelo **Grupo 6** na disciplina de Compiladores.

---

## O que é o C-- Strict?

C-- Strict é um subconjunto simplificado da linguagem C, projetado para ser compilado diretamente para Assembly x86-64. A linguagem elimina ponteiros, alocação dinâmica e strings, tornando todo o gerenciamento de memória previsível e restrito à pilha (*stack*).

## Estrutura da Documentação

| Seção | Conteúdo |
|---|---|
| **Visão Geral** | Descrição do compilador, lógica da linguagem e arquitetura técnica |
| **Fases do Compilador** | Documentação de cada fase: léxica, sintática, intermediária e final |
| **Projeto e Processo** | Ferramentas, setup do ambiente e padrão de commits |
| **Implementação** | Detalhes de implementação de cada fase |
| **Testes** | Relatórios e resultados de testes por fase |
| **Notas de Estudo** | Material de apoio teórico |

## Fases do Compilador

```
Código-fonte (.cmm)
      ↓
  Análise Léxica      →  tokens
      ↓
  Análise Sintática   →  AST
      ↓
  Análise Semântica   →  AST validada + tabela de símbolos
      ↓
  Geração TAC         →  código de três endereços
      ↓
  Geração Assembly    →  x86-64
```

## Início Rápido

```bash
# Build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

# Analisar um arquivo
./compilador --semantic --symtable programa.cmm
```

Consulte [Descrição do Setup](setup_compilador.md) para instruções completas de ambiente.
