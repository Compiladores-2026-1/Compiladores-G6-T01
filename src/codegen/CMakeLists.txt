add_library(codegen STATIC
    codegen.cpp
)

target_link_libraries(codegen PUBLIC tac)

target_include_directories(codegen PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/include
)
