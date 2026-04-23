# Roda o Bison e diz onde jogar os arquivos
BISON_TARGET(
    Parser
    parser.y
    ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.hpp
)

# Cria uma biblioteca estática apenas com o código gerado pelo Bison
add_library(parser STATIC ${BISON_Parser_OUTPUTS})

# Permite que quem usar essa biblioteca consiga enxergar o parser.hpp gerado
target_include_directories(parser PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include # Adiciona o diretório de include do projeto para que o parser possa encontrar os headers necessários
)
