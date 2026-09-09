#include "jsonGenerator.hpp"
#include "compiler/astTypes.hpp"
#include <nlohmann/json.hpp>

void to_json(nlohmann::json& json, const SentenceItem& sItem) 
{
    json = nlohmann::json{
        {"sentence", sItem.sentence},
        {"pinyin", sItem.pinyin},
        {"translation", sItem.translation}
    };
}

void to_json(nlohmann::json& json, const VocItem& vItem) 
{
    json = nlohmann::json{
        {"hanzi", vItem.hanzi},
        {"traditional", vItem.traditional},
        {"pinyin", vItem.pinyin},
        {"translation", vItem.translation},
        {"examples", vItem.examples}
    };
}

void to_json(nlohmann::json& json, const Module& m)
{
    json = nlohmann::json{
        {"id", m.id},
        {"title", m.title},
        {"vocItems", m.vocItems},
        {"vocItemsCount", m.vocItems.size()}
    };
}

void to_json(nlohmann::json& json, const MainPage& mp)
{
    json = nlohmann::json{
        {"title", mp.title},
        {"modules", mp.modules}
    };
}

void astToJson(const MainPage& ast, std::string& toWrite, Errors& errors)
{
    toWrite.clear();
    nlohmann::json json = ast;
    toWrite = json.dump(2);
}