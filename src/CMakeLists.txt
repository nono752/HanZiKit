find_package(nlohmann_json CONFIG REQUIRED)

add_library(HanZiKit STATIC "")

add_subdirectory(compiler)
add_subdirectory(enricher)
add_subdirectory(generator)

target_link_libraries(HanZiKit PRIVATE nlohmann_json::nlohmann_json)
target_include_directories(HanZiKit PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(hzk main.cpp)

target_link_libraries(hzk PRIVATE HanZiKit)
target_include_directories(hzk PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/compiler)