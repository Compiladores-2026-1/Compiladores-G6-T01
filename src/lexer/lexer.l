%option noyywrap

%{
#include <stdio.h>
#include "lexer/tokens.h"

int line = 1;
%}

DIGIT [0-9]
INTNUM {DIGIT}+

%%

{INTNUM} { return NUM; }

%%
