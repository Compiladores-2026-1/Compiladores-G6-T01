# Roda o Flex
FLEX_TARGET(
    Lexer
    lexer.l
    ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp
)

# Cria a biblioteca com o código gerado
add_library(lexer STATIC ${FLEX_Lexer_OUTPUTS})

# O Lexer precisa saber quais são os tokens definidos pelo Parser.
# Ao linkar com "parser", o lexer herda o diretório do parser.hpp automaticamente.
target_link_libraries(lexer PUBLIC parser)

# O lexer também precisa incluir o diretório "include" para acessar os arquivos de cabeçalho.
target_include_directories(lexer PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Adiciona a biblioteca fl (yywrap) se existir
find_library(FL_LIBRARY NAMES fl)
if(FL_LIBRARY)
    target_link_libraries(lexer PRIVATE ${FL_LIBRARY})
endif()
