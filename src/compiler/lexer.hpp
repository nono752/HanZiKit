#ifndef LEXER_H
#define LEXER_H

#include "tokenTypes.hpp"
#include "error_handling/errorTypes.hpp"

void fileToString(const std::string& filepath, std::string& toWrite, Errors& errors);
Tokens tokenize(const std::string& file, Errors& errors);

#endif