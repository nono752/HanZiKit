#include "parser.hpp"
#include "tokenTypes.hpp"
#include "error_handling/errorTypes.hpp"
#include <string_view>
#include <vector>
#include <iostream>

void Parser::pushErrorAndSynchronize(ErrorCode err, std::string_view detail)
{
    std::string msg(detail); 
    if (msg.empty()) msg = peek() ? std::string(peek()->data) : "EOF"; 

    unsigned line, col;
    if (!isAtEnd())
    {
        line = tokens[current].line;
        col = tokens[current].col;
    }
    else if (!tokens.empty())
    {
        line = tokens.back().line;
        col = tokens.back().col;
    }
    else
        line = col = 1;

    errors.push_back({ErrorPhase::PARSER, err, msg, line, col});
    synchronize();
}

std::string_view Parser::getTextSequence()
{
    std::string_view title;

    const Token* firstTok = peek();
    if (!firstTok || (firstTok->type != TokenType::TEXT && firstTok->type != TokenType::HANZI))
        return {};

    const Token* lastTok = nullptr;
    while (peek() && (peek()->type == TokenType::TEXT || peek()->type == TokenType::HANZI))
    {
        lastTok = advance();
    }

    const char* start = firstTok->data.data();
    const char* end = lastTok->data.data() + lastTok->data.size();
    size_t size = end - start;

    return std::string_view(start, size);
}

void Parser::detectTitle(bool isMainPage)
{
    if (isMainPage && !ast.title.empty()) 
    {
        pushErrorAndSynchronize(ErrorCode::MULTIPLE_MAIN_PAGES);
        return;
    }
        
    std::string_view title = getTextSequence();
    
    if (title.empty() && peek() && peek()->type != TokenType::NEWLINE) 
    {
        pushErrorAndSynchronize(ErrorCode::UNEXPECTED_SYMBOL);
        return;
    }
    if (title.empty()) 
    {
        pushErrorAndSynchronize(ErrorCode::MISSING_TITLE);
        return;
    }
        
    if (isMainPage) 
    {
        ast.title = title;
    } 
    else 
    {
        ast.addModule(title, {});
        currentModule = &ast.modules.back();
    }

    advance();
}

void Parser::detectVocItem()
{
    if (!currentModule)
    {
        pushErrorAndSynchronize(ErrorCode::VOCAB_OUTSIDE_MODULE, tokens[current - 1].data);
        return;
    }

    VocItem item;
    item.hanzi = tokens[current - 1].data;

    if (peek() && peek()->type == TokenType::SPECIAL_CHAR && peek()->data == "|")
        advance();
    else
    {
        pushErrorAndSynchronize(ErrorCode::MISSING_SEPARATOR);
        return;
    }

    std::string_view trad = getTextSequence();

    if (trad.empty())
    {
        pushErrorAndSynchronize(ErrorCode::MISSING_TRADUCTION);
        return;
    }
    else if (peek() && peek()->type != TokenType::NEWLINE)
    {
        pushErrorAndSynchronize(ErrorCode::UNEXPECTED_SYMBOL);
        return;
    }
    else
    {
        item.translation = trad;
        currentModule->vocItems.push_back(item);
        advance();
    }
}

void Parser::parseLine()
{
    const Token* first = advance();

    if (!first || first->type == TokenType::NEWLINE) return;

    if (first->type == TokenType::SPECIAL_CHAR && first->data == "#")
        detectTitle(true); // mainPage = true
    else if (first->type == TokenType::SPECIAL_CHAR && first->data == "##")
        detectTitle(false);
    else if (first->type == TokenType::HANZI)
        detectVocItem();
    else if (first->type == TokenType::TEXT)
        pushErrorAndSynchronize(ErrorCode::NO_INSTRUCTION);
    else
        pushErrorAndSynchronize(ErrorCode::UNKNOWN_PARSER_ERROR);
}