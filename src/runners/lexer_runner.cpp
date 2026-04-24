#include "runners.hpp"
#include <iostream>
#include <string>

// Declaração da função do lexer gerada pelo Flex
extern int yylex();

// Função auxiliar mantida restrita a este arquivo (static)
static std::string token_to_string(int token)
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

int run_lexer_mode()
{
    int token;
    bool first_token = true;

    // Fica lendo tokens até o Flex retornar 0 (fim de arquivo)
    while ((token = yylex()) != 0)
    {
        if (!first_token)
        {
            std::cout << " ";
        }
        std::cout << token_to_string(token);
        first_token = false;
    }

    std::cout << "\n\nFim da analise léxica.\n";
    return 0;
}
