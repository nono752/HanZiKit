#ifndef ENRICHER_H
#define ENRICHER_H

#include "compiler/types.hpp"
#include "compiler/astTypes.hpp"

class Cedict;

void enrichAst(MainPage& ast, const Cedict& dict, Errors& errors);

#endif