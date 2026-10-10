#ifndef ERROR_TYPES_H
#define ERROR_TYPES_H

#include <string>
#include <vector>
#include <array>

enum class ErrorCode 
{
    // lexer errors 000 - 099
    SOURCE_FILE_NOPEN = 001,
    STRING_BUFFER_WRITING_FAILED = 002,

    // parser Errors 100 - 199
    MULTIPLE_MAIN_PAGES = 100,
    MISSING_TITLE = 101,
    UNEXPECTED_SYMBOL = 102,
    VOCAB_OUTSIDE_MODULE = 103,
    MISSING_SEPARATOR = 104,
    MISSING_TRANSLATION = 105,
    NO_INSTRUCTION = 106,
    UNKNOWN_PARSER_ERROR = 107,
    MISSING_PINYIN = 108,

    // enricher errors 200 - 299
    UNKWNOWN_HANZI_ENTRY = 200,
    INVALID_PINYIN_TONE = 201,
    MULTIPLE_AUTO_COMPLETION = 202,
    PINYIN_AND_HANZI_COUNT_NEQ = 203,
    PINYIN_DONT_MATCH_CEDICT = 204,
    
    // generation errors 300 - 399
    JSON_IS_EMPTY = 300,

    // exportation errors 400 - 499
    NO_JSON_TAG_IN_HTML = 400,
    MULTIPLE_JSON_TAG_IN_HTML = 401,
    HTML_OUTPUT_NOPEN = 402,
    HTML_BUFFER_WRITING_FAILED = 403
};

enum class ErrorPhase
{
    LEXER, 
    PARSER,
    ENRICHER,

    JSON_GENERATION,
    LATEX_GENERATION,

    HTML_EXPORTER,
};

struct Error 
{
    ErrorPhase phase;
    ErrorCode code;
    std::string details = "";
    unsigned line = 0;
    unsigned col = 0;
};

inline constexpr std::array<ErrorCode, 1> warningCodes = {
    ErrorCode::MULTIPLE_AUTO_COMPLETION,
};

struct ErrorCodeAndDetail
{
    ErrorCode code;
    std::string detail;
};

typedef std::vector<Error> Errors;

#endif