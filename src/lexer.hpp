#ifndef LEXER_H
#define LEXER_H

#include "types.hpp"

std::string fileToString(const std::string& filepath);
Tokens tokenize(const std::string& file);

#endif