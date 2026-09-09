#ifndef COMPILER_H
#define COMPILER_H

#include "types.hpp"
#include "astTypes.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "enricher/cedict.hpp"
#include "enricher/enricher.hpp"
#include "generator/jsonGenerator.hpp"
#include "generator/exporter.hpp"

class Compiler
{
    private:
        std::string source = "";
        std::vector<Error> errors;
        Tokens tokens = {};
        MainPage ast;
        Cedict dict;
        std::string json = "";

    public:
        void init()
        {
            dict.init();
        }
        void loadAndTokenize(const std::string& filePath)
        {
            source = fileToString(filePath, errors);
            tokens = tokenize(source, errors);
        }
        void parseTokens()
        {
            Parser parser(tokens, ast, errors);
            parser.parse();
        }
        void completeAst()
        {
            enrichAst(ast, dict, errors);
        }
        void generateJSON()
        {
            astToJson(ast, json, errors);
        }
        void generateHtml()
        {
            exportToHtml(json, "index.html", errors);
        }

        void printErrorsInTerminal()
        {
            for (const auto& err : errors) 
            {
                std::string message;

                switch (err.code) 
                {
                    case ErrorCode::MULTIPLE_MAIN_PAGES:
                        message = "More than one main page defined. Only one '#' is allowed.";
                        break;
                    case ErrorCode::MISSING_TITLE:
                        message = "Missing section or module title after marker.";
                        break;
                    case ErrorCode::UNEXPECTED_SYMBOL:
                        message = "Unexpected symbol encountered: '" + err.details + "'.";
                        break;
                    case ErrorCode::VOCAB_OUTSIDE_MODULE:
                        message = "Vocabulary entry '" + err.details + "' defined outside of a module ('##').";
                        break;
                    case ErrorCode::MISSING_SEPARATOR:
                        message = "Missing separator '|' for the entry '" + err.details + "'.";
                        break;
                    case ErrorCode::MISSING_TRADUCTION:
                        message = "Missing translation for the entry '" + err.details + "'.";
                        break;
                    case ErrorCode::NO_INSTRUCTION:
                        message = "Empty instruction or missing data.";
                        break;
                    case ErrorCode::UNKNOWN_PARSER_ERROR:
                    default:
                        message = "Unknown syntax error.";
                        break;
                }

                std::string phase = (err.phase == ErrorPhase::LEXER) ? "[LEXER]" : "[PARSER]";

                std::cerr << phase << " Error at Line " << err.pos.line 
                          << ", Col " << err.pos.col << " : " << message << "\n";
            }

            std::cerr << std::flush;
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
                    << " ,pos = (" << e.pos.line << "," << e.pos.col << ")" 
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