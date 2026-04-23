%option noyywrap

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokens.hpp"

int line = 1;
%}

DIGIT [0-9]
LETTER [a-zA-Z_]
IDENTIFIER {LETTER}({LETTER}|{DIGIT})*
INTEGER_LITERAL {DIGIT}+
EXPONENT [eE][+-]?{DIGIT}+
FRACTION ({DIGIT}+"."{DIGIT}*|"."{DIGIT}+)
DOUBLE_LITERAL ({FRACTION}({EXPONENT})?|{DIGIT}+{EXPONENT})
FLOAT_LITERAL ({DOUBLE_LITERAL}[fF])
CHAR_LITERAL \'([^\\\']|\\.)\'
WHITESPACE [ \t\r]+
NEWLINE \n
COMMENT "//".*
COMMENT_BLOCK "/*"([^*]|(\*+[^*/]))*\*+"/"

%%

{WHITESPACE}    { /* Ignore whitespace */ }
{NEWLINE}       { line++; }

{COMMENT}       { /* Ignore comments */ }
{COMMENT_BLOCK} { /* Ignore block comments */ }

"int"        { return INT; }
"float"      { return FLOAT; }
"double"     { return DOUBLE; }
"char"       { return CHAR; }
"bool"       { return BOOL; }
"void"       { return VOID; }

"if"         { return IF; }
"else"       { return ELSE; }
"for"        { return FOR; }
"while"      { return WHILE; }

"break"      { return BREAK; }
"continue"   { return CONTINUE; }
"return"     { return RETURN; }

"main"       { return MAIN; }

"and"        { return AND; }
"or"         { return OR; }
"not"        { return NOT; }

"true" { yylval.intval = 1; return BOOL_LITERAL; }
"false" { yylval.intval = 0; return BOOL_LITERAL; }

"++"         { return INC; }
"--"         { return DEC; }

"+="         { return PLUSEQ; }
"-="         { return MINUSEQ; }
"*="         { return MULTEQ; }
"/="         { return DIVEQ; }
"%="         { return MODEQ; }

">="         { return GE; }
"<="         { return LE; }
"=="         { return EQ; }
"!="         { return NEQ; }

">"          { return GT; }
"<"          { return LT; }
"="          { return ASSIGN; }

"+"          { return PLUS; }
"-"          { return MINUS; }
"*"          { return MULT; }
"/"          { return DIV; }
"%"          { return MOD; }

"!"          { return NOT_OP; }

"("          { return LPAREN; }
")"          { return RPAREN; }
"{"          { return LBRACE; }
"}"          { return RBRACE; }
";"          { return SEMICOLON; }
","          { return COMMA; }

{INTEGER_LITERAL}    {
                yylval.intval = atoi(yytext);
                return INTEGER_LITERAL;
            }

{FLOAT_LITERAL}  {
                yylval.floatval = strtof(yytext, NULL);
                return FLOAT_LITERAL;
            }

{DOUBLE_LITERAL}  {
                yylval.doubleval = strtod(yytext, NULL);
                return DOUBLE_LITERAL;
            }

{CHAR_LITERAL}  {
                int ascii_val = 0;

                if (yytext[1] == '\\') {
                    switch(yytext[2]) {
                        case 'n': ascii_val = 10; break; // Nova linha
                        case 't': ascii_val = 9;  break; // Tab
                        case 'r': ascii_val = 13; break; // Carriage return
                        case '0': ascii_val = 0;  break; // Null
                        default:  ascii_val = yytext[2]; break; // Outros escapes
                    }
                } else {
                    ascii_val = (int)yytext[1];
                }

                yylval.intval = ascii_val;
                return INTEGER_LITERAL;
            }

{IDENTIFIER}    {
                    yylval.strval = strdup(yytext);
                    return IDENTIFIER;
                }

. {
    fprintf(stderr, "Erro léxico: '%s' na linha %d\n", yytext, line);
    return UNKNOWN_TOKEN;
}

%%
