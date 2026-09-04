#include "enricher.hpp"
#include "cedict.hpp"

void enrichAst(MainPage& ast, const Cedict& dict, Errors& errors)
{
    for (Module& m : ast.modules)
    {
        for (VocItem& vocItem : m.vocItems)
        {
            DictEntries entries = dict.get(vocItem.hanzi);
            if (entries.empty()) continue; // TODO GENERATE ENRICHER ERROR

            DictEntry entry = entries[0];
            vocItem.traditional = entry.traditional;
            if (vocItem.pinyin.empty()) vocItem.pinyin = entry.pinyin;
            if (vocItem.translation.empty()) vocItem.translation = entry.translation;
        }
    }
}