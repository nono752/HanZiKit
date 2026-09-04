#include "generator.hpp"
#include "compiler/astTypes.hpp"
#include <nlohmann/json.hpp>

void to_json(nlohmann::json& json, const SentenceItem& sItem) 
{
    json = nlohmann::json{
        {"hanzi", sItem.sentence},
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
        {"Examples", vItem.examples}
    };
}

void to_json(nlohmann::json& json, const Module& m)
{
    static size_t id = 0;

    json = nlohmann::json{
        {"id", id++},
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

void astToJson(const MainPage& ast, Errors& errors, std::string& toWrite)
{
    nlohmann::json json = ast;
    toWrite = json.dump(2);
}