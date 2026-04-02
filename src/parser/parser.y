%{
#include <stdio.h>

int yylex(void);
void yyerror(const char *msg);
%}

%start programa

%%

programa:
	/* vazio */
	;

%%

void yyerror(const char *msg)
{
	fprintf(stderr, "Erro sintatico: %s\n", msg);
}
