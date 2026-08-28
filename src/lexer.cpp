#include "lexer.hpp"
#include "types.hpp"
#include <iostream>
#include <fstream>
#include <string>

std::string fileToString(const std::string& filepath) 
{
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file) 
    {
        std::cerr << "couldn't open : " << filepath << std::endl;
        return "";
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::string buffer(size, '\0');
    if (file.read(buffer.data(), size)) 
    {
        file.close();
        return buffer;
    }

    file.close();
    return "";
}

struct Reader
{
    size_t start = 0;
    size_t current = 0;
    std::string_view source;
    Pos pos = {1, 1};

    Reader(const std::string& file) : source(file) {}

    bool isAtEnd() const { return current >= source.length(); }

    char peek() const 
    {
        if (isAtEnd()) return '\0';
        return source[current];
    }
    char advance() 
    {
        if (isAtEnd()) return '\0';
        char c = source[current++];

        if (c == '\n') 
        {
            pos.line++;
            pos.col = 1;
        } 
        else 
        {
            pos.col++;
        }
        
        return c;
    }

    void ignore() { start = current; }

    Token makeToken(TokenType type)
    {
        std::string_view word = source.substr(start, current - start);
        Token token = {type, word, pos};
        start = current;

        return token;
    }
};

bool isSpace(char c) { return c == ' ' || c == '\t' || c == '\r'; }

Tokens tokenize(const std::string& file)
{
    Tokens tokens;
    Reader reader(file);

    while (!reader.isAtEnd()) 
    {
        char c = reader.advance();

        if (isSpace(c)) 
        {
            reader.ignore();
            continue;
        }

        switch (c) 
        {
            case '\n':
                tokens.push_back(reader.makeToken(TokenType::NEWLINE));
                break;

            case ':': // use fall-through for other single special char
                tokens.push_back(reader.makeToken(TokenType::SPECIAL_CHAR));
                break;

            case '#':
                if (reader.peek() == '#')
                    reader.advance();
                tokens.push_back(reader.makeToken(TokenType::SPECIAL_CHAR));
                break;

            default:
                unsigned char currentUChar = static_cast<unsigned char>(c);

                unsigned char whatKindOfText = 0;
                if (currentUChar > 127) whatKindOfText = 2;
                else if (isalnum(currentUChar)) whatKindOfText = 1;

                if (whatKindOfText > 0) 
                {
                    while (!reader.isAtEnd())
                    {
                        unsigned char nextUChar = static_cast<unsigned char>(reader.peek());

                        if (isalnum(nextUChar))
                            whatKindOfText |= 1;
                        else if (nextUChar > 127)
                            whatKindOfText |= 2;
                        else 
                            break;
                        
                        reader.advance();
                    }   
                    
                    // if only utf8 considerated as hanzi, if only alnum or mixed considerated as text
                    TokenType type = whatKindOfText == 2 ? TokenType::HANZI : TokenType::TEXT;

                    tokens.push_back(reader.makeToken(type));
                }
                else
                {
                    tokens.push_back(reader.makeToken(TokenType::UNKNOWN_CHAR));
                }
                break;
        }
    }

    return tokens;
}