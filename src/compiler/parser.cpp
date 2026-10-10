#include "parser.hpp"
#include "tokenTypes.hpp"
#include "error_handling/errorTypes.hpp"
#include <string_view>
#include <vector>

void Parser::pushErrorAndSynchronize(ErrorCode err, std::string_view detail)
{
    std::string msg(detail); 
    if (msg.empty()) msg = isAtEnd() ? "EOF" : std::string(peek()->data); 

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

void Parser::parseTitle(bool isMainPage)
{
    if (isMainPage && !ast.title.empty()) 
    {
        pushErrorAndSynchronize(ErrorCode::MULTIPLE_MAIN_PAGES);
        return;
    }
        
    std::string_view title = consumeTo<TokenType::NEWLINE>();
    
    if (title.empty() && (isAtEnd() || NextTokenIsNewline())) 
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

std::string_view Parser::extractPinyin()
{
    std::string_view pinyin = consumeTo<TokenType::SEPARATOR_MARKER, TokenType::NEWLINE>();
    if (pinyin.empty())
    {
        pushErrorAndSynchronize(ErrorCode::MISSING_PINYIN);
        return "";
    }

    if (!NextTokenIsSeparator())
    {
        pushErrorAndSynchronize(ErrorCode::MISSING_SEPARATOR);
        return "";
    }
    advance();
    
    return pinyin;
}
std::string_view Parser::extractTranslation()
{
    std::string_view trad = consumeTo<TokenType::NEWLINE>();
    if (trad.empty())
    {
        pushErrorAndSynchronize(ErrorCode::MISSING_TRANSLATION);
        return "";
    }
    else if (!NextTokenIsNewline())
    {
        pushErrorAndSynchronize(ErrorCode::UNEXPECTED_SYMBOL);
        return "";
    }

    return trad;
}
void Parser::parseVocItem(const Token& vocItemTok)
{
    if (!currentModule)
    {
        pushErrorAndSynchronize(ErrorCode::VOCAB_OUTSIDE_MODULE, vocItemTok.data);
        return;
    }

    VocItem item;
    item.hanzi = vocItemTok.data;
    item.line = vocItemTok.line;
    item.col = vocItemTok.col;
    
    if (NextTokenIsSeparator()) advance();
    else if (NextTokenIsNewline() || isAtEnd())
    {
        currentModule->vocItems.push_back(item);
        if (!isAtEnd()) advance();
        return;
    }
    else
    {
        pushErrorAndSynchronize(ErrorCode::MISSING_SEPARATOR);
        return;
    }

    item.pinyin = extractPinyin();
    if (item.pinyin.empty()) return;
    item.translation = extractTranslation();
    if (item.translation.empty()) return;

    currentModule->vocItems.push_back(item);
    advance();
}

void Parser::parseLine()
{
    const Token* first = advance();

    if (!first || first->type == TokenType::NEWLINE) return;

    if (first->type == TokenType::TITLE_MARKER)
        parseTitle(true); // mainPage = true
    else if (first->type == TokenType::MODULE_MARKER)
        parseTitle(false);
    else if (first->type == TokenType::HANZI)
        parseVocItem(*first); // TODO: parse sentence in vocItemSection
    else if (first->type == TokenType::TEXT)
        pushErrorAndSynchronize(ErrorCode::NO_INSTRUCTION);
    else
        pushErrorAndSynchronize(ErrorCode::UNKNOWN_PARSER_ERROR);
}