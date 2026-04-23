%code requires {
    #include "ast.hpp"
    #include <vector>
    #include <string>
}

%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.hpp"

int yylex(void);
extern int line;
void yyerror(const char *s);

// Variável global para armazenar a raiz da AST
ProgramNode* root_ast = nullptr;
%}

/* Semantic Types */
%union {
    int intval;
    float floatval;
    double doubleval;
    char charval;
    char *strval;

    // Tipos de nós da AST
    class ASTNode* ast_node;
    class BlockNode* block_node;
    class FuncDeclNode* func_decl_node;
    class ProgramNode* prog_node;
    class VarDeclNode* var_decl_node;

    // Listas de nós
    std::vector<class ASTNode*>* node_list;
    std::vector<class VarDeclNode*>* param_list;
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

/* Tipos dos não-terminais */
%type <prog_node> program
%type <func_decl_node> main_function function_declaration
%type <intval> type_specifier assignment_operator unary_operator relational_operator

%type <node_list> global_declaration_list global_declaration
%type <node_list> local_declaration
%type <node_list> variable_declaration init_declarator_list
%type <node_list> statement_list optional_argument_list argument_list optional_call_suffix

%type <ast_node> init_declarator
%type <param_list> optional_parameters parameter_list
%type <var_decl_node> parameter

%type <block_node> block
%type <ast_node> statement matched_statement unmatched_statement jump_statement expression_statement
%type <ast_node> expression optional_expression assignment_expression
%type <ast_node> logical_or_expression logical_and_expression relational_expression
%type <ast_node> additive_expression multiplicative_expression unary_expression primary_expression
%type <ast_node> optional_initializer optional_for_initializer optional_for_condition optional_for_step

/* Start Symbol */
%start program

%%

/* =========================================================================
   Program & Globals
   ========================================================================= */

program
    : main_function {
        $$ = new ProgramNode();
        $$->declarations.push_back($1);
        root_ast = $$;
    }
    | global_declaration_list main_function {
        $$ = new ProgramNode();
        $$->declarations = *$1;
        $$->declarations.push_back($2);
        root_ast = $$;
        delete $1;
    }
    ;

global_declaration_list
    : global_declaration {
        $$ = $1;
    }
    | global_declaration_list global_declaration {
        $$ = $1;
        $$->insert($$->end(), $2->begin(), $2->end());
        delete $2;
    }
    ;

global_declaration
    : function_declaration {
        $$ = new std::vector<ASTNode*>{$1};
    }
    | variable_declaration SEMICOLON {
        $$ = $1;
    }
    ;

/* =========================================================================
   Main & Declarations
   ========================================================================= */

main_function
    : INT MAIN LPAREN RPAREN block {
        $$ = new FuncDeclNode(INT, "main", $5);
    }
    ;

function_declaration
    : type_specifier IDENTIFIER LPAREN optional_parameters RPAREN block {
        $$ = new FuncDeclNode($1, std::string($2), $6);
        if ($4) {
            $$->params = *$4;
            delete $4;
        }
    }
    | type_specifier IDENTIFIER LPAREN optional_parameters RPAREN SEMICOLON {
        $$ = new FuncDeclNode($1, std::string($2), nullptr); // Protótipo de função
        if ($4) {
            $$->params = *$4;
            delete $4;
        }
    }
    ;

variable_declaration
    : type_specifier init_declarator_list {
        $$ = $2;
        // Aplica o tipo base a todas as variáveis na lista (ex: int x, y, z;)
        for (auto node : *$$) {
            if (auto var_node = dynamic_cast<VarDeclNode*>(node)) {
                var_node->type_token = $1;
            }
        }
    }
    ;

init_declarator_list
    : init_declarator {
        $$ = new std::vector<ASTNode*>();
        $$->push_back($1);
    }
    | init_declarator_list COMMA init_declarator {
        $$ = $1;
        $$->push_back($3);
    }
    ;

init_declarator
    : IDENTIFIER optional_initializer {
        // Criamos com tipo 0 inicialmente, variable_declaration ajustará o tipo.
        $$ = new VarDeclNode(0, std::string($1), $2);
    }
    ;

/* Types */

type_specifier
    : INT    { $$ = INT; }
    | FLOAT  { $$ = FLOAT; }
    | DOUBLE { $$ = DOUBLE; }
    | CHAR   { $$ = CHAR; }
    | BOOL   { $$ = BOOL; }
    | VOID   { $$ = VOID; }
    ;

optional_initializer
    : ASSIGN expression { $$ = $2; }
    | /* empty */ { $$ = nullptr; }
    ;

/* =========================================================================
   Parameters
   ========================================================================= */

optional_parameters
    : parameter_list { $$ = $1; }
    | /* empty */ { $$ = nullptr; }
    ;

parameter_list
    : parameter {
        $$ = new std::vector<VarDeclNode*>();
        $$->push_back($1);
    }
    | parameter_list COMMA parameter {
        $$ = $1;
        $$->push_back($3);
    }
    ;

parameter
    : type_specifier IDENTIFIER {
        $$ = new VarDeclNode($1, std::string($2), nullptr);
    }
    ;

/* =========================================================================
   Blocks & Local Declarations
   ========================================================================= */

block
    : LBRACE statement_list RBRACE {
        $$ = new BlockNode();
        if ($2) {
            $$->statements.insert($$->statements.end(), $2->begin(), $2->end());
            delete $2;
        }
    }
    ;

local_declaration
    : function_declaration { $$ = new std::vector<ASTNode*>{$1}; }
    | variable_declaration SEMICOLON { $$ = $1; }
    ;

/* =========================================================================
   Statements
   ========================================================================= */

statement_list
    : /* empty */ { $$ = new std::vector<ASTNode*>(); }
    | statement_list statement {
        $$ = $1;
        if ($2) $$->push_back($2);
    }
    | statement_list local_declaration {
        $$ = $1;
        if ($2) {
            // local_declaration retorna um vector de ASTNode*, por isso inserimos os elementos
            $$->insert($$->end(), $2->begin(), $2->end());
            delete $2;
        }
    }
    ;

statement
    : matched_statement { $$ = $1; }
    | unmatched_statement { $$ = $1; }
    ;

jump_statement
    : BREAK SEMICOLON { $$ = new BreakNode(); } // Em vez de new IdNode("break")
    | CONTINUE SEMICOLON { $$ = new ContinueNode(); }
    | RETURN optional_expression SEMICOLON { $$ = new ReturnNode($2); }
    ;

matched_statement
    : block { $$ = $1; }
    | jump_statement { $$ = $1; }
    | expression_statement { $$ = $1; }
    | WHILE LPAREN expression RPAREN matched_statement {
        $$ = new WhileNode($3, $5);
    }
    | FOR LPAREN optional_for_initializer SEMICOLON optional_for_condition SEMICOLON optional_for_step RPAREN matched_statement {
        $$ = new ForNode($3, $5, $7, $9);
    }
    | IF LPAREN expression RPAREN matched_statement ELSE matched_statement {
        $$ = new IfNode($3, $5, $7);
    }
    ;

unmatched_statement
    : IF LPAREN expression RPAREN statement {
        $$ = new IfNode($3, $5, nullptr);
    }
    | IF LPAREN expression RPAREN matched_statement ELSE unmatched_statement {
        $$ = new IfNode($3, $5, $7);
    }
    | WHILE LPAREN expression RPAREN unmatched_statement {
        $$ = new WhileNode($3, $5);
    }
    | FOR LPAREN optional_for_initializer SEMICOLON optional_for_condition SEMICOLON optional_for_step RPAREN unmatched_statement {
        $$ = new ForNode($3, $5, $7, $9);
    }
    ;

/* For Loop Components */

optional_for_initializer
    : expression { $$ = $1; }
    | variable_declaration {
        // Se houver múltiplas declarações, agrupamos num bloco auxiliar para a AST.
        if ($1->size() == 1) {
            $$ = (*$1)[0];
            delete $1;
        } else {
            auto block = new BlockNode();
            block->statements = *$1;
            delete $1;
            $$ = block;
        }
    }
    | /* empty */ { $$ = nullptr; }
    ;

optional_for_condition
    : expression { $$ = $1; }
    | /* empty */ { $$ = nullptr; }
    ;

optional_for_step
    : expression { $$ = $1; }
    | /* empty */ { $$ = nullptr; }
    ;

/* =========================================================================
   Expressions
   ========================================================================= */

expression_statement
    : optional_expression SEMICOLON { $$ = $1; }
    ;

optional_expression
    : expression { $$ = $1; }
    | /* empty */ { $$ = nullptr; }
    ;

expression
    : assignment_expression { $$ = $1; }
    ;

assignment_expression
    : logical_or_expression { $$ = $1; }
    | IDENTIFIER assignment_operator assignment_expression {
        $$ = new AssignNode(std::string($1), $2, $3);
    }
    ;

assignment_operator
    : ASSIGN  { $$ = ASSIGN; }
    | PLUSEQ  { $$ = PLUSEQ; }
    | MINUSEQ { $$ = MINUSEQ; }
    | MULTEQ  { $$ = MULTEQ; }
    | DIVEQ   { $$ = DIVEQ; }
    | MODEQ   { $$ = MODEQ; }
    ;

/* * Hierarquia de Precedência Usando Recursão à Esquerda (Padrão Bison)
 */

logical_or_expression
    : logical_and_expression { $$ = $1; }
    | logical_or_expression OR logical_and_expression {
        $$ = new BinOpNode(OR, $1, $3);
    }
    ;

logical_and_expression
    : relational_expression { $$ = $1; }
    | logical_and_expression AND relational_expression {
        $$ = new BinOpNode(AND, $1, $3);
    }
    ;

relational_expression
    : additive_expression { $$ = $1; }
    | relational_expression relational_operator additive_expression {
        $$ = new BinOpNode($2, $1, $3);
    }
    ;

relational_operator
    : GT  { $$ = GT; }
    | LT  { $$ = LT; }
    | GE  { $$ = GE; }
    | LE  { $$ = LE; }
    | EQ  { $$ = EQ; }
    | NEQ { $$ = NEQ; }
    ;

additive_expression
    : multiplicative_expression { $$ = $1; }
    | additive_expression PLUS multiplicative_expression {
        $$ = new BinOpNode(PLUS, $1, $3);
    }
    | additive_expression MINUS multiplicative_expression {
        $$ = new BinOpNode(MINUS, $1, $3);
    }
    ;

multiplicative_expression
    : unary_expression { $$ = $1; }
    | multiplicative_expression MULT unary_expression {
        $$ = new BinOpNode(MULT, $1, $3);
    }
    | multiplicative_expression DIV unary_expression {
        $$ = new BinOpNode(DIV, $1, $3);
    }
    | multiplicative_expression MOD unary_expression {
        $$ = new BinOpNode(MOD, $1, $3);
    }
    ;

unary_expression
    : primary_expression {
        $$ = $1;
    }
    | unary_operator unary_expression {
        $$ = new UnOpNode($1, $2);
    }
    | IDENTIFIER INC {
        $$ = new UnOpNode(POST_INC, new IdNode(std::string($1)));
    }
    | IDENTIFIER DEC {
        $$ = new UnOpNode(POST_DEC, new IdNode(std::string($1)));
    }
    ;

unary_operator
    : NOT_OP { $$ = NOT_OP; }
    | NOT    { $$ = NOT; }
    | MINUS  { $$ = MINUS; }
    | INC    { $$ = INC; }
    | DEC    { $$ = DEC; }
    ;

primary_expression
    : IDENTIFIER optional_call_suffix {
        if ($2) { // Se houver sufixo de chamada, é uma chamada de função
            auto call = new FuncCallNode(std::string($1));
            call->args = *$2;
            delete $2;
            $$ = call;
        } else {
            $$ = new IdNode(std::string($1));
        }
    }
    | INTEGER_LITERAL { $$ = new LiteralNode(INTEGER_LITERAL, std::to_string($1)); }
    | FLOAT_LITERAL   { $$ = new LiteralNode(FLOAT_LITERAL, std::to_string($1)); }
    | DOUBLE_LITERAL  { $$ = new LiteralNode(DOUBLE_LITERAL, std::to_string($1)); }
    | BOOL_LITERAL    { $$ = new LiteralNode(BOOL_LITERAL, std::to_string($1)); }
    | CHAR_LITERAL    { $$ = new LiteralNode(CHAR_LITERAL, std::string(1, $1)); }
    | UNKNOWN_TOKEN {
        yyerror("Token desconhecido ou caractere inválido detectado.");
        $$ = nullptr;
    }
    | LPAREN expression RPAREN { $$ = $2; }
    ;

/* Function Calls & Arguments */

optional_call_suffix
    : LPAREN optional_argument_list RPAREN { $$ = $2; }
    | /* empty */ { $$ = nullptr; }
    ;

optional_argument_list
    : /* empty */ { $$ = new std::vector<ASTNode*>(); }
    | argument_list { $$ = $1; }
    ;

argument_list
    : expression {
        $$ = new std::vector<ASTNode*>();
        $$->push_back($1);
    }
    | argument_list COMMA expression {
        $$ = $1;
        $$->push_back($3);
    }
    ;

%%

void yyerror(const char *s)
{
    fprintf(stderr, "Erro sintático na linha %d: %s\n", line, s);
}
