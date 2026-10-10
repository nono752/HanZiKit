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

struct Internal_LexerReader
{
    size_t start = 0;
    size_t current = 0;
    std::string_view source;
    unsigned line = 1;
    unsigned col = 1;
    unsigned tokenStartCol = 1;

    Internal_LexerReader(const std::string& file) : source(file) {}

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

static bool isSpace(char c) { return c == ' ' || c == '\t' || c == '\r'; }
static bool isSpecial(char c) { return c == '\n' || c == '#' || c == '|' || c == '>'; }

Tokens tokenize(const std::string& file, Errors& errors)
{
    Tokens tokens;
    Internal_LexerReader reader(file);

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

            case '|':
                tokens.push_back(reader.makeToken(TokenType::SEPARATOR_MARKER));
                break;

            case '#':  case '>':
                if (reader.peek() == '#')
                {
                    reader.advance();
                    tokens.push_back(reader.makeToken(TokenType::MODULE_MARKER));
                }
                else
                    tokens.push_back(reader.makeToken(TokenType::TITLE_MARKER));

                break;

            default:
                unsigned char currentUChar = static_cast<unsigned char>(c);
                unsigned char whatKindOfText = 0;

                while (!reader.isAtEnd() && !isSpecial(reader.peek()) && !isSpace(reader.peek()))
                {
                    if (currentUChar > 127)
                        whatKindOfText |= 2;
                    else 
                        whatKindOfText |= 1;

                    currentUChar = static_cast<unsigned char>(reader.advance());
                }
                    
                // if only utf8: considerated as hanzi, if only alnum or mixed: considerated as text
                TokenType type = whatKindOfText == 2 ? TokenType::HANZI : TokenType::TEXT;
                tokens.push_back(reader.makeToken(type));

                break;
        }
    }

    return tokens;
}