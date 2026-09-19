#include "exporter.hpp"
#include <fstream>
#include <string>
#include "htmlTemplate.hpp"
#include <iostream>

extern const unsigned char htmlTemplate[];
extern const size_t htmlTemplateSize;

void injectJson(const std::string_view htmlBase, const std::string& json, std::string& toWrite, Errors& errors)
{
    toWrite.clear();

    std::string jsonTag = "null /* {{JSON_INJECT}} */";
    size_t jsonBegin = htmlBase.find(jsonTag);
    if (jsonBegin == std::string_view::npos)
    {
        errors.push_back({ErrorPhase::HTML_EXPORTER, ErrorCode::NO_JSON_TAG_IN_HTML});
        return;
    }
    size_t jsonEnd = jsonBegin + jsonTag.size();

    // case: multiple jsonTag
    size_t secondPos = htmlBase.find(jsonTag, jsonEnd);
    if (secondPos != std::string_view::npos)
    {
        size_t startPrint = (secondPos > 50) ? secondPos - 50 : 0; // TMP debug
        std::cerr << "Doublon trouve pres de : \n" 
              << std::string(htmlBase.substr(startPrint, 100)) << std::endl;

        errors.push_back({ErrorPhase::HTML_EXPORTER, ErrorCode::MULTIPLE_JSON_TAG_IN_HTML});
        return;
    }

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
    if (!out)
    {
        errors.push_back({ErrorPhase::HTML_EXPORTER, ErrorCode::HTML_OUTPUT_NOPEN});
        return false;
    }
  
    std::string finalContent;
    injectJson(std::string_view(reinterpret_cast<const char*> (htmlTemplate), htmlTemplateSize), json, finalContent, errors);
    if (finalContent.empty())
    {
        errors.push_back({ErrorPhase::HTML_EXPORTER, ErrorCode::HTML_BUFFER_WRITING_FAILED});
        return false;
    }

    out << finalContent;

    // TODO check if fail and push error
    
    return true;
}