find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

add_subdirectory(compiler)
add_subdirectory(enricher)
add_subdirectory(generator)