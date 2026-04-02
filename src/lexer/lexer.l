%option noyywrap

%{
#include <stdio.h>
#include "lexer/tokens.h"

int line = 1;
%}

DIGIT [0-9]
INTEGER_LITERAL {DIGIT}+
FLOAT_LITERAL {DIGIT}+"."{DIGIT}*
CHAR_LITERAL \'([^\\\']|\\.)\'

%%
/* --- Literais --- */
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

%%
