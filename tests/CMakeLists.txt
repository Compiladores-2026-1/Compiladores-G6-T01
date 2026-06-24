include(FetchContent)

# Baixa o GoogleTest
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Cria o executável de testes
add_executable(testes
    lexer/teste_léxico.cpp
    parser/teste_sintatico_incorreto.cpp
    parser/teste_sintatico_parser.cpp
)

# Linka com o lexer e o parser (que possuem nossa lógica) e com o GTest
target_link_libraries(testes PRIVATE
    lexer
    parser
    GTest::gtest_main
)

# Coloca o executável de teste na pasta build
set_target_properties(testes PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Descobre e registra os testes automaticamente.
# WORKING_DIRECTORY aponta para a raiz do projeto para que os testes que abrem
# arquivos .cmm por caminho relativo (ex.: ParserTest.ArquivoAbrangente) os encontrem.
include(GoogleTest)
gtest_discover_tests(testes
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    DISCOVERY_TIMEOUT 60
)

# Teste de geracao de TAC: compara a saida do compilador (--tac) com golden files.
# Roda o script de comparacao passando o binario do compilador recem-compilado.
add_test(
    NAME tac_golden
    COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/tac/run_tac_tests.sh $<TARGET_FILE:compilador>
)
