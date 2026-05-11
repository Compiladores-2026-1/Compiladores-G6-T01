# Adiciona os subdiretórios
add_subdirectory(parser)
add_subdirectory(lexer)
add_subdirectory(ast)
add_subdirectory(symtable)
add_subdirectory(semantic)
add_subdirectory(tac)
add_subdirectory(optimizer)
add_subdirectory(runners)

# Cria o executável do compilador
add_executable(compilador main.cpp)

# Linka as bibliotecas ao executável do compilador
target_link_libraries(compilador PRIVATE runners)

# Define o diretório de saída para o executável do compilador
set_target_properties(compilador PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
