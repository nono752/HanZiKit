#include <fstream>
#include <iostream>
#include <string>
#include "utils.hpp"

/*
    Convert the html/css/js file into a rawstring:
        constexpr std::string_view HTML_TEMPLATE = R\"hzk_template(...)hzk_template"

    Usage: HtmlToHeader <in.html> <in.css> <in.js> <out.hpp>
    Current CMake write it in the file htmlTemplate.hpp in the binary dir.
*/

int main(int argc, char* argv[]) 
{
    if (argc != 5)
    {
        std::cerr << "Error: Usage: HtmlToHeader <in.html> <in.css> <in.js> <out.hpp>\n";
        printArgs(argv, argc);
        return 1;
    }

    std::string htmlBuff = makeBufferFrom(argv[1]);
    std::string cssBuff = makeBufferFrom(argv[2]);
    std::string jsBuff = makeBufferFrom(argv[3]);
    std::ofstream out(argv[4]);

    if (!out)
    {
        std::cerr << "Error: cannot open output file\n";
        return 1;
    }
    if (htmlBuff.empty())
    {
        std::cerr << "Error: html file is empty, maybe failed to read the file\n";
        return 1;
    }
    if (cssBuff.empty())
    {
        std::cerr << "Error: css file is empty, maybe failed to read the file\n";
        return 1;
    }
    if (jsBuff.empty())
    {
        std::cerr << "Error: js file is empty, maybe failed to read the file\n";
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

    out << "#ifndef HTML_TEMPLATE_H\n#define HTML_TEMPLATE_H\n\n#include <string_view>\n\n";
    out << "constexpr std::string_view HTML_TEMPLATE = R\"hzk_template(\n";

    out << htmlBuff.substr(0, cssBegin);
    out << cssBuff;
    out << htmlBuff.substr(cssEnd, jsBegin - cssEnd);
    out << jsBuff;
    out << htmlBuff.substr(jsEnd);

    out << "\n)hzk_template\";\n\n#endif";

    if (out.fail())
    {
        std::cerr << "Error: ostream failed\n";
        return 1;
    }
    out.close();

    return 0;
}