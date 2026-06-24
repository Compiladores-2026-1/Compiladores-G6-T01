# Integração Contínua (CI)

O projeto usa **GitHub Actions** para automatizar, a cada `push` e `pull request`, a compilação, a execução da suíte de testes e a validação da documentação. Assim, regressões em código ou em docs são detectadas antes do merge.

---

## 1. Workflows

| Workflow | Arquivo | Gatilho | Função |
|---|---|---|---|
| **CI** | `.github/workflows/ci.yml` | `push` e `pull_request` | Compila o projeto, roda os testes e valida o build das docs |
| **Deploy MkDocs** | `.github/workflows/deploy.yml` | `push` na `main` | Publica a documentação no GitHub Pages |

---

## 2. Workflow de CI (`ci.yml`)

São dois jobs independentes:

### Job `test`
1. Instala as dependências de build (`flex`, `bison`, `cmake`, `build-essential`).
2. Configura o projeto em modo Debug — os testes só são habilitados nesse modo (ver `CMakeLists.txt`):
   ```bash
   cmake -B build -DCMAKE_BUILD_TYPE=Debug
   ```
3. Compila:
   ```bash
   cmake --build build -j
   ```
4. Executa toda a suíte (GoogleTest + comparação de TAC) via CTest:
   ```bash
   ctest --test-dir build --output-on-failure
   ```

A suíte inclui os testes léxicos, os sintáticos (*happy path* e *sad path*) e a comparação de TAC (`tac_golden`). Qualquer teste falho derruba o job.

### Job `docs`
1. Configura Python 3.12 e instala as versões fixadas de `mkdocs` e `mkdocs-material` (as mesmas do deploy).
2. Valida o site em modo estrito, que falha em erros de link, navegação ou configuração:
   ```bash
   mkdocs build --strict
   ```

---

## 3. Reproduzindo Localmente

Os mesmos passos da CI podem ser rodados na sua máquina:

```bash
# Testes
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
ctest --test-dir build --output-on-failure

# Documentação
mkdocs build --strict
```

Consulte [Setup do Projeto](setup_compilador.md) para a instalação das dependências.

---

## 4. Lendo o Resultado

Na aba **Actions** do repositório no GitHub, cada execução mostra os jobs `test` e `docs`. Em um pull request, o status aparece diretamente na página do PR — um ✗ indica teste ou build de docs quebrado, com o log detalhado disponível ao expandir o passo correspondente.

Para detalhes dos testes de cada fase, veja a seção **Testes** (ex.: [Testes de Análise Sintática](testes_analise_sintatica.md) e [Testes de Geração de Código Intermediário](testes_conversao_tac.md)).
