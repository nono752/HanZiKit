set(HTML_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/html/index.html")

set(CSS_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/html/style.css"
    "${CMAKE_CURRENT_SOURCE_DIR}/html/print.css"
)
set(JS_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/html/app.js"
    "${CMAKE_CURRENT_SOURCE_DIR}/html/print.js"
)

set(HTML_TEMPLATE_HEADER "${CMAKE_CURRENT_BINARY_DIR}/htmlTemplate.hpp")

set_source_files_properties(${HTML_TEMPLATE_HEADER} PROPERTIES GENERATED TRUE)

add_custom_command(
    OUTPUT ${HTML_TEMPLATE_HEADER}
    COMMAND HtmlToHeader ${HTML_SOURCE} --CSS ${CSS_SOURCES} --JS ${JS_SOURCES} ${HTML_TEMPLATE_HEADER}
    DEPENDS HtmlToHeader ${HTML_SOURCE} ${CSS_SOURCES} ${JS_SOURCES}
    COMMENT "Generate htmlTemplate.hpp in current binary dir..."
)

add_custom_target(GenerateHtmlTemplate DEPENDS ${HTML_TEMPLATE_HEADER})
add_dependencies(HanZiKit GenerateHtmlTemplate)