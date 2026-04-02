#include <stdio.h>
#include <stdlib.h>

#include "lexer/tokens.h"

/* Declaração da função do lexer gerada pelo Flex */
extern int yylex(void); // Declaração da função do lexer gerada pelo Flex
extern char *yytext;    // Ponteiro para o texto do token atual, definido pelo Flex
extern FILE *yyin;      // Ponteiro para o arquivo de entrada, definido pelo Flex

static const char *token_to_string(int token)
{
    switch (token)
    {
    case INT:
        return "INT";
    case FLOAT:
        return "FLOAT";
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
    case CHAR_LITERAL:
        return "CHAR_LITERAL";
    case BOOL_LITERAL:
        return "BOOL_LITERAL";
    default:
        return "UNKNOWN_TOKEN";
    }
}

int main(int argc, char **argv)
{
    int token;
    FILE *input = stdin;
    int first_token = 1;

    if (argc > 2)
    {
        fprintf(stderr, "Uso: %s [arquivo_entrada]\n", argv[0]);
        return 1;
    }

    if (argc == 2)
    {
        input = fopen(argv[1], "r");
        if (input == NULL)
        {
            perror("Erro ao abrir arquivo de entrada");
            return 1;
        }
    }

    yyin = input;

    if (argc == 1)
    {
        printf("Digite a entrada para testar o lexer (pressione Ctrl+D para sair):\n");
    }

    /* Loop para processar tokens ate o fim da entrada (EOF) */
    while ((token = yylex()) != 0)
    { /* yylex() retorna 0 no EOF */
        if (!first_token)
        {
            printf(" ");
        }

        printf("%s", token_to_string(token));
        first_token = 0;
    }

    printf("\n");

    if (input != stdin)
    {
        fclose(input);
    }

    printf("Fim da analise lexica.\n");
    return 0;
}
