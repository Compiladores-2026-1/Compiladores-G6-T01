%option noyywrap

%{
#include <stdio.h>
#include <stdlib.h>
#include "lexer/tokens.h"

int line = 1;
%}

DIGIT [0-9]
LETTER [a-zA-Z_]
IDENTIFIER {LETTER}({LETTER}|{DIGIT})*
INTEGER_LITERAL {DIGIT}+
FLOAT_LITERAL {DIGIT}+"."{DIGIT}*
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
"char"       { return CHAR; }
"bool"       { return BOOL; }
"void"       { return VOID; }

{INTEGER_LITERAL}    {
                yylval.intval = atoi(yytext);
                return INTEGER_LITERAL;
            }

{FLOAT_LITERAL}  {
                yylval.floatval = atof(yytext);
                return FLOAT_LITERAL;
            }

{CHAR_LITERAL}  {
                    yylval.charval = yytext[1];
                    return CHAR_LITERAL;
                }

{IDENTIFIER}    {
                    yylval.strval = strdup(yytext);
                    return IDENTIFIER;
                }

%%
