#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include "generator/jsonGenerator.hpp"
#include "compiler/parser.hpp"
#include "compiler/types.hpp"

void generateAst(MainPage& ast)
{
    ast.title = "main title";
    ast.addModule();
    ast.addModule();
    Module& module = ast.modules[0];
    module.title = "module title";
    module.vocItems.push_back(VocItem());
    module.vocItems.push_back(VocItem());
    VocItem& voc1 = module.vocItems[0];
    VocItem& voc2 = module.vocItems[1];
    voc1.examples.push_back(SentenceItem());
    SentenceItem& sentence = voc1.examples[0];

    voc1.hanzi = "猫";
    voc1.pinyin = "voc pin1";
    voc1.traditional = "voc trad1";
    voc1.translation = "voc trans1";
    sentence.sentence = "sentence sen1";
    sentence.pinyin = "sentence pin1";
    sentence.translation = "sentence trans1";

    voc2.hanzi = "voc han2";
    voc2.pinyin = "voc pin2";
    voc2.traditional = "落雨";
    voc2.translation = "voc trans2";
}

TEST(JsonGenerator, GeneratedIsValid)
{
    MainPage ast;
    generateAst(ast);
    Errors err;
    std::string jsonString;
    astToJson(ast, jsonString, err);

    nlohmann::json json = nlohmann::json::parse(jsonString);

    EXPECT_EQ(json["title"], "main title");
    
    ASSERT_TRUE(json.contains("modules"));
    ASSERT_EQ(json["modules"].size(), 2);
    auto& module1 = json["modules"][0];
    auto& module2 = json["modules"][1];

    EXPECT_EQ(module1["title"], "module title");
    EXPECT_EQ(module1["id"], 0); 
    EXPECT_TRUE(module1["id"].is_number_integer());
    EXPECT_EQ(module2["id"], 1); 

    ASSERT_TRUE(module1.contains("vocItems"));
    ASSERT_EQ(module1["vocItems"].size(), 2);
    EXPECT_EQ(module1["vocItems"].size(), module1["vocItemsCount"]);
    auto& voc1 = module1["vocItems"][0];
    auto& voc2 = module1["vocItems"][1];
    
    EXPECT_EQ(voc1["hanzi"], "猫");
    EXPECT_EQ(voc1["translation"], "voc trans1");
    EXPECT_EQ(voc2["traditional"], "落雨");
    EXPECT_EQ(voc2["pinyin"], "voc pin2");

    ASSERT_TRUE(voc1.contains("examples"));
    ASSERT_EQ(voc1["examples"].size(), 1);
    auto& sentence = voc1["examples"][0];

    EXPECT_EQ(sentence["sentence"], "sentence sen1");
}

TEST(JsonGenerator, HandleEmptyAST)
{
    MainPage ast;
    Errors err;
    std::string jsonString;
    
    astToJson(ast, jsonString, err);
    nlohmann::json json = nlohmann::json::parse(jsonString);

    EXPECT_TRUE(json["title"].get<std::string>().empty());
    ASSERT_TRUE(json.contains("modules"));
    EXPECT_EQ(json["modules"].size(), 0);
}

TEST(JsonGenerator, IDsRemainStableOnMultipleCalls)
{
    MainPage ast;
    ast.modules.push_back(Module());
    Errors err;
    
    std::string json1, json2;
    astToJson(ast, json1, err);
    astToJson(ast, json2, err); // second call

    nlohmann::json j1 = nlohmann::json::parse(json1);
    nlohmann::json j2 = nlohmann::json::parse(json2);

    EXPECT_EQ(j1["modules"][0]["id"], j2["modules"][0]["id"]); 
}