#include "lexer.hpp"
#include "tokenTypes.hpp"
#include "error_handling/errorTypes.hpp"
#include <iostream>
#include <fstream>
#include <string>

void fileToString(const std::string& filepath, std::string& toWrite, Errors& errors) 
{
    toWrite.clear();
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file) 
    {
        errors.push_back({ErrorPhase::LEXER, ErrorCode::SOURCE_FILE_NOPEN, filepath});
        return;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    toWrite.resize(size, '\0');
    if (file.read(toWrite.data(), size)) 
    {
        file.close();
        return;
    }

    file.close();
    errors.push_back({ErrorPhase::LEXER, ErrorCode::STRING_BUFFER_WRITING_FAILED});
    toWrite.clear();
    return;
}

struct Reader
{
    size_t start = 0;
    size_t current = 0;
    std::string_view source;
    unsigned line = 1;
    unsigned col = 1;
    unsigned tokenStartCol = 1;

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
        unsigned char uc = static_cast<unsigned char>(c);

        if (c == '\n') 
        {
            line++;
            col = 1;
        } 
        else if (!(uc >= 128 && uc <= 191)) // not an utf8 continuation octet
        {
            col++;
        }
        
        return c;
    }

    void ignore() { start = current; }

    void saveTokenStartPos() { tokenStartCol = col - 1; }
    Token makeToken(TokenType type)
    {
        std::string_view word = source.substr(start, current - start);
        Token token = {type, word, line, tokenStartCol};
        start = current;

        return token;
    }
};

bool isSpace(char c) { return c == ' ' || c == '\t' || c == '\r'; }

Tokens tokenize(const std::string& file, Errors& errors)
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

        reader.saveTokenStartPos();

        switch (c)
        {
            case '\n':
                tokens.push_back(reader.makeToken(TokenType::NEWLINE));
                break;

            case '|': case '>':
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
                    // TODO: implement behavior when unknown char encoutered
                    errors.push_back({ErrorPhase::LEXER, ErrorCode::UNKNOWN_CHAR_ENCOUNTERED, std::string(1, c), reader.line, reader.col});
                    tokens.push_back(reader.makeToken(TokenType::UNKNOWN_CHAR));
                }
                break;
        }
    }

    return tokens;
}