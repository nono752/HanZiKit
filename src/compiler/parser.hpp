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

    public:
        Parser(const Tokens& t, MainPage& origin, Errors& err) : tokens(t), ast(origin), errors(err) {}
        void parse() 
        {
            while (!isAtEnd())
                parseLine();
        }

    private:
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

        void pushErrorAndSynchronize(ErrorCode err, std::string_view detail = "");
        template <TokenType... Stop>
        std::string_view consumeTo();
        
        void parseTitle(bool isMainPage);

        std::string_view extractPinyin();
        std::string_view extractTranslation();
        void parseVocItem(const Token& vocItemTok);

        void parseLine();

        bool NextTokenIsSeparator() const
        {
            return peek() && peek()->type == TokenType::SEPARATOR_MARKER;
        }
        bool NextTokenIsNewline() const
        {
            return peek() && peek()->type == TokenType::NEWLINE;
        }
};

template <TokenType... Stop>
std::string_view Parser::consumeTo()
{
    const Token* firstTok = peek();
    if (!firstTok || (... || (firstTok->type == Stop))) return {};

    const Token* lastTok = nullptr;
    while (peek() && !(... || (peek()->type == Stop)))
        lastTok = advance();

    const char* start = firstTok->data.data();
    const char* end = lastTok->data.data() + lastTok->data.size();
    size_t size = end - start;

    return std::string_view(start, size);
}

#endif