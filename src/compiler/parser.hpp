#ifndef PARSER_H
#define PARSER_H

#include "tokenTypes.hpp"
#include "astTypes.hpp"
#include "error_handling/errorTypes.hpp"

class Parser 
{
    private:
        const Tokens& tokens;
        size_t current = 0;

        MainPage& ast;
        Module* currentModule = nullptr;
        Errors& errors;

        bool isAtEnd() const { return current >= tokens.size(); }
        const Token* peek() const 
        {
            if (isAtEnd()) return nullptr;
            return &tokens[current];
        }
        const Token* advance() 
        {
            if (isAtEnd()) return nullptr;
            return &tokens[current++];
        }
        void synchronize() 
        {
            while (!isAtEnd() && peek()->type != TokenType::NEWLINE)
                advance();
        }

    public:
        Parser(const Tokens& t, MainPage& origin, Errors& err) : tokens(t), ast(origin), errors(err) {}
        void parse() 
        {
            while (!isAtEnd())
                parseLine();
        }

    private:
        void pushErrorAndSynchronize(ErrorCode err, std::string_view detail = "");
        std::string_view getTextSequence();
        
        void parseTitle(bool isMainPage);

        std::string_view extractPinyin();
        std::string_view extractTranslation();
        void parseVocItem(const Token& vocItemTok);

        void parseLine();

        bool NextTokenIsSeparator() const
        {
            return peek() && peek()->type == TokenType::SPECIAL_CHAR && peek()->data == "|";
        }
        bool NextTokenIsNewline() const
        {
            return peek() && peek()->type == TokenType::NEWLINE;
        }
};

#endif