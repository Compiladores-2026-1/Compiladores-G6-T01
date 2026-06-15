# Padrão de Commit — Angular Commit Convention

Este projeto adota o padrão **Angular Commit Convention** para mensagens de commit. O objetivo é manter o histórico do repositório legível, filtrável e semântico.

---

## 1. Formato

```
<tipo>(<escopo opcional>): <descrição curta>

<corpo opcional>

<rodapé opcional>
```

### Regras gerais

- A **descrição curta** deve estar em letras minúsculas e não terminar com ponto final.
- O **tipo** é obrigatório.
- O **escopo** é opcional e identifica o módulo afetado (ex: `lexer`, `parser`, `semantic`, `docs`).
- A linha de assunto não deve ultrapassar **72 caracteres**.

---

## 2. Tipos Permitidos

| Tipo       | Quando usar |
|------------|-------------|
| `feat`     | Nova funcionalidade ou comportamento adicionado |
| `fix`      | Correção de bug |
| `docs`     | Alterações exclusivamente em documentação |
| `refactor` | Refatoração de código sem mudança de comportamento externo |
| `test`     | Adição ou correção de testes |
| `chore`    | Tarefas de manutenção: build, dependências, configuração |
| `style`    | Formatação, espaços, vírgulas — sem mudança de lógica |
| `perf`     | Melhoria de desempenho |
| `ci`       | Alterações em arquivos de integração contínua (GitHub Actions, etc.) |
| `revert`   | Reversão de um commit anterior |

---

## 3. Exemplos

### Feat sem escopo
```
feat: adiciona suporte a operadores de atribuição composta
```

### Feat com escopo
```
feat(semantic): implementar análise semântica e tabela de símbolos
```

### Fix
```
fix(lexer): corrigir reconhecimento de literais float negativos
```

### Docs
```
docs: adiciona README com informações gerais do projeto
```

### Docs com escopo
```
docs(parser): documenta regras de precedência de operadores
```

### Test
```
test(parser): adiciona testes sintáticos negativos (sad path)
```

### Chore
```
chore: atualiza dependências do CMakeLists
```

### Refactor
```
refactor(ast): simplifica destrutores usando delete recursivo
```

---

## 4. Nomes de Branches

As branches seguem o padrão `<tipo>/<descricao-em-kebab-case>`:

| Exemplo de Branch                       | Propósito                                 |
|-----------------------------------------|-------------------------------------------|
| `feat/analise-semantica`                | Nova funcionalidade                       |
| `fix/erro-lexer-float`                  | Correção de bug                           |
| `docs/readme`                           | Documentação                              |
| `docs/arquitetura-escopo-tac`           | Documentação de arquitetura e TAC         |
| `test/testes-incorretos-analisador`     | Novos testes                              |
| `refactor/ast-visitor`                  | Refatoração                               |

---

## 5. Por que seguir esse padrão?

- Facilita a **geração automática de changelogs**.
- Permite identificar o impacto de um commit pelo tipo imediatamente.
- Mantém consistência no histórico para todos os membros do grupo.
- É amplamente adotado em projetos open-source e reconhecido pelo GitHub.
