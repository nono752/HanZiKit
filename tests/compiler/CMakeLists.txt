add_executable(test_compiler 
    test_lexer.cpp 
    test_parser.cpp
)

target_link_libraries(test_compiler PRIVATE 
    GTest::gtest GTest::gtest_main
    HanZiKit
)

add_test(NAME CompilerTests COMMAND test_compiler)

gtest_discover_tests(test_compiler)