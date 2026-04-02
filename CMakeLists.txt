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
