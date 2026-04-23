#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokens.hpp"

/* Declaração da função do lexer gerada pelo Flex */
extern int yylex(void);   // Declaração da função do lexer gerada pelo Flex
extern char *yytext;      // Ponteiro para o texto do token atual, definido pelo Flex
extern FILE *yyin;        // Ponteiro para o arquivo de entrada, definido pelo Flex
extern int yyparse(void); // Declaração da função do parser gerada pelo Bison

typedef enum
{
    MODE_LEXER,
    MODE_PARSER
} ExecutionMode;

static void print_usage(const char *program_name)
{
    fprintf(stderr,
            "Uso: %s <--lexer | --parser> [arquivo_entrada]\n"
            "Exemplos:\n"
            "  %s --parser ./tests/parser/teste_parser.txt\n"
            "  %s --lexer ./tests/parser/teste_parser.txt\n",
            program_name,
            program_name,
            program_name);
}

static const char *token_to_string(int token)
{
    switch (token)
    {
    case INT:
        return "INT";
    case FLOAT:
        return "FLOAT";
    case DOUBLE:
        return "DOUBLE";
    case CHAR:
        return "CHAR";
    case BOOL:
        return "BOOL";
    case VOID:
        return "VOID";
    case IF:
        return "IF";
    case ELSE:
        return "ELSE";
    case FOR:
        return "FOR";
    case WHILE:
        return "WHILE";
    case BREAK:
        return "BREAK";
    case CONTINUE:
        return "CONTINUE";
    case RETURN:
        return "RETURN";
    case MAIN:
        return "MAIN";
    case AND:
        return "AND";
    case OR:
        return "OR";
    case NOT:
        return "NOT";
    case PLUS:
        return "PLUS";
    case MINUS:
        return "MINUS";
    case MULT:
        return "MULT";
    case DIV:
        return "DIV";
    case MOD:
        return "MOD";
    case ASSIGN:
        return "ASSIGN";
    case PLUSEQ:
        return "PLUSEQ";
    case MINUSEQ:
        return "MINUSEQ";
    case MULTEQ:
        return "MULTEQ";
    case DIVEQ:
        return "DIVEQ";
    case MODEQ:
        return "MODEQ";
    case EQ:
        return "EQ";
    case NEQ:
        return "NEQ";
    case LT:
        return "LT";
    case GT:
        return "GT";
    case LE:
        return "LE";
    case GE:
        return "GE";
    case INC:
        return "INC";
    case DEC:
        return "DEC";
    case NOT_OP:
        return "NOT_OP";
    case LPAREN:
        return "LPAREN";
    case RPAREN:
        return "RPAREN";
    case LBRACE:
        return "LBRACE";
    case RBRACE:
        return "RBRACE";
    case SEMICOLON:
        return "SEMICOLON";
    case COMMA:
        return "COMMA";
    case IDENTIFIER:
        return "IDENTIFIER";
    case INTEGER_LITERAL:
        return "INTEGER_LITERAL";
    case FLOAT_LITERAL:
        return "FLOAT_LITERAL";
    case DOUBLE_LITERAL:
        return "DOUBLE_LITERAL";
    case CHAR_LITERAL:
        return "CHAR_LITERAL";
    case BOOL_LITERAL:
        return "BOOL_LITERAL";
    default:
        return "UNKNOWN_TOKEN";
    }
}

static int run_lexer_mode(void)
{
    int token;
    int first_token = 1;

    while ((token = yylex()) != 0)
    {
        if (!first_token)
        {
            printf(" ");
        }

        printf("%s", token_to_string(token));
        first_token = 0;
    }

    printf("\n");
    printf("Fim da analise léxica.\n");
    return 0;
}

static int run_parser_mode(void)
{
    int parse_result = yyparse();

    if (parse_result == 0)
    {
        printf("Fim da analise sintática.\n");
        return 0;
    }

    fprintf(stderr, "Falha na analise sintática.\n");
    return 1;
}

int main(int argc, char **argv)
{
    ExecutionMode mode = MODE_LEXER;
    int mode_defined = 0;
    const char *input_path = NULL;
    FILE *input = stdin;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--lexer") == 0)
        {
            if (mode_defined && mode != MODE_LEXER)
            {
                fprintf(stderr, "Erro: use apenas uma flag de modo.\n");
                print_usage(argv[0]);
                return 1;
            }
            mode = MODE_LEXER;
            mode_defined = 1;
            continue;
        }

        if (strcmp(argv[i], "--parser") == 0)
        {
            if (mode_defined && mode != MODE_PARSER)
            {
                fprintf(stderr, "Erro: use apenas uma flag de modo.\n");
                print_usage(argv[0]);
                return 1;
            }
            mode = MODE_PARSER;
            mode_defined = 1;
            continue;
        }

        if (argv[i][0] == '-')
        {
            fprintf(stderr, "Erro: flag desconhecida: %s\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }

        if (input_path != NULL)
        {
            fprintf(stderr, "Erro: informe no máximo um arquivo de entrada.\n");
            print_usage(argv[0]);
            return 1;
        }

        input_path = argv[i];
    }

    if (!mode_defined)
    {
        fprintf(stderr, "Erro: informe uma flag de modo (--lexer ou --parser).\n");
        print_usage(argv[0]);
        return 1;
    }

    if (input_path != NULL)
    {
        input = fopen(input_path, "r");
        if (input == NULL)
        {
            perror("Erro ao abrir arquivo de entrada");
            return 1;
        }
    }

    yyin = input;

    if (input == stdin)
    {
        printf("Digite a entrada para testar o %s (Ctrl+D para sair):\n",
               mode == MODE_LEXER ? "lexer" : "parser");
    }

    int exit_code = (mode == MODE_LEXER) ? run_lexer_mode() : run_parser_mode();

    if (input != stdin)
    {
        fclose(input);
    }

    return exit_code;
}
