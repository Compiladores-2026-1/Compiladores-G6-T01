add_library(runners STATIC
    lexer_runner.cpp
    parser_runner.cpp
    ast_runner.cpp
)

target_link_libraries(runners PUBLIC lexer parser ast)
