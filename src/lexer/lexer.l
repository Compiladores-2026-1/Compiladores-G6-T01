%option noyywrap

%{
#include <stdio.h>
#include "lexer/tokens.h"

int line = 1;
%}

DIGIT [0-9]
INTNUM {DIGIT}+
FLOATNUM {DIGIT}+"."{DIGIT}*
CHAR_LITERAL \'([^\\\']|\\.)\'

%%

{INTNUM}    {
                yylval.intval = atoi(yytext);
                return NUM;
            }
{FLOATNUM}  {
                yylval.floatval = atof(yytext);
                return NUM;
            }

{CHAR_LITERAL}  {
                    yylval.charval = yytext[1];
                    return CHAR_LITERAL;
                }

%%
