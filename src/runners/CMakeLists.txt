add_library(runners STATIC
    lexer_runner.cpp
    parser_runner.cpp
    ast_runner.cpp
    semantic_runner.cpp
    tac_runner.cpp
)

target_link_libraries(runners PUBLIC lexer parser ast symtable semantic tac)
