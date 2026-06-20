#pragma once

#include "tokens.hpp"
#include "ASTPrinter.hpp"
#include "semantic.hpp"
#include "tac.hpp"
#include "optimizer.hpp"
#include "codegen.hpp"

int run_lexer_mode();
int run_parser_mode();
int run_ast_mode();
int run_semantic_mode(bool debugMode);
int run_tac_mode(bool debugMode);
int run_opt_mode(bool debugMode);
int run_codegen_mode(bool debugMode);
int run_tac_mode();
int run_opt_mode();
int run_codegen_mode();
