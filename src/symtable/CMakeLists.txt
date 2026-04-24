add_library(symtable STATIC
    symtable.cpp
)

target_include_directories(symtable PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/include
)
