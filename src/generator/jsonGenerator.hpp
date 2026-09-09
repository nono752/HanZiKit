#ifndef JSON_GENERATOR_H
#define JSON_GENERATOR_H

#include "compiler/types.hpp"

struct MainPage;

void astToJson(const MainPage& ast, std::string& toWrite, Errors& errors);

#endif