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

#endif