#include "enricher.hpp"
#include "pinyinEnricher.hpp"
#include "cedict.hpp"
#include <format>

class Internal_VocItemFormatter
{
    private:
        Errors& errors;
        std::deque<std::string>& pool;
        const VocItem* currentVocItem = nullptr;

    private:
        void pushError(const ErrorCodeAndDetail& err) { errors.push_back({ErrorPhase::ENRICHER, 
            err.code, 
            err.detail,
            currentVocItem->line,
            currentVocItem->col}); 
        }

    public:
        Internal_VocItemFormatter(Errors& err, std::deque<std::string>& p) : errors(err), pool(p) {}
        void format(VocItem& vocItem)
        {
            currentVocItem = &vocItem;

            std::variant<std::string_view, ErrorCodeAndDetail> errOrVal = formatPinyin(vocItem, pool);
            if (std::holds_alternative<ErrorCodeAndDetail> (errOrVal))
                pushError(std::get<ErrorCodeAndDetail> (errOrVal));
            else
                vocItem.pinyin = std::get<std::string_view> (errOrVal);
        }
};

void enrichAst(MainPage& ast, const Cedict& dict, Errors& errors, std::deque<std::string>& pool)
{
    Internal_VocItemFormatter vocForm(errors, pool);

    for (Module& m : ast.modules)
    {
        for (VocItem& vocItem : m.vocItems)
        {

            DictEntries entries = dict.get(vocItem.hanzi);
            if (entries.empty())
            {
                errors.push_back({ErrorPhase::ENRICHER, ErrorCode::UNKWNOWN_HANZI_ENTRY, std::string(vocItem.hanzi), vocItem.line, vocItem.col});
                continue;
            } 

            // Complete vocItems with cedict
            DictEntry entry = entries[0];
            vocItem.traditional = entry.traditional;
            if (vocItem.pinyin.empty() && vocItem.translation.empty())
            {
                vocItem.pinyin = entry.pinyin;
                vocItem.translation = entry.translation;

                if (entries.size() > 1) // TODO: handle duoyinzi with warning instead of errors
                {
                    std::string detail = std::format("hanzi = {}", vocItem.hanzi);
                    for (const DictEntry& entry : entries)
                    {
                        detail += std::format("\n > pinyin = {}, translation = {}", entry.pinyin, entry.translation);
                    }
                    errors.push_back({ErrorPhase::ENRICHER, ErrorCode::MULTIPLE_AUTO_COMPLETION, detail, vocItem.line, vocItem.col});
                }
            }

            vocForm.format(vocItem);

            std::optional<ErrorCodeAndDetail> optVal = checkPinyinValidity(vocItem, entries);
            if (optVal)
                errors.push_back({ErrorPhase::ENRICHER, optVal->code, optVal->detail, vocItem.line, vocItem.col});
            
        }
    }
}