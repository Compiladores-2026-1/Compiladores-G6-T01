#ifndef TOKENS_H
#define TOKENS_H

enum TokenType
{
    NUM = 256,
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
