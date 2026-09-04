#ifndef TYPES_H
#define TYPES_H

#include <vector>
#include <string>
#include <string_view>

enum TokenType
{
    TEXT,
    HANZI, // it is an utf8 but not guaranted to be chinese character
    SPECIAL_CHAR,
    NEWLINE,
    UNKNOWN_CHAR
};

struct Pos
{
    unsigned line = 0;
    unsigned col = 0;
};

struct Token
{
    TokenType type;
    std::string_view data;
    Pos pos;
};

typedef std::vector<Token> Tokens;

enum class ErrorCode 
{
    // parser Errors 100 - 199
    MULTIPLE_MAIN_PAGES = 100,
    MISSING_TITLE = 101,
    UNEXPECTED_SYMBOL = 102,
    VOCAB_OUTSIDE_MODULE = 103,
    MISSING_SEPARATOR = 104,
    MISSING_TRADUCTION = 105,
    NO_INSTRUCTION = 106,
    UNKNOWN_PARSER_ERROR = 107
};

enum class ErrorPhase
{
    LEXER, 
    PARSER
};

struct Error 
{
    ErrorPhase phase;
    ErrorCode code;
    std::string details;
    Pos pos;
};

typedef std::vector<Error> Errors;

#endif