# Setup do Projeto (Compilador)

Este guia descreve os passos necessários para configurar o ambiente de desenvolvimento, compilar e executar o nosso compilador localmente, além de como executar a documentação do projeto.

---

## 🛠️ Compilando o Projeto

O motor principal da compilação do nosso projeto utiliza o **CMake** para orquestrar o Flex, o Bison e a compilação em C.

### 1. Pré-requisitos (Dependências)

Para que o projeto seja compilado e executado corretamente, seu sistema precisará ter as seguintes ferramentas instaladas:

* **CMake**: Ferramenta de automação de compilação *out-of-source*.
* **Compilador C (GCC)**: Necessário para a etapa final de montagem e linkagem do código em C.
* **Flex**: Gerador de analisador léxico.
* **Bison**: Gerador de analisador sintático.
* **Make**: (Usado implicitamente pelo CMake em alguns ambientes).

**Como instalar as dependências no Linux (Ubuntu/Debian ou WSL):**
```bash
sudo apt update
sudo apt install build-essential flex bison cmake
```
*(O pacote `build-essential` já instala o `make` e o `gcc` automaticamente)*

**No macOS (usando Homebrew):**
```bash
brew install flex bison cmake
```

**No Windows:**
Recomendamos o uso do **WSL (Windows Subsystem for Linux)** ou o ambiente **MSYS2** para instalar as ferramentas acima.

### 2. Como Compilar

Graças ao `CMake`, o processo se divide em configuração e construção (build), mantendo os arquivos gerados separados do código fonte.

1. **Clone o repositório:**
   ```bash
   git clone https://github.com/Marjoriemitzi/Compiladores--G6-T01.git
   cd Compiladores--G6-T01
   ```

2. **Gere o ambiente de compilação (Debug):**
   ```bash
   cmake -B debug -DCMAKE_BUILD_TYPE=Debug
   ```

3. **Compile o projeto:**
   ```bash
   cmake --build debug -j
   ```
   *(Isto gerará o executável dentro da pasta `debug/`)*

### 3. Como Executar

Após o sucesso da compilação, o executável estará pronto para uso. Para rodar o programa, execute:

```bash
./debug/compilador
```

> **Testes Automatizados:** O nosso `CMakeLists.txt` possui integração com o `GoogleTest`. Ao compilar no modo `Debug`, os testes podem ser adicionados/compilados e registrados, mas **não são executados automaticamente**.
> 
> Para executá-los, rode:
> ```bash
> ctest --test-dir debug --output-on-failure
> ```
> 
> Alternativamente, você também pode executar diretamente o binário de testes gerado na pasta `debug/`, caso aplicável.

---

## 📚 Executando a Documentação (MkDocs)

Nós utilizamos o **MkDocs** com o tema Material para a documentação, e o **Poetry** para gerenciar essas dependências no Python.

### 1. Pré-requisitos da Documentação

Caso não tenha o Poetry instalado no sistema (Linux/WSL), recomendamos a instalação oficial:

```bash
curl -sSL https://install.python-poetry.org | python3 -
export PATH="$HOME/.local/bin:$PATH"
```
*(Para tornar o comando permanente, adicione o `export` ao final do seu `~/.bashrc` ou `~/.zshrc`)*

### 2. Como Rodar a Documentação

1. **Instale as dependências (MkDocs e plugins):**
 Na raiz do projeto, execute:
   ```bash
   poetry install --with docs --no-root
   ```
   *(Nota: O grupo `docs` precisa ser instalado explicitamente para disponibilizar o `mkdocs`. Se houver a configuração `package-mode = false` no `pyproject.toml`, a flag `--no-root` pode ser omitida, mas mantenha a flag `--with docs`).*

2. **Inicie o servidor local:**
   ```bash
   poetry run mkdocs serve
   ```

3. **Acesse:** Abra http://127.0.0.1:8000 no seu navegador. A página recarregará automaticamente sempre que você alterar um arquivo `.md` na pasta `docs/`.

