cmake_minimum_required(VERSION 3.16)
project(compilador_cminus_strict CXX)

# Configuração Global
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Definir o tipo de build padrão como Debug, se não especificado
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Atualizado para CXX_FLAGS
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

# Dependências Globais
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# Orquestração das Pastas
add_subdirectory(src)

# Somente habilitar testes em modo Debug
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    enable_testing()
    add_subdirectory(tests)
endif()
