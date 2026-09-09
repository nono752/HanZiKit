#ifndef EXPORTER_H
#define EXPORTER_H

#include <string>
#include <string_view>
#include <compiler/types.hpp>

void injectJson(const std::string_view htmlBase, const std::string& json, std::string& toWrite, Errors& errors);
bool exportToHtml(const std::string& json, const std::string& outFileName, Errors& errors);

#endif