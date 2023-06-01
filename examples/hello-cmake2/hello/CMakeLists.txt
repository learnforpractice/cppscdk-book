cmake_minimum_required(VERSION 3.5)

add_contract(test test
    src/test.cpp
)

target_link_libraries(test PUBLIC utils)

target_include_directories(test PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/utils/include
)
