#ifndef TOKENS_H
#define TOKENS_H

enum TokenType
{

    INT = 256,
    FLOAT,
    CHAR,
    BOOL,
    VOID,

    IDENTIFIER,
    INTEGER_LITERAL,
    FLOAT_LITERAL,
    CHAR_LITERAL
};

typedef union
{
    int intval;
    float floatval;
    char charval;
    char *strval;
} YYSTYPE;

extern YYSTYPE yylval;

#endif
