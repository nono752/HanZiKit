#include <iostream>
#include "compiler.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

int main(int argc, char* argv[])
{
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    #endif

    if (argc < 2) 
    {
        std::cerr << "Error : missing argument" << std::endl;
        std::cerr << "Use : hzk <file.txt>" << std::endl;
        return 1;
    }
    else if (argc == 2)
    {
        Compiler compiler;
        std::string filePath = argv[1];

        compiler.init();
        
        compiler.loadAndTokenize(filePath);
        if (!compiler.printAndClearErrors()) std::cerr << "Tokens succesfully created..." << std::endl;
        else return 1;
        //compiler._printTokens();

        compiler.parseTokens();
        if (!compiler.printAndClearErrors()) std::cerr << "Tokens succesfully parsed..." << std::endl;
        else return 1;

        compiler.completeAst();
        if (!compiler.printAndClearErrors()) std::cerr << "ast enriched succesfully..." << std::endl;
        else return 1;

        compiler.generateJSON();
        if (!compiler.printAndClearErrors()) std::cerr << "json succesfully generated..." << std::endl;
        //compiler._printJSON();
        else return 1;

        compiler.generateHtml();
        if (!compiler.printAndClearErrors()) std::cerr << "html file successfully generated..." << std::endl;
        else return 1;

        return 0;
    }
    else
    {
        std::cerr << "Error : too much arguments" << std::endl;
        std::cerr << "Use : hzk <file.txt>" << std::endl;
        return 1;
    }
}