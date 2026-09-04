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
        //compiler._printTokens();

        compiler.parseTokens();
        compiler.printErrorsInTerminal();

        compiler.completeAst();
        
        compiler.generateJSON();
        compiler._printJSON();
        
        return 0;
    }
    else
    {
        std::cerr << "Error : too much arguments" << std::endl;
        std::cerr << "Use : hzk <file.txt>" << std::endl;
        return 1;
    }
}