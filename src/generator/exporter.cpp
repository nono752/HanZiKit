#include "exporter.hpp"
#include <fstream>
#include <string>
#include "htmlTemplate.hpp"

void injectJson(const std::string_view htmlBase, const std::string& json, std::string& toWrite, Errors& errors)
{
    toWrite.clear();

    std::string jsonTag = "null /* {{JSON_INJECT}} */";
    size_t jsonBegin = htmlBase.find(jsonTag);
    if (jsonBegin == std::string_view::npos) return; // ADD ERROR
    size_t jsonEnd = jsonBegin + jsonTag.size();

    // case: multiple jsonTag
    if (htmlBase.find(jsonTag, jsonEnd) != std::string_view::npos) return; // ADD ERROR

    toWrite.reserve(htmlBase.size() + json.size());
    toWrite.append(htmlBase.substr(0, jsonBegin));

    if (json.empty())
        toWrite.append("null");
    else
        toWrite.append(json);
        
    toWrite.append(htmlBase.substr(jsonEnd));
}

// TODO ADD ERROR
bool exportToHtml(const std::string& json, const std::string& outFileName, Errors& errors)
{
    std::ofstream out(outFileName);
    if (!out) return 1;
  
    std::string finalContent;
    injectJson(HTML_TEMPLATE, json, finalContent, errors);
    if (finalContent.empty()) return 1;

    out << finalContent;

    return 0;
}