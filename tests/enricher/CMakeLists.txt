add_executable(test_enricher 
    test_cedict.cpp
    test_enricher.cpp
)

target_link_libraries(test_enricher PRIVATE 
    GTest::gtest GTest::gtest_main
    HanZiKit
)

add_test(NAME EnricherTests COMMAND test_enricher)

gtest_discover_tests(test_enricher)