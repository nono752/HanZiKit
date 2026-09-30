#ifndef COMPILER_H
#define COMPILER_H

#include "compiler/tokenTypes.hpp"
#include "compiler/astTypes.hpp"
#include "compiler/lexer.hpp"
#include "compiler/parser.hpp"
#include "error_handling/errorTypes.hpp"
#include "error_handling/ErrorHandler.hpp"
#include "enricher/cedict.hpp"
#include "enricher/enricher.hpp"
#include "generator/jsonGenerator.hpp"
#include "generator/exporter.hpp"
#include <deque>

class Compiler
{
    private:
        std::string source = "";
        std::deque<std::string> astPool; // avoid dangling pointer in ast when resized
        Cedict dict;
        Errors errors;
        ErrorHandler errorHandler{errors};

        Tokens tokens = {};
        MainPage ast;
        std::string json = "";
        
    public:
        void init()
        {
            dict.init();
        }
        void loadAndTokenize(const std::string& filePath)
        {
            fileToString(filePath, source, errors);
            tokens = tokenize(source, errors);
        }
        void parseTokens()
        {
            Parser parser(tokens, ast, errors);
            parser.parse();
        }
        void completeAst()
        {
            enrichAst(ast, dict, errors, astPool);
        }
        void generateJSON()
        {
            astToJson(ast, json, errors);
        }
        void generateHtml()
        {
            exportToHtml(json, "index.html", errors);
        }

        // return true if there is fatalErrors
        bool printAndClearErrors(bool shouldPrintLog = false)
        {
            errorHandler.makeErrorBuffer();
            if (shouldPrintLog && !errorHandler.printErrorBufferInLogFile("log.txt"))
            {
                std::cerr << "failed to print errors in log.txt ...\n";
            }
            errorHandler.printErrorBufferInTerminal();

            bool hasFatalErrors = errorHandler.getFatalErrors() > 0;
            errorHandler.clear();

            return hasFatalErrors;
        }
        
        void _printTokens()
        {
            std::cout << "total tokens = " << tokens.size() << std::endl;
            size_t count = 0;

            for (auto e : tokens)
            {
                std::cout << count << " :" 
                    << " type = " << e.type
                    << " ,data = " << e.data
                    << " ,pos = (" << e.line << "," << e.col << ")" 
                    << std::endl;

                count++;
            }
        }
        void _printJSON()
        {
            std::cout << json << std::endl;
        }
};

#endif