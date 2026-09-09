add_executable(test_generators
    test_exporter.cpp
    test_jsonGenerator.cpp
)

target_link_libraries(test_generators PRIVATE 
    GTest::gtest GTest::gtest_main
    HanZiKit
)

add_test(NAME GeneratorsTests COMMAND test_generators)

gtest_discover_tests(test_generators)