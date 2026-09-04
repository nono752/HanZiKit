#ifndef CEDICT_H
#define CEDICT_H

#include <string_view>
#include <string>
#include <vector>

struct DictEntry
{
    std::string_view hanzi;
    std::string_view traditional;
    std::string_view pinyin;
    std::string_view translation;
};

typedef std::vector<DictEntry> DictEntries;

class Cedict
{
    private:
        DictEntries entries;

    public:
        DictEntries get(std::string_view word) const;
        void init(const std::string& source = "");

        size_t size() const { return entries.size(); }
        const DictEntries& getEntries() const { return entries; }

    private:
        void makeEntries(std::string_view rawText);
};

#endif