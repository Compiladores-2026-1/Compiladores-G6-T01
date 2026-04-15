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

/* Program */

program
    : main_function {
        printf("[PROGRAM]\n");
    }
    | global_declaration_list main_function {
        printf("[PROGRAM]\n");
    }
    ;

/* Global Declarations */

global_declaration_list
    : global_declaration {
        printf("[GLOBAL_LIST]\n");
    }
    | global_declaration_list global_declaration {
        printf("[GLOBAL_LIST]\n");
    }
    ;

global_declaration
    : type_specifier IDENTIFIER global_declaration_tail {
        printf("[GLOBAL_DECL: %s]\n", $2);
    }
    ;

global_declaration_tail
    : LPAREN optional_parameters RPAREN block {
        printf("[FUNCTION_DEF]\n");
    }
    | LPAREN optional_parameters RPAREN SEMICOLON {
        printf("[FUNCTION_DECL]\n");
    }
    | global_variable_tail SEMICOLON {
        printf("[GLOBAL_VAR]\n");
    }
    ;

global_variable_tail
    : optional_initializer global_variable_items_tail
    ;

global_variable_items_tail
    : COMMA IDENTIFIER optional_initializer global_variable_items_tail {
        printf("[GLOBAL_VAR_ITEM: %s]\n", $2);
    }
    | /* empty */
    ;

/* Main */

main_function
    : INT MAIN LPAREN RPAREN block {
        printf("[MAIN]\n");
    }
    ;

/* Types */

type_specifier
    : INT   { printf("[TYPE:int]\n"); }
    | FLOAT { printf("[TYPE:float]\n"); }
    | DOUBLE { printf("[TYPE:double]\n"); }
    | CHAR  { printf("[TYPE:char]\n"); }
    | BOOL  { printf("[TYPE:bool]\n"); }
    | VOID  { printf("[TYPE:void]\n"); }
    ;

/* Initialization */

optional_initializer
    : ASSIGN expression {
        printf("[INIT]\n");
    }
    | /* empty */
    ;

/* Parameters */

optional_parameters
    : parameter_list
    | /* empty */
    ;

parameter_list
    : parameter parameter_list_tail {
        printf("[PARAMS]\n");
    }
    ;

parameter_list_tail
    : COMMA parameter parameter_list_tail
    | /* empty */
    ;

parameter
    : type_specifier IDENTIFIER {
        printf("[PARAM: %s]\n", $2);
    }
    ;

/* Blocks */

block
    : LBRACE local_declaration_list statement_list RBRACE {
        printf("[BLOCK]\n");
    }
    ;

/* Local Declarations */

local_declaration_list
    : local_declaration local_declaration_list
    | /* empty */
    ;

local_declaration
    : type_specifier IDENTIFIER LPAREN optional_parameters RPAREN SEMICOLON {
        printf("[FUNCTION_DECL: %s]\n", $2);
    }
    | type_specifier declaration_item_list SEMICOLON {
        printf("[DECL]\n");
    }
    ;

declaration_item_list
    : IDENTIFIER optional_initializer declaration_item_list_tail {
        printf("[VAR: %s]\n", $1);
    }
    ;

declaration_item_list_tail
    : COMMA IDENTIFIER optional_initializer declaration_item_list_tail {
        printf("[VAR: %s]\n", $2);
    }
    | /* empty */
    ;

/* Statements */

statement_list
    : statement statement_list
    | /* empty */
    ;

statement
    : matched_statement
    | unmatched_statement
    ;


jump_statement
    : BREAK SEMICOLON {
        printf("[BREAK]\n");
    }
    | CONTINUE SEMICOLON {
        printf("[CONTINUE]\n");
    }
    | RETURN optional_expression SEMICOLON {
        printf("[RETURN]\n");
    }
    ;


matched_statement
    : block
    | jump_statement
    | expression_statement {
        printf("[STMT]\n");
    }
    | WHILE LPAREN expression RPAREN matched_statement {
        printf("[WHILE]\n");
    }
    | FOR LPAREN optional_for_initializer SEMICOLON optional_for_condition SEMICOLON optional_for_step RPAREN matched_statement {
        printf("[FOR]\n");
    }
    | IF LPAREN expression RPAREN matched_statement ELSE matched_statement {
        printf("[ELSE]\n");
        printf("[IF]\n");
    }
    ;

unmatched_statement
    : IF LPAREN expression RPAREN statement {
        printf("[IF]\n");
    }
    | IF LPAREN expression RPAREN matched_statement ELSE unmatched_statement {
        printf("[ELSE]\n");
        printf("[IF]\n");
    }
    | WHILE LPAREN expression RPAREN unmatched_statement {
        printf("[WHILE]\n");
    }
    | FOR LPAREN optional_for_initializer SEMICOLON optional_for_condition SEMICOLON optional_for_step RPAREN unmatched_statement {
        printf("[FOR]\n");
    }
    ;

