cmake_minimum_required(VERSION 3.5)

project(hello)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall --no-missing-ricardian-clause")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS ON)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(cdt)

add_subdirectory(utils)
add_subdirectory(hello)
