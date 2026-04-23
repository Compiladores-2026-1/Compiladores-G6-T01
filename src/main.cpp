#include <iostream>
#include <string>
#include <cstdio> // Para FILE*, stdin, fopen, perror

#include "lexer_runner.hpp"
#include "parser_runner.hpp"

// Ponteiro para o arquivo de entrada, exigido pelo Flex
extern FILE *yyin;

// Uso de 'enum class' (fortemente tipado do C++11 em diante)
enum class ExecutionMode
{
    LEXER,
    PARSER
};

static void print_usage(const std::string &program_name)
{
    std::cerr << "Uso: " << program_name << " <--lexer | --parser> [arquivo_entrada]\n"
              << "Exemplos:\n"
              << "  " << program_name << " --parser ./tests/parser/teste_parser.txt\n"
              << "  " << program_name << " --lexer ./tests/parser/teste_parser.txt\n";
}

int main(int argc, char **argv)
{
    ExecutionMode mode = ExecutionMode::LEXER;
    bool mode_defined = false;
    std::string input_path = "";
    FILE *input = stdin;
    std::string program_name = argv[0];

    // Processamento de Argumentos com C++ std::string
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg == "--lexer")
        {
            if (mode_defined && mode != ExecutionMode::LEXER)
            {
                std::cerr << "Erro: use apenas uma flag de modo.\n";
                print_usage(program_name);
                return 1;
            }
            mode = ExecutionMode::LEXER;
            mode_defined = true;
            continue;
        }

        if (arg == "--parser")
        {
            if (mode_defined && mode != ExecutionMode::PARSER)
            {
                std::cerr << "Erro: use apenas uma flag de modo.\n";
                print_usage(program_name);
                return 1;
            }
            mode = ExecutionMode::PARSER;
            mode_defined = true;
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
            std::cerr << "Erro: informe no maximo um arquivo de entrada.\n";
            print_usage(program_name);
            return 1;
        }

        input_path = arg;
    }

    if (!mode_defined)
    {
        std::cerr << "Erro: informe uma flag de modo (--lexer ou --parser).\n";
        print_usage(program_name);
        return 1;
    }

    // Configuração do Arquivo de Entrada
    if (!input_path.empty())
    {
        input = fopen(input_path.c_str(), "r");
        if (input == nullptr)
        {
            perror("Erro ao abrir arquivo de entrada");
            return 1;
        }
    }

    yyin = input;

    if (input == stdin)
    {
        std::cout << "Digite a entrada para testar o "
                  << (mode == ExecutionMode::LEXER ? "lexer" : "parser")
                  << " (Ctrl+D para sair):\n";
    }

    // Delegação para o módulo correto
    int exit_code = (mode == ExecutionMode::LEXER) ? run_lexer_mode() : run_parser_mode();

    if (input != stdin)
    {
        fclose(input);
    }

    return exit_code;
}
