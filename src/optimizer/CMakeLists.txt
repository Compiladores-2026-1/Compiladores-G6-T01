add_library(optimizer STATIC
    optimizer.cpp
)

target_link_libraries(optimizer PUBLIC tac)

target_include_directories(optimizer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/include
)
