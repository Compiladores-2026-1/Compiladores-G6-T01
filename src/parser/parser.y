%{
#include <stdio.h>

int yylex(void);
void yyerror(const char *msg);
%}


%token INT FLOAT CHAR BOOL
%token IDENTIFIER INTEGER_LITERAL
%token ASSIGN SEMICOLON
%token INCREMENT  

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