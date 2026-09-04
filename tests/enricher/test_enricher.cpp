#include <gtest/gtest.h>
#include "enricher/cedict.hpp"
#include "enricher/enricher.hpp"
#include "compiler/parser.hpp"
#include <string>
#include <string_view>

struct Context
{
    Errors errors;
    MainPage ast;
    Cedict dict;
    std::string excerpt = "参 参 [can1] /variant of 參|参[can1]/\n"
        "落雨 落雨 [luo4 yu3] /(dialect) to rain/";

    Context()
    { 
        dict.init(excerpt);

        Module m;
        m.vocItems.push_back(VocItem());
        m.vocItems.push_back(VocItem());

        ast.modules.push_back(std::move(m));
    }

    VocItem& getItem1() { return ast.modules[0].vocItems[0]; }
    VocItem& getItem2() { return ast.modules[0].vocItems[1]; }
};

TEST(EnricherTest, completeEmptyVocItem)
{
    Context context;
    VocItem& item1 = context.getItem1();
    VocItem& item2 = context.getItem2();

    item1.hanzi = "参";
    item2.hanzi = "落雨";

    enrichAst(context.ast, context.dict, context.errors);

    EXPECT_EQ(item1.hanzi, "参");
    EXPECT_EQ(item1.traditional, "参");
    EXPECT_EQ(item1.pinyin, "can1");
    EXPECT_EQ(item1.translation, "variant of 參|参[can1]");

    EXPECT_EQ(item2.hanzi, "落雨");
    EXPECT_EQ(item2.traditional, "落雨");
    EXPECT_EQ(item2.pinyin, "luo4 yu3");
    EXPECT_EQ(item2.translation, "(dialect) to rain");
}

TEST(EnricherTest, dontOverwritePinyinAndTranslation)
{
    Context context;
    VocItem& item1 = context.getItem1();
    VocItem& item2 = context.getItem2();

    item1.hanzi = "参";
    item1.pinyin = "pinyin";
    item1.translation = "translation";

    item2.hanzi = "落雨";
    item2.pinyin = "pinyin";
    item2.translation = "translation";

    enrichAst(context.ast, context.dict, context.errors);

    EXPECT_EQ(item1.pinyin, "pinyin");
    EXPECT_EQ(item1.translation, "translation");

    EXPECT_EQ(item2.pinyin, "pinyin");
    EXPECT_EQ(item2.translation, "translation");
}