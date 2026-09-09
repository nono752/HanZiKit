#include "cedict.hpp"
#include "cedictData.hpp"
#include <algorithm>

extern const unsigned char cedictData[];
extern const unsigned int cedictEntriesCount;
extern const unsigned long long cedictDataSize;

struct KeyCompHanzi
{
    bool operator()(const DictEntry& entry, std::string_view key) const { return entry.hanzi < key; }
    bool operator()(std::string_view key, const DictEntry& entry) const { return key < entry.hanzi; }
};

void Cedict::init(const std::string& source)
{
    std::string_view rawText;

    if (source.empty()) // default
        rawText = std::string_view(reinterpret_cast<const char*> (cedictData), cedictDataSize);
    else
        rawText = source;

    makeEntries(rawText);

    std::sort(entries.begin(), entries.end(), 
        [](const DictEntry& a, const DictEntry& b)
        {
            return a.hanzi < b.hanzi;
        }
    );
}

DictEntries Cedict::get(std::string_view word) const
{
    // all entries that have the key
    auto [first, last] = std::equal_range(entries.begin(), entries.end(), word, KeyCompHanzi{});

    DictEntries possibilies;
    for (auto it = first; it != last; ++it) 
        possibilies.push_back(*it);
    
    return possibilies;
}

void Cedict::makeEntries(std::string_view rawText)
{
    entries.reserve(cedictEntriesCount); 
    size_t start = 0;
    while (start < rawText.size())
    {
        size_t endLine = rawText.find('\n', start);
        std::string_view line = endLine != std::string_view::npos ? rawText.substr(start, endLine - start) : rawText.substr(start);
        start = endLine != std::string_view::npos ? endLine + 1 : rawText.size();
        
        if (line.empty() || line[0] == '#') continue;

        size_t space1 = line.find(' ');
        if (space1 == std::string_view::npos) continue;
        
        size_t space2 = line.find(' ', space1 + 1);
        if (space2 == std::string_view::npos) continue;
        
        size_t bracket1 = line.find('[', space2 + 1);
        if (bracket1 == std::string_view::npos) continue;
        
        size_t bracket2 = line.find(']', bracket1 + 1);
        if (bracket2 == std::string_view::npos) continue;
        
        size_t slash1 = line.find('/', bracket2 + 1);
        if (slash1 == std::string_view::npos) continue;
        
        size_t lastSlash = line.find_last_of('/'); 
        if (lastSlash == std::string_view::npos || lastSlash <= slash1) continue;

        DictEntry entry;
        entry.traditional = line.substr(0, space1);
        entry.hanzi = line.substr(space1 + 1, space2 - space1 - 1);
        entry.pinyin = line.substr(bracket1 + 1, bracket2 - bracket1 - 1);
        entry.translation = line.substr(slash1 + 1, lastSlash - slash1 - 1);
        entries.push_back(entry);
    }
}