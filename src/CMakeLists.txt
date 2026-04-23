# Adiciona os subdiretórios
add_subdirectory(parser)
add_subdirectory(lexer)

# Cria o executável do compilador
add_executable(compilador main.cpp)

# Linka as bibliotecas ao executável do compilador
target_link_libraries(compilador PRIVATE lexer parser)

# Define o diretório de saída para o executável do compilador
set_target_properties(compilador PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
