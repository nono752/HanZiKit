find_package(GTest CONFIG REQUIRED)

add_executable(test_compiler 
    test_lexer.cpp 
    test_parser.cpp
)

target_link_libraries(test_compiler PRIVATE 
    GTest::gtest GTest::gtest_main
    HanZiKit
)

target_include_directories(test_compiler PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_test(NAME CompilerTests COMMAND test_compiler)

include(GoogleTest)
gtest_discover_tests(test_compiler)