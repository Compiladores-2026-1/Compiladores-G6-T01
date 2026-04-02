#ifndef TOKENS_H
#define TOKENS_H

enum TokenType
{
    /* Keywords */
    /* Types */
    INT = 256,
    FLOAT,
    CHAR,
    BOOL,
    VOID,

    /* Control Flow */
    IF,
    ELSE,
    FOR,
    WHILE,

    /* Statements */
    BREAK,
    CONTINUE,
    RETURN,

    /* Main */
    MAIN,

    /* Operators */
    AND,
    OR,
    NOT,

    /* Literals */
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
