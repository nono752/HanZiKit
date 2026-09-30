#include "pinyinEnricher.hpp"
#include "compiler/astTypes.hpp"
#include "error_handling/errorTypes.hpp"
#include <format>
#include <string>
#include <cassert>

constexpr const char* TONE_MARKS[6][5] = {
    {"ǖ", "ǘ", "ǚ", "ǜ", "ü"}, // v (ü) = 0
    {"ū", "ú", "ǔ", "ù", "u"}, // u = 1
    {"ī", "í", "ǐ", "ì", "i"}, // i = 2
    {"ō", "ó", "ǒ", "ò", "o"}, // o = 3
    {"ē", "é", "ě", "è", "e"}, // e = 4
    {"ā", "á", "ǎ", "à", "a"} // a = 5
};
static bool canHaveTone(char c) 
{
    return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' 
        || c == 'ü' || c == 'u:' || c == 'v';
}
static int getPriority(char c)
{
    switch (c)
    {
        case 'a': return 5;
        case 'e': return 4;
        case 'o': return 3;
        case 'i': return 2;
        case 'u': return 1;
        case 'ü': case 'u:': case 'v': return 0;
        default: return -1;
    }
}

static std::variant<const char*, ErrorCodeAndDetail> makeTonedStr(int priority, int tone, std::string_view subView)
{
    if (tone < 1 || tone > 5) return ErrorCodeAndDetail{ErrorCode::INVALID_PINYIN_TONE, std::string(subView)};
    assert(priority >= 0 && priority <= 5 && "Enricher assert: Tone priority out of bounds");
    return TONE_MARKS[priority][tone - 1];
}
static std::optional<ErrorCodeAndDetail> formatSubView(std::string_view subView, std::string& formatted)
{
    if (subView.empty()) return std::nullopt;
     
    char lastChar = subView.back();
    if (!isdigit(lastChar))
    {
        formatted += subView;
        return std::nullopt;
    }
     
    size_t tonePos = 0;
    int currentPriority = -1;
     
    // get tone position
    for (size_t i = 0; i < subView.size(); ++i)
    {
        char c = subView[i];
        if ( i + 1 < subView.size() && ((c == 'i' && subView[i + 1] == 'u') || (c == 'u' && subView[i + 1] == 'i')))
        {
            currentPriority = getPriority(subView[i + 1]);
            tonePos = i + 1;
            break;
        }
        else if (canHaveTone(c) && currentPriority < getPriority(c))
        {
            currentPriority = getPriority(c);
            tonePos = i;
        }
    }
     
    std::variant<const char*, ErrorCodeAndDetail> tonedStrOrError = makeTonedStr(currentPriority, lastChar - '0', subView);
    if (std::holds_alternative<const char*> (tonedStrOrError))
    {
        const char* tonedStr = std::get<const char*> (tonedStrOrError);
        assert(tonedStr && "Enricher assert: tonedStr is nullptr");
        formatted += subView.substr(0, tonePos);
        formatted += tonedStr;
        formatted += subView.substr(tonePos + 1, subView.size() - tonePos - 2);
    }
    else
    {
        return std::get<ErrorCodeAndDetail> (tonedStrOrError);
    }
     
    return std::nullopt;
}

std::variant<std::string_view, ErrorCodeAndDetail> formatPinyin(const VocItem& vocItem, std::deque<std::string>& pool)
{
    size_t beg = 0;
    size_t end = 0;
    std::string_view pinyin = vocItem.pinyin;
     
    pool.push_back(std::string());
    std::string& formatted = pool.back();
    formatted.reserve(pinyin.size());
     
    while ((end = pinyin.find(' ', beg)) != std::string_view::npos)
    {
        std::optional<ErrorCodeAndDetail> optVal = formatSubView(pinyin.substr(beg, end - beg), formatted);
        if (optVal) return *optVal;
        formatted += " ";
        beg = end + 1;
    }
     
    if (beg < pinyin.size())
    {
        std::optional<ErrorCodeAndDetail> optVal = formatSubView(pinyin.substr(beg), formatted);
        if (optVal) return *optVal;
    }
     
    // clean multiple and start/end spaces
    std::string cleaned = "";
    cleaned.reserve(formatted.size());
    bool inPinyin = false;
     
    for (char c : formatted)
    {
        if (c == ' ')
        {
            inPinyin = false;
            continue;
        }
             
        if (!inPinyin) 
        {
            if (!cleaned.empty()) 
            {
                cleaned += ' ';
            }
            inPinyin = true;
        }
         
        cleaned += c; 
    }
     
    return formatted = std::move(cleaned);
}
    
static std::optional<ErrorCodeAndDetail> matchHanziCount(VocItem& formattedVocItem)
{
    int pinyinCount = 0;
    bool inPinyin = false;
    int hanziCount = 0;
     
    // count pinyin
    for (char c : formattedVocItem.pinyin) 
    {
        if (c == ' ') 
            inPinyin = false;
        else if (!inPinyin) 
        {
            inPinyin = true;
            pinyinCount++;
        }
    }
     
    // count hanzi
    for (char c : formattedVocItem.hanzi)
    {
        unsigned char uc = static_cast<unsigned char> (c);
        if (!(uc >= 128 && uc <= 191)) // not an utf8 continuation octet
            hanziCount++;
            
        // RM: Won't work if it's not exclusivly hanzi
        // TODO: Either authorize only hanzi in vocitem.hanzi or accept ascii and handle here.
    }
     
    if (hanziCount != pinyinCount)
    {
        std::string detail = std::format("hanzi = {}, pinyin = {}", formattedVocItem.hanzi, formattedVocItem.pinyin);
        return ErrorCodeAndDetail{ErrorCode::PINYIN_AND_HANZI_COUNT_NEQ, detail};
    }
     
    return std::nullopt;
}
static std::optional<ErrorCodeAndDetail> matchHanzi(VocItem& formattedVocItem, const DictEntries& entries)
{
    std::deque<std::string> buffer;
    std::string detail = std::format("current pinyin = {}", formattedVocItem.pinyin);
     
    for (const DictEntry& entry : entries)
    {
        std::variant<std::string_view, ErrorCodeAndDetail> errOrVal = formatPinyin({ .hanzi = entry.hanzi, .pinyin = std::string(entry.pinyin) }, buffer);
        if (std::holds_alternative<std::string_view> (errOrVal))
        {
            std::string_view formattedEntryPinyin = std::get<std::string_view> (errOrVal);
            detail += std::format("\n > pinyin: {}, translation: {}", formattedEntryPinyin, entry.translation); // TODO: format translation
            if (formattedEntryPinyin == formattedVocItem.pinyin)
                return std::nullopt;
        }
        else
            assert(false && "Enricher assert: cedict pinyin format is invalid");
    }
     
    return ErrorCodeAndDetail{ErrorCode::PINYIN_DONT_MATCH_CEDICT, detail};
}

std::optional<ErrorCodeAndDetail> checkPinyinValidity(VocItem& formattedVocItem, const DictEntries& entries)
{
    std::optional<ErrorCodeAndDetail> optVal = matchHanziCount(formattedVocItem);
    if (optVal) return *optVal;

    optVal = matchHanzi(formattedVocItem, entries);
    if (optVal) return *optVal;

    return std::nullopt;
}