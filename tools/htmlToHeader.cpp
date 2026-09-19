#include <fstream>
#include <iostream>
#include <string>
#include "utils.hpp"

/*
    Convert the html/css/js file into: const unsigned char htmlTemplate[]

    Usage: HtmlToHeader <in.html> [--css f1.css...] [--js f1.js...] <out.hpp>
    Current CMake write it in the file htmlTemplate.hpp in the current binary dir.
*/

int main(int argc, char* argv[]) 
{
    if (argc < 3)
    {
        std::cerr << "ERROR: Usage: HtmlToHeader <in.html> [--css f1.css...] [--js f1.js...] <out.hpp>\n";
        printArgs(argv, argc);
        return 1;
    }

    std::string htmlBuff;
    if (!makeBufferFrom(argv[1], htmlBuff))
    {
        std::cerr << "Error: failed to make buffer from input file\n";
        return 1;
    }
    std::ofstream out(argv[argc - 1]);
    std::string cssBuff;
    std::string jsBuff;

    enum class Mode { NONE, CSS, JS };
    Mode currentMode = Mode::NONE;

    for (int i = 2; i < argc - 1; ++i) 
    {
        std::string arg = argv[i];
        
        if (arg == "--CSS") currentMode = Mode::CSS;
        else if (arg == "--JS") currentMode = Mode::JS;
        else
        {
            std::string content;
            makeBufferFrom(arg.data(), content);
            if (content.empty()) std::cout << "WARNING: file '" << arg << "' empty or not readable\n";
            
            if (currentMode == Mode::CSS) cssBuff += content + "\n";
            else if (currentMode == Mode::JS) jsBuff += content + "\n";
        }
    }

    if (!out)
    {
        std::cerr << "ERROR: cannot open output file\n";
        return 1;
    }
    else if (htmlBuff.empty())
    {
        std::cerr << "Error: html file is empty, maybe failed to read the file\n";
        return 1;
    }

    std::string cssTag = "/* {{CSS_INJECT}} */";
    size_t cssBegin = htmlBuff.find(cssTag);
    if (cssBegin == std::string::npos)
    {
        std::cerr << "Error: cannot find css Tag in html file\n";
        return 1;
    }
    size_t cssEnd = cssBegin + cssTag.size();

    std::string jsTag = "/* {{JS_INJECT}} */";
    size_t jsBegin = htmlBuff.find(jsTag, cssEnd);
    if (jsBegin == std::string::npos)
    {
        std::cerr << "Error: cannot find js Tag in html file\n";
        return 1;
    }
    size_t jsEnd = jsBegin + jsTag.size();

    std::string toCssTagHexBuff;
    std::string cssHexBuff;
    std::string cssTagToJsTagHexBuff;
    std::string jsHexBuff;
    std::string jsTagToEndHexBuff;

    makeHexListString(htmlBuff.substr(0, cssBegin), toCssTagHexBuff);
    makeHexListString(cssBuff, cssHexBuff);
    makeHexListString(htmlBuff.substr(cssEnd, jsBegin - cssEnd), cssTagToJsTagHexBuff);
    makeHexListString(jsBuff, jsHexBuff);
    makeHexListString(htmlBuff.substr(jsEnd), jsTagToEndHexBuff);

    size_t totalBytes = htmlBuff.substr(0, cssBegin).size() + 
        cssBuff.size() + 
        htmlBuff.substr(cssEnd, jsBegin - cssEnd).size() + 
        jsBuff.size() + 
        htmlBuff.substr(jsEnd).size();

    size_t invalidJsonTag = jsBuff.find("null /* {{JSON_INJECT}} */");
    if (invalidJsonTag != std::string::npos)
    {
        size_t startPrint = (invalidJsonTag > 50) ? invalidJsonTag - 50 : 0;
        std::cerr << "Error: invalid jsonTag in javascript file \n" 
              << std::string(jsBuff.substr(startPrint, 100)) << std::endl;
        return 1;
    }

    out << "#ifndef HTML_TEMPLATE_H\n#define HTML_TEMPLATE_H\n\n";
    out << "const unsigned char htmlTemplate[] = {\n";
    out << toCssTagHexBuff;
    out << cssHexBuff;
    out << cssTagToJsTagHexBuff;
    out << jsHexBuff;
    out << jsTagToEndHexBuff;
    out << "\n};const size_t htmlTemplateSize = " << totalBytes << ";";
    out << "\n\n#endif";

    if (out.fail())
    {
        std::cerr << "Error: ostream failed\n";
        return 1;
    }
    out.close();

    return 0;
}