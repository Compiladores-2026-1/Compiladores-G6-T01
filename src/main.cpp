#include <iostream>
#include <string>
#include <cstdio>
#include <vector>

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
    MODE_CODEGEN,
    MODE_EXE
} ExecutionMode;

static void print_usage(const std::string &program_name)
{
    std::cerr << "Uso: " << program_name << " [opcoes] arquivo_entrada\n\n"
              << "Opcoes:\n"
              << "  -h, --help               Exibe esta mensagem de ajuda e sai\n"
              << "  -o <arquivo>             Coloca a saida em <arquivo>\n"
              << "  -E, --lexer              Executa apenas o Lexer e imprime os tokens\n"
              << "  -fsyntax-only, --parser  Executa apenas o Parser e valida a sintaxe\n"
              << "  -fdump-ast, --ast        Gera e imprime a Arvore Sintatica Abstrata (AST)\n"
              << "  --semantic               Executa a analise semantica\n"
              << "  -fdump-tac, --tac        Gera o Codigo de Tres Enderecos (TAC)\n"
              << "  -O, --opt                Executa a otimizacao de codigo TAC\n"
              << "  -S, --codegen            Gera apenas o codigo Assembly e para (sem gerar binario)\n"
              << "  --symtable               Imprime a tabela de simbolos nas fases (semantica, tac, opt, codegen e modo padrao)\n\n"
              << "O comportamento padrao (sem flags de estagio) e gerar o binario executavel final usando gcc.\n\n"
              << "Exemplos:\n"
              << "  " << program_name << " programa.cmm                   (Gera o binario a.out)\n"
              << "  " << program_name << " programa.cmm -o meu_prog       (Gera o binario com o nome meu_prog)\n"
              << "  " << program_name << " -S programa.cmm -o saida.s     (Apenas gera o assembly e salva em saida.s)\n"
              << "  " << program_name << " -fdump-ast programa.cmm        (Apenas processa e exibe a AST)\n";
}

static std::string get_base_name(const std::string &path)
{
    size_t last_slash = path.find_last_of("/\\");
    std::string file_name = (last_slash == std::string::npos) ? path : path.substr(last_slash + 1);
    size_t last_dot = file_name.find_last_of(".");
    if (last_dot != std::string::npos)
    {
        return file_name.substr(0, last_dot);
    }
    return file_name;
}

int main(int argc, char **argv)
{
    // Comportamento padrao: gerar binario executavel
    ExecutionMode mode = MODE_EXE;

    bool debug_symtable = false;
    std::string input_path = "";
    std::string output_path = "";
    std::string program_name = argv[0];

    // Processar os argumentos de linha de comando
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg == "-o")
        {
            if (i + 1 < argc)
            {
                output_path = argv[++i];
            }
            else
            {
                std::cerr << "Erro: a opcao -o exige um argumento (arquivo de saida).\n";
                return 1;
            }
        }
        else if (arg == "-E" || arg == "--lexer") mode = MODE_LEXER;
        else if (arg == "-fsyntax-only" || arg == "--parser") mode = MODE_PARSER;
        else if (arg == "-fdump-ast" || arg == "--ast") mode = MODE_AST;
        else if (arg == "--semantic") mode = MODE_SEMANTIC;
        else if (arg == "-fdump-tac" || arg == "--tac") mode = MODE_TAC;
        else if (arg == "-O" || arg == "--opt") mode = MODE_OPT;
        else if (arg == "-S" || arg == "--codegen") mode = MODE_CODEGEN;
        else if (arg == "--symtable") debug_symtable = true;
        else if (arg == "-h" || arg == "--help")
        {
            print_usage(program_name);
            return 0;
        }
        else if (arg[0] == '-')
        {
            std::cerr << "Erro: flag desconhecida: " << arg << "\n";
            print_usage(program_name);
            return 1;
        }
        else
        {
            if (!input_path.empty())
            {
                std::cerr << "Erro: informe no maximo um arquivo de entrada.\n";
                print_usage(program_name);
                return 1;
            }
            input_path = arg;
        }
    }

    // GCC exige um arquivo de entrada explicitamente
    if (input_path.empty())
    {
        std::cerr << "Erro: nenhum arquivo de entrada fornecido.\n";
        print_usage(program_name);
        return 1;
    }

    // Se o usuario nao passou um arquivo de saida com -o, e o modo nao for MODE_EXE,
    // geramos um nome de arquivo padrao baseado no input e no modo (para nao jogar no terminal)
    if (output_path.empty() && mode != MODE_EXE)
    {
        std::string base_name = get_base_name(input_path);
        switch (mode)
        {
        case MODE_LEXER: output_path = base_name + ".lex"; break;
        case MODE_PARSER: output_path = base_name + ".parser"; break;
        case MODE_AST: output_path = base_name + ".ast"; break;
        case MODE_SEMANTIC: output_path = base_name + ".semantic"; break;
        case MODE_TAC: output_path = base_name + ".tac"; break;
        case MODE_OPT: output_path = base_name + ".opt"; break;
        case MODE_CODEGEN: output_path = base_name + ".s"; break;
        default: break;
        }
        std::cerr << "[Info] Saida redirecionada para o arquivo padrao: " << output_path << "\n";
    }

    // Abrir o arquivo de entrada
    FILE *input = fopen(input_path.c_str(), "r");
    if (input == nullptr)
    {
        perror("Erro ao abrir arquivo de entrada");
        return 1;
    }
    yyin = input;

    // Se o modo for EXE, precisamos gravar o assembly num arquivo temporario primeiro.
    // Senao, se houver -o, redirecionamos para o arquivo de saida especificado.
    FILE *output = nullptr;
    std::string temp_asm_file = "temp_compiler_assembly.s";

    if (mode == MODE_EXE)
    {
        output = freopen(temp_asm_file.c_str(), "w", stdout);
        if (!output)
        {
            perror("Erro ao criar arquivo temporario de assembly");
            fclose(input);
            return 1;
        }
    }
    else if (!output_path.empty())
    {
        output = freopen(output_path.c_str(), "w", stdout);
        if (!output)
        {
            perror("Erro ao abrir arquivo de saida");
            fclose(input);
            return 1;
        }
    }

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
    case MODE_SEMANTIC:
        exit_code = run_semantic_mode(debug_symtable);
        break;
    case MODE_TAC:
        exit_code = run_tac_mode(debug_symtable);
        break;
    case MODE_OPT:
        exit_code = run_opt_mode(debug_symtable);
        break;
    case MODE_CODEGEN:
        exit_code = run_codegen_mode(debug_symtable);
        break;
    case MODE_EXE:
        exit_code = run_codegen_mode(debug_symtable);
        
        // Precisamos fechar o arquivo temporario para o GCC conseguir le-lo
        if (output)
        {
            fclose(output);
            output = nullptr;
        }

        // Se a compilacao ate o assembly teve sucesso, usamos o GCC para linkar
        if (exit_code == 0)
        {
            std::string final_out = output_path.empty() ? "a.out" : output_path;
            // Chama o GCC para linkar o assembly gerado no executavel final
            std::string cmd = "gcc " + temp_asm_file + " -o " + final_out;
            int ret = system(cmd.c_str());
            
            if (ret == 0)
            {
                // Remove o arquivo assembly temporario apos sucesso
                remove(temp_asm_file.c_str());
            }
            else
            {
                std::cerr << "Erro: falha ao gerar executavel final com GCC.\n";
                exit_code = 1;
            }
        }
        else
        {
            // Se falhou no assembly, remove o arquivo temporario que ficou imcompleto
            remove(temp_asm_file.c_str());
        }
        break;
    }

    // Limpeza
    fclose(input);
    if (output)
    {
        fclose(output);
    }

    return exit_code;
}
