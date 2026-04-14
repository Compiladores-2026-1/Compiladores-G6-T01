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

    /* Arithmetic Operators */
    PLUS,
    MINUS,
    MULT,
    DIV,
    MOD,

    /* Assignment Operators */
    ASSIGN,
    PLUSEQ,
    MINUSEQ,
    MULTEQ,
    DIVEQ,
    MODEQ,

    /* Comparison Operators */
    EQ,
    NEQ,
    LT,
    GT,
    LE,
    GE,

    /* Unary Operators */
    INC,
    DEC,
    NOT_OP,

    /* Delimiters */
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    SEMICOLON,
    COMMA,

    /* Literals */
    IDENTIFIER,
    INTEGER_LITERAL,
    FLOAT_LITERAL,
    CHAR_LITERAL,
    BOOL_LITERAL,

    /* Error Token */
    UNKNOWN_TOKEN
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
