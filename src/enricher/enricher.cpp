#include "enricher.hpp"
#include "cedict.hpp"
#include <string_view>
#include <deque>
#include <iostream>

class PinyinFormatter
{
    private:
        static constexpr const char* TONE_MARKS[6][5] = {
            {"ǖ", "ǘ", "ǚ", "ǜ", "ü"},  // v (ü) = 0
            {"ū", "ú", "ǔ", "ù", "u"}, // u = 1
            {"ī", "í", "ǐ", "ì", "i"}, // i = 2
            {"ō", "ó", "ǒ", "ò", "o"}, // o = 3
            {"ē", "é", "ě", "è", "e"}, // e = 4
            {"ā", "á", "ǎ", "à", "a"} // a = 5
        };
        Errors& errors;
        std::deque<std::string>& pool;

    private:
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
        const char* makeTonedStr(int priority, int tone, std::string_view subView)
        {
            if (tone < 1 || tone > 5)
            {
                errors.push_back({ErrorPhase::ENRICHER, ErrorCode::INVALID_PINYIN_TONE, std::string(subView)});
                return nullptr;
            }
            else if (priority < 0 || priority > 5) return nullptr;

            return TONE_MARKS[priority][tone - 1];
        }
        void formatSubViewAsPinyin(std::string_view subView, std::string& formatted);
    
    public:
        PinyinFormatter(Errors& err, std::deque<std::string>& p) : errors(err), pool(p) {}
        std::string_view formatPinyin(std::string_view pinyin);
        //std::string& formatTranslation(std::string_view translation)
        //{
        //    // TODO: handle cdict auto traduction format with hanzi and pinyin inside
        //}
};

void PinyinFormatter::formatSubViewAsPinyin(std::string_view subView, std::string& formatted)
{
    if (subView.empty()) return;

    char lastChar = subView.back();
    if (!isdigit(lastChar))
    {
        formatted += subView;
        return;
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

    const char* tonedStr = makeTonedStr(currentPriority, lastChar - '0', subView);
    if (tonedStr)
    {
        formatted += subView.substr(0, tonePos);
        formatted += tonedStr;
        formatted += subView.substr(tonePos + 1, subView.size() - tonePos - 2);
    }
    else
    {
        formatted += subView;
    }
}
std::string_view PinyinFormatter::formatPinyin(std::string_view pinyin)
{
    size_t beg = 0;
    size_t end = 0;

    pool.push_back(std::string());
    std::string& formatted = pool.back();
    formatted.reserve(pinyin.size());

    while ((end = pinyin.find(' ', beg)) != std::string_view::npos)
    {
        formatSubViewAsPinyin(pinyin.substr(beg, end - beg), formatted);
        formatted += " ";
        beg = end + 1;
    }

    if (beg < pinyin.size())
    {
        formatSubViewAsPinyin(pinyin.substr(beg), formatted);
    }

    return formatted;
}

void enrichAst(MainPage& ast, const Cedict& dict, Errors& errors, std::deque<std::string>& pool)
{
    PinyinFormatter pform(errors, pool);

    for (Module& m : ast.modules)
    {
        for (VocItem& vocItem : m.vocItems)
        {

            DictEntries entries = dict.get(vocItem.hanzi);
            if (entries.empty())
            {
                errors.push_back({ErrorPhase::ENRICHER, ErrorCode::UNKWNOWN_HANZI_ENTRY, std::string(vocItem.hanzi)});
                continue;
            } 

            DictEntry entry = entries[0];
            vocItem.traditional = entry.traditional;
            if (vocItem.pinyin.empty())
            {
                vocItem.pinyin = entry.pinyin;

                if (entries.size() > 1) // handle duoyinzi
                {
                    errors.push_back({ErrorPhase::ENRICHER, ErrorCode::MULTIPLE_AUTO_PINYIN, std::string(vocItem.hanzi)});
                }
            } 
            vocItem.pinyin = pform.formatPinyin(vocItem.pinyin);

            if (vocItem.translation.empty()) vocItem.translation = entry.translation;
            //vocItem.translation = pform.formatTranslation(vocItem.translation);
        }
    }
}