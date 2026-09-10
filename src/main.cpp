#include <iostream>
#include "compiler.hpp"

int main(int argc, char* argv[])
{
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
        //compiler._printTokens();

        compiler.parseTokens();
        if (!compiler.printAndClearErrors()) std::cerr << "Tokens succesfully parsed..." << std::endl;

        compiler.completeAst();
        if (!compiler.printAndClearErrors()) std::cerr << "ast enriched succesfully..." << std::endl;

        compiler.generateJSON();
        if (!compiler.printAndClearErrors()) std::cerr << "json succesfully generated..." << std::endl;
        //compiler._printJSON();

        compiler.generateHtml();
        if (!compiler.printAndClearErrors()) std::cerr << "html file successfully generated..." << std::endl;

        return 0;
    }
    else
    {
        std::cerr << "Error : too much arguments" << std::endl;
        std::cerr << "Use : hzk <file.txt>" << std::endl;
        return 1;
    }
}