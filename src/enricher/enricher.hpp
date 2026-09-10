#ifndef ENRICHER_H
#define ENRICHER_H

#include "compiler/tokenTypes.hpp"
#include "error_handling/errorTypes.hpp"
#include "compiler/astTypes.hpp"

class Cedict;

void enrichAst(MainPage& ast, const Cedict& dict, Errors& errors);

#endif