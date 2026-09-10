#ifndef JSON_GENERATOR_H
#define JSON_GENERATOR_H

#include "compiler/tokenTypes.hpp"
#include "error_handling/errorTypes.hpp"

struct MainPage;

void astToJson(const MainPage& ast, std::string& toWrite, Errors& errors);

#endif