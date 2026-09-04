#ifndef GENERATOR_H
#define GENERATOR_H

#include "compiler/types.hpp"

struct MainPage;

void astToJson(const MainPage& ast, Errors& errors, std::string& toWrite);

#endif