/* For Loop Components */

optional_for_initializer
    : expression
    | type_specifier for_declaration_item_list
    | /* empty */
    ;

for_declaration_item_list
    : IDENTIFIER optional_initializer for_declaration_item_list_tail {
        printf("[VAR: %s]\n", $1);
    }
    ;

for_declaration_item_list_tail
    : COMMA IDENTIFIER optional_initializer for_declaration_item_list_tail {
        printf("[VAR: %s]\n", $2);
    }
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

/* Expressions */

expression_statement
    : optional_expression SEMICOLON {
        printf("[EXPR_STMT]\n");
    }
    ;

optional_expression
    : expression
    | /* empty */
    ;

expression
    : assignment_expression
    ;

/* Assignment, AND, OR, Relational, Additive, Multiplicative, Unary, Primary Expressions */

assignment_expression
    : IDENTIFIER assignment_operator assignment_expression {
        printf("[ASSIGN: %s]\n", $1);
    }
    | logical_or_expression
    ;

assignment_operator
    : ASSIGN
    | PLUSEQ
    | MINUSEQ
    | MULTEQ
    | DIVEQ
    | MODEQ
    ;

logical_or_expression
    : logical_and_expression logical_or_expression_tail
    ;

logical_or_expression_tail
    : OR logical_and_expression logical_or_expression_tail {
        printf("[OR]\n");
    }
    | /* empty */
    ;

logical_and_expression
    : relational_expression logical_and_expression_tail
    ;

logical_and_expression_tail
    : AND relational_expression logical_and_expression_tail {
        printf("[AND]\n");
    }
    | /* empty */
    ;

relational_expression
    : additive_expression relational_expression_tail
    ;

relational_expression_tail
    : relational_operator additive_expression {
        printf("[REL_OP]\n");
    }
    | /* empty */
    ;

relational_operator
    : GT | LT | GE | LE | EQ | NEQ
    ;

additive_expression
    : multiplicative_expression additive_expression_tail
    ;

additive_expression_tail
    : PLUS multiplicative_expression additive_expression_tail {
        printf("[PLUS]\n");
    }
    | MINUS multiplicative_expression additive_expression_tail {
        printf("[MINUS]\n");
    }
    | /* empty */
    ;

multiplicative_expression
    : unary_expression multiplicative_expression_tail
    ;

multiplicative_expression_tail
    : MULT unary_expression multiplicative_expression_tail {
        printf("[MULT]\n");
    }
    | DIV unary_expression multiplicative_expression_tail {
        printf("[DIV]\n");
    }
    | MOD unary_expression multiplicative_expression_tail {
        printf("[MOD]\n");
    }
    | /* empty */
    ;

unary_expression
    : unary_operator unary_expression {
        printf("[UNARY]\n");
    }
    | IDENTIFIER INC {
        printf("[POST_INC: %s]\n", $1);
    }
    | IDENTIFIER DEC {
        printf("[POST_DEC: %s]\n", $1);
    }
    | primary_expression
    ;

unary_operator
    : NOT_OP { printf("[NOT]\n"); }
    | NOT    { printf("[NOT]\n"); }
    | MINUS  { printf("[NEG]\n"); }
    | INC    { printf("[INC]\n"); }
    | DEC    { printf("[DEC]\n"); }
    ;

primary_expression
    : IDENTIFIER optional_call_suffix {
        printf("[ID: %s]\n", $1);
    }
    | INTEGER_LITERAL {
        printf("[NUM: %d]\n", $1);
    }
    | FLOAT_LITERAL {
        printf("[FLOAT: %f]\n", $1);
    }
    | DOUBLE_LITERAL {
        printf("[DOUBLE: %lf]\n", $1);
    }
    | BOOL_LITERAL {
        printf("[BOOL: %d]\n", $1);
    }
    | CHAR_LITERAL {
        printf("[CHAR]\n");
    }
    | UNKNOWN_TOKEN {
        yyerror("Token desconhecido ou caractere inválido detectado.");
    }
    | LPAREN expression RPAREN
    ;

/* Function Calls */

optional_call_suffix
    : LPAREN optional_argument_list RPAREN {
        printf("[CALL]\n");
    }
    | /* empty */
    ;

/* Arguments */

optional_argument_list
    : argument_list
    | /* empty */
    ;

argument_list
    : expression argument_list_tail
    ;

argument_list_tail
    : COMMA expression argument_list_tail
    | /* empty */
    ;

%%


void yyerror(const char *s)
{
    fprintf(stderr, "Erro sintático na linha %d: %s\n", line, s);
}
