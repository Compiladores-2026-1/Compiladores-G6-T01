#ifndef TOKENS_H
#define TOKENS_H

enum TokenType
{
    INTEGER_LITERAL = 256,
    FLOAT_LITERAL,
    CHAR_LITERAL
};

typedef union
{
    int intval;
    float floatval;
    char charval;
    char *str;
} YYSTYPE;

extern YYSTYPE yylval;

#endif
