#ifndef AST_TYPES_H
#define AST_TYPES_H

#include <string_view>
#include <vector>

struct SentenceItem
{
    std::string_view sentence;
    std::string_view pinyin;
    std::string_view translation;
};

struct VocItem
{
    std::string_view hanzi;
    std::string_view traditional;
    std::string_view pinyin;
    std::string_view translation;
    std::vector<SentenceItem> examples;
};

struct Module
{
    std::string_view title;
    std::vector<VocItem> vocItems;
};

struct MainPage
{
    std::string_view title;
    std::vector<Module> modules;
};

#endif