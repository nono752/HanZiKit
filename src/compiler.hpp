#ifndef COMPILER_H
#define COMPILER_H

#include "lexer.hpp"
#include "types.hpp"

class Compiler
{
    private:
        std::string source = "";
        Tokens tokens = {};
    
    public:
        void loadAndTokenize(const std::string& filePath)
        {
            source = fileToString(filePath);
            tokens = tokenize(source);
        }

        void _printTokens(void)
        {
            std::cout << "total tokens = " << tokens.size() << std::endl;
            size_t count = 0;

            for (auto e : tokens)
            {
                std::cout << count << " :" 
                    << " type = " << e.type
                    << " ,data = " << e.data
                    << " ,pos = (" << e.pos.line << "," << e.pos.col << ")" 
                    << std::endl;

                count++;
            }
        }
};

#endif