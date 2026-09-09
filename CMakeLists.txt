cmake_minimum_required(VERSION 3.10)

project(HanZiKit)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

cmake_policy(SET CMP0076 NEW)

add_subdirectory(tools)

enable_testing()
add_subdirectory(tests)

add_subdirectory(src)