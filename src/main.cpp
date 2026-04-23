#include <iostream>
#include <string>
#include <cstdio>

#include "runners.hpp"

extern FILE *yyin;

typedef enum
{
    MODE_LEXER,
    MODE_PARSER,
    MODE_AST,
    MODE_SEMANTIC,
    MODE_TAC,
    MODE_OPT,
    MODE_CODEGEN
} ExecutionMode;

static void print_usage(const std::string &program_name)
{
    std::cerr << "Uso: " << program_name << " <--lexer | --parser | --ast | --semantic | --tac | --opt | --codegen> [--symtable] [arquivo_entrada]\n"
              << "Exemplos:\n"
              << "  " << program_name << " --opt ./tests/parser/arquivo_entrada.cmm\n"
              << "  " << program_name << " --semantic --symtable arquivo_entrada.cmm\n";
}

int main(int argc, char **argv)
{
    ExecutionMode mode = MODE_LEXER; // Valor padrão, será sobrescrito se uma flag de modo for fornecida

    bool mode_defined = false;   // Para garantir que o usuário forneça exatamente uma flag de modo
    bool debug_symtable = false; // Para controlar a impressão da tabela de símbolos durante as fases de semântica, TAC, otimização e geração de código

    std::string input_path = ""; // Caminho do arquivo de entrada, se fornecido. Se vazio, lerá da entrada padrão.
    FILE *input = stdin;         // Ponteiro para o arquivo de entrada, inicializado para stdin

    std::string program_name = argv[0]; // Nome do programa, usado para mensagens de erro e uso

    // Processar os argumentos de linha de comando
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg == "--lexer")
        {
            mode = MODE_LEXER;
            mode_defined = true;
            continue;
        }
        if (arg == "--parser")
        {
            mode = MODE_PARSER;
            mode_defined = true;
            continue;
        }
        if (arg == "--ast")
        {
            mode = MODE_AST;
            mode_defined = true;
            continue;
        }
        if (arg == "--semantic")
        {
            mode = MODE_SEMANTIC;
            mode_defined = true;
            continue;
        }
        if (arg == "--tac")
        {
            mode = MODE_TAC;
            mode_defined = true;
            continue;
        }
        if (arg == "--opt")
        {
            mode = MODE_OPT;
            mode_defined = true;
            continue;
        }
        if (arg == "--codegen")
        {
            mode = MODE_CODEGEN;
            mode_defined = true;
            continue;
        }

        if (arg == "--symtable")
        {
            debug_symtable = true;
            continue;
        }

        if (arg[0] == '-')
        {
            std::cerr << "Erro: flag desconhecida: " << arg << "\n";
            print_usage(program_name);
            return 1;
        }

        if (!input_path.empty())
        {
            std::cerr << "Erro: informe no máximo um arquivo de entrada.\n";
            print_usage(program_name);
            return 1;
        }

        input_path = arg;
    }

    // Verificar se uma flag de modo foi fornecida
    if (!mode_defined)
    {
        std::cerr << "Erro: informe uma flag de modo.\n";
        print_usage(program_name);
        return 1;
    }

    // Abrir o arquivo de entrada, se um caminho for fornecido
    if (!input_path.empty())
    {
        input = fopen(input_path.c_str(), "r");
        if (input == nullptr)
        {
            perror("Erro ao abrir arquivo de entrada");
            return 1;
        }
    }

    // Configurar yyin para ler do arquivo de entrada ou da entrada padrão
    yyin = input;

    // Executar o modo selecionado
    int exit_code = 1;
    switch (mode)
    {
    case MODE_LEXER:
        exit_code = run_lexer_mode();
        break;
    case MODE_PARSER:
        exit_code = run_parser_mode();
        break;
    case MODE_AST:
        exit_code = run_ast_mode();
        break;
        // case MODE_SEMANTIC:
        //     exit_code = run_semantic_mode(debug_symtable);
        //     break;
        // case MODE_TAC:
        //     exit_code = run_tac_mode(debug_symtable);
        //     break;
        // case MODE_OPT:
        //     exit_code = run_opt_mode(debug_symtable);
        //     break;
        // case MODE_CODEGEN:
        //     exit_code = run_codegen_mode(debug_symtable);
        //     break;
    }

    // Fechar o arquivo de entrada se não for stdin
    if (input != stdin)
    {
        fclose(input);
    }

    return exit_code;
}
