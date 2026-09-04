#ifndef LEXER_H
#define LEXER_H

#include "types.hpp"

std::string fileToString(const std::string& filepath, Errors& errors);
Tokens tokenize(const std::string& file, Errors& errors);

#endif