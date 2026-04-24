add_library(semantic STATIC
    semantic.cpp
)

target_link_libraries(semantic PUBLIC ast symtable)

target_include_directories(semantic PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/include
)
