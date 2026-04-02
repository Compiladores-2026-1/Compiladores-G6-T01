cmake_minimum_required(VERSION 3.16)
project(compilador_cminus_strict C CXX)

# Exportar compile_commands.json para IntelliSense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ========================
# Configuração de linguagem
# ========================
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ========================
# Build type padrão
# ========================
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Flags de compilação
set(CMAKE_C_FLAGS_DEBUG "-g -O0")
set(CMAKE_C_FLAGS_RELEASE "-O2")

# ========================
# Dependências
# ========================
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# ========================
# Diretórios e arquivos
# ========================
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(PARSER_Y ${SRC_DIR}/parser/parser.y)
set(LEXER_L  ${SRC_DIR}/lexer/lexer.l)
set(MAIN_C   ${SRC_DIR}/main.c)

# Validação dos arquivos
foreach(file ${PARSER_Y} ${LEXER_L} ${MAIN_C})
    if(NOT EXISTS ${file})
        message(FATAL_ERROR "Arquivo não encontrado: ${file}")
    endif()
endforeach()

# ========================
# Bison / Flex
# ========================
BISON_TARGET(
    Parser
    ${PARSER_Y}
    ${CMAKE_CURRENT_BINARY_DIR}/parser.c
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.h
)

FLEX_TARGET(
    Lexer
    ${LEXER_L}
    ${CMAKE_CURRENT_BINARY_DIR}/lexer.c
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/tokens.h
)

ADD_FLEX_BISON_DEPENDENCY(Lexer Parser)

# ========================
# Biblioteca compilador_lib
# ========================
add_library(compilador_lib
    ${BISON_Parser_OUTPUTS}
    ${FLEX_Lexer_OUTPUTS}
)

target_include_directories(compilador_lib PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${SRC_DIR}
)

# Flex lib (yywrap)
find_library(FL_LIBRARY NAMES fl)
if(FL_LIBRARY)
    target_link_libraries(compilador_lib PRIVATE ${FL_LIBRARY})
endif()

# ========================
# Executável principal
# ========================
add_executable(compilador ${MAIN_C})
target_link_libraries(compilador PRIVATE compilador_lib)
set_target_properties(compilador PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# ========================
# TESTES (apenas Debug)
# ========================
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    include(FetchContent)

    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()

    add_executable(testes
        tests/test_parser.cpp
    )

    add_dependencies(testes compilador_lib)

    target_link_libraries(testes PRIVATE
        compilador_lib
        GTest::gtest_main
    )

    target_include_directories(testes PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
    )

    set_target_properties(testes PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    )

    include(GoogleTest)
    gtest_discover_tests(testes
        DISCOVERY_TIMEOUT 60
    )
endif()
