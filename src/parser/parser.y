%code requires {
    /* Sem dependências da AST ou bibliotecas C++ */
}

%{
#include <stdio.h>
#include <stdlib.h>

int yylex(void);
extern int line;
void yyerror(const char *s);
%}

/* Semantic Types */
%union {
    int intval;
    float floatval;
    double doubleval;
    char charval;
    char *strval;
}

/* Tokens */
%token UNKNOWN_TOKEN
%token INT FLOAT DOUBLE CHAR BOOL VOID
%token IF ELSE FOR WHILE
%token BREAK CONTINUE RETURN
%token MAIN
%token AND OR NOT

%token PLUS MINUS MULT DIV MOD
%token ASSIGN PLUSEQ MINUSEQ MULTEQ DIVEQ MODEQ
%token EQ NEQ LT GT LE GE
%token INC DEC NOT_OP
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON COMMA

%token <strval> IDENTIFIER
%token <intval> INTEGER_LITERAL
%token <floatval> FLOAT_LITERAL
%token <doubleval> DOUBLE_LITERAL
%token <charval> CHAR_LITERAL
%token <intval> BOOL_LITERAL

/* Start Symbol */
%start program

%%

/* =========================================================================
   Program & Globals
   ========================================================================= */

program
    : main_function {
        printf("[PROGRAMA ANALISADO COM SUCESSO]\n");
    }
    | global_declaration_list main_function {
        printf("[PROGRAMA ANALISADO COM SUCESSO]\n");
    }
    ;

global_declaration_list
    : global_declaration
    | global_declaration_list global_declaration
    ;

global_declaration
    : function_declaration
    | variable_declaration SEMICOLON
    ;

/* =========================================================================
   Main & Declarations
   ========================================================================= */

main_function
    : INT MAIN LPAREN RPAREN block {
        printf("[FUNÇÃO PRINCIPAL: main]\n");
    }
    ;

function_declaration
    : type_specifier IDENTIFIER LPAREN optional_parameters RPAREN block {
        printf("[DECLARAÇÃO DE FUNÇÃO: %s]\n", $2);
    }
    | type_specifier IDENTIFIER LPAREN optional_parameters RPAREN SEMICOLON {
        printf("[PROTÓTIPO DE FUNÇÃO: %s]\n", $2);
    }
    ;

variable_declaration
    : type_specifier init_declarator_list {
        printf("[FIM DECLARAÇÃO DE VARIÁVEIS]\n");
    }
    ;

init_declarator_list
    : init_declarator
    | init_declarator_list COMMA init_declarator
    ;

init_declarator
    : IDENTIFIER optional_initializer {
        printf("[VARIÁVEL RECONHECIDA: %s]\n", $1);
    }
    ;

/* Types */

type_specifier
    : INT    {
        printf("[TIPO: INT]\n");
    }
    | FLOAT  {
        printf("[TIPO: FLOAT]\n");
    }
    | DOUBLE {
        printf("[TIPO: DOUBLE]\n");
    }
    | CHAR   {
        printf("[TIPO: CHAR]\n");
    }
    | BOOL   {
        printf("[TIPO: BOOL]\n");
    }
    | VOID   {
        printf("[TIPO: VOID]\n");
    }
    ;

optional_initializer
    : ASSIGN expression
    | /* empty */
    ;

/* =========================================================================
   Parameters
   ========================================================================= */

optional_parameters
    : parameter_list
    | /* empty */
    ;

parameter_list
    : parameter
    | parameter_list COMMA parameter
    ;

parameter
    : type_specifier IDENTIFIER {
        printf("[PARÂMETRO: %s]\n", $2);
    }
    ;

/* =========================================================================
   Blocks & Local Declarations
   ========================================================================= */

block
    : LBRACE statement_list RBRACE {
        printf("[BLOCO DE CÓDIGO FECHADO]\n");
    }
    ;

local_declaration_list
    : /* empty */
    | local_declaration_list local_declaration
    ;

local_declaration
    : function_declaration
    | variable_declaration SEMICOLON
    ;

/* =========================================================================
   Statements
   ========================================================================= */

statement_list
    : /* empty */
    | statement_list statement
    | statement_list local_declaration
    ;

statement
    : matched_statement
    | unmatched_statement
    ;

jump_statement
    : BREAK SEMICOLON {
        printf("[COMANDO: BREAK]\n");
    }
    | CONTINUE SEMICOLON {
        printf("[COMANDO: CONTINUE]\n");
    }
    | RETURN optional_expression SEMICOLON {
        printf("[COMANDO: RETURN]\n");
    }
    ;

matched_statement
    : block
    | jump_statement
    | expression_statement
    | WHILE LPAREN expression RPAREN matched_statement {
        printf("[LACO WHILE FECHADO]\n");
    }
    | FOR LPAREN optional_for_initializer SEMICOLON optional_for_condition SEMICOLON optional_for_step RPAREN matched_statement {
        printf("[LACO FOR FECHADO]\n");
    }
    | IF LPAREN expression RPAREN matched_statement ELSE matched_statement {
        printf("[IF-ELSE FECHADO]\n");
    }
    ;

unmatched_statement
    : IF LPAREN expression RPAREN statement {
        printf("[IF FECHADO]\n");
    }
    | IF LPAREN expression RPAREN matched_statement ELSE unmatched_statement {
        printf("[IF-ELSE FECHADO]\n");
    }
    | WHILE LPAREN expression RPAREN unmatched_statement {
        printf("[LACO WHILE FECHADO]\n");
    }
    | FOR LPAREN optional_for_initializer SEMICOLON optional_for_condition SEMICOLON optional_for_step RPAREN unmatched_statement {
        printf("[LACO FOR FECHADO]\n");
    }
    ;

/* For Loop Components */

optional_for_initializer
    : expression
    | variable_declaration
    | /* empty */
    ;

optional_for_condition
    : expression
    | /* empty */
    ;

optional_for_step
    : expression
    | /* empty */
    ;

/* =========================================================================
   Expressions
   ========================================================================= */

expression_statement
    : optional_expression SEMICOLON
    ;

optional_expression
    : expression
    | /* empty */
    ;

expression
    : assignment_expression
    ;

assignment_expression
    : logical_or_expression
    | IDENTIFIER assignment_operator assignment_expression {
        printf("[ATRIBUIÇÃO PARA: %s]\n", $1);
    }
    ;

assignment_operator
    : ASSIGN  {
        printf("[OP ASSIGN: =]\n");
    }
    | PLUSEQ  {
        printf("[OP ASSIGN: +=]\n");
    }
    | MINUSEQ {
        printf("[OP ASSIGN: -=]\n");
    }
    | MULTEQ  {
        printf("[OP ASSIGN: *=]\n");
    }
    | DIVEQ   {
        printf("[OP ASSIGN: /=]\n");
    }
    | MODEQ   {
        printf("[OP ASSIGN: %=]\n");
    }
    ;

logical_or_expression
    : logical_and_expression
    | logical_or_expression OR logical_and_expression {
        printf("[OP LOGICO: OR]\n");
    }
    ;

logical_and_expression
    : relational_expression
    | logical_and_expression AND relational_expression {
        printf("[OP LOGICO: AND]\n");
    }
    ;

relational_expression
    : additive_expression
    | relational_expression relational_operator additive_expression
    ;

relational_operator
    : GT  {
        printf("[OP RELACIONAL: >]\n");
    }
    | LT  {
        printf("[OP RELACIONAL: <]\n");
    }
    | GE  {
        printf("[OP RELACIONAL: >=]\n");
    }
    | LE  {
        printf("[OP RELACIONAL: <=]\n");
    }
    | EQ  {
        printf("[OP RELACIONAL: ==]\n");
    }
    | NEQ {
        printf("[OP RELACIONAL: !=]\n");
    }
    ;

additive_expression
    : multiplicative_expression
    | additive_expression PLUS multiplicative_expression {
        printf("[OP ARITMÉTICO: +]\n");
    }
    | additive_expression MINUS multiplicative_expression {
        printf("[OP ARITMÉTICO: -]\n");
    }
    ;

multiplicative_expression
    : unary_expression
    | multiplicative_expression MULT unary_expression {
        printf("[OP ARITMÉTICO: *]\n");
    }
    | multiplicative_expression DIV unary_expression {
        printf("[OP ARITMÉTICO: /]\n");
    }
    | multiplicative_expression MOD unary_expression {
        printf("[OP ARITMÉTICO: %%]\n");
    }
    ;

unary_expression
    : primary_expression
    | unary_operator unary_expression
    | IDENTIFIER INC {
        printf("[POS-INCREMENTO: %s]\n", $1);
    }
    | IDENTIFIER DEC {
        printf("[POS-DECREMENTO: %s]\n", $1);
    }
    ;

unary_operator
    : NOT_OP {
        printf("[OP UNÁRIO: ~]\n");
    }
    | NOT    {
        printf("[OP UNÁRIO: !]\n");
    }
    | MINUS  {
        printf("[OP UNÁRIO: -]\n");
    }
    | INC    {
        printf("[PRE-INCREMENTO]\n");
    }
    | DEC    {
        printf("[PRE-DECREMENTO]\n");
    }
    ;

/* Redefini a chamada de função direto aqui para facilitar a impressão sem precisar de nós */
primary_expression
    : IDENTIFIER {
        printf("[IDENTIFICADOR: %s]\n", $1);
    }
    | IDENTIFIER LPAREN optional_argument_list RPAREN {
        printf("[CHAMADA DE FUNÇÃO: %s]\n", $1);
    }
    | INTEGER_LITERAL {
        printf("[INTEGER: %d]\n", $1);
    }
    | FLOAT_LITERAL   {
        printf("[FLOAT: %f]\n", $1);
    }
    | DOUBLE_LITERAL  {
        printf("[DOUBLE: %lf]\n", $1);
    }
    | BOOL_LITERAL    {
        printf("[BOOL: %d]\n", $1);
    }
    | CHAR_LITERAL    {
        printf("[CHAR: %c]\n", $1);
    }
    | UNKNOWN_TOKEN {
        yyerror("Token desconhecido ou caractere invalido detectado.");
    }
    | LPAREN expression RPAREN
    ;

/* Function Arguments */

optional_argument_list
    : /* empty */
    | argument_list
    ;

argument_list
    : expression
    | argument_list COMMA expression
    ;

%%

void yyerror(const char *s)
{
    fprintf(stderr, "Erro sintático na linha %d: %s\n", line, s);
}
