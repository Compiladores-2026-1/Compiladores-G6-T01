# Cria a biblioteca com os seus runners
add_library(runners STATIC
    lexer_runner.cpp
    parser_runner.cpp
)

# Linka com o lexer e o parser. Usar PUBLIC aqui faz com que
# qualquer um que use "runners" (como o main.cpp) também ganhe
# acesso aos includes do lexer e do parser.
target_link_libraries(runners PUBLIC lexer parser)

# O runners precisa incluir o diretório "include" para acessar os arquivos de cabeçalho.
target_include_directories(runners PUBLIC ${CMAKE_SOURCE_DIR}/cli)
