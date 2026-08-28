find_package(GTest CONFIG REQUIRED)

add_executable(test_lexer 
    test_lexer.cpp 
    ../src/lexer.cpp
)

target_link_libraries(test_lexer PRIVATE GTest::gtest GTest::gtest_main)

target_include_directories(test_lexer PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_test(NAME LexerTests COMMAND test_lexer)