cmake_minimum_required(VERSION 3.5)

add_library(utils
    src/utils.cpp
)

target_include_directories(utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
