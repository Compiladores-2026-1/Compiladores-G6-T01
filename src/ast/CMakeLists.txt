add_library(ast STATIC
    ast.cpp
    ASTPrinter.cpp
)

target_include_directories(ast PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/include
)
