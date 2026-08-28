cmake_minimum_required(VERSION 3.10)

project(HanZiKit)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()
add_subdirectory(tests)

add_subdirectory(src)

add_executable(hzk src/main.cpp)
target_link_libraries(hzk PRIVATE HanZiKit)