add_library(tac STATIC
    tac.cpp
)

target_link_libraries(tac PUBLIC lexer ast symtable)

target_include_directories(tac PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/include
)
