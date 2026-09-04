#include <gtest/gtest.h>
#include "enricher/cedict.hpp"
#include <string>
#include <string_view>

TEST(CedictTest, isCorrectlyParsedAndLookupIsFunctional)
{
    std::string excerpt = "㕘 参 [can1] /variant of 參|参[can1]/\n"
        "㕥 㕥 [shen1] /old variant of 呻[shen1]/\n"
        "㕥 以 [yi3] /old variant of 以[yi3]/\n"
        "㕥 以 [duoyin test] /old variant of 以[yi3]/\n"
        "落雨 落雨 [luo4 yu3] /(dialect) to rain/";

    Cedict dict;
    dict.init(excerpt);
    ASSERT_EQ(dict.size(), 5);

    DictEntries entries = dict.get("参");
    ASSERT_EQ(entries.size(), 1);
    DictEntry entry = entries[0];
    EXPECT_EQ(entry.hanzi, "参");
    EXPECT_EQ(entry.traditional, "㕘");
    EXPECT_EQ(entry.pinyin, "can1");
    EXPECT_EQ(entry.translation, "variant of 參|参[can1]");
    
    entries = dict.get("落雨");
    ASSERT_EQ(entries.size(), 1);
    entry = entries[0];
    EXPECT_EQ(entry.hanzi, "落雨");
    EXPECT_EQ(entry.traditional, "落雨");
    EXPECT_EQ(entry.pinyin, "luo4 yu3");
    EXPECT_EQ(entry.translation, "(dialect) to rain");

    // multiple entry for a key
    entries = dict.get("以");
    ASSERT_EQ(entries.size(), 2);
    EXPECT_EQ(entries[0].pinyin, "yi3");
    EXPECT_EQ(entries[1].pinyin, "duoyin test");

    // not in the dict
    entries = dict.get("猫");
    ASSERT_EQ(entries.size(), 0);
}

TEST(CedictTest, ignoreEmptyLinesAndCommentary)
{
    std::string excerpt = "㕘 参 [can1] /variant of 參|参[can1]/\n"
        "\n"
        "\n"
        "落雨 落雨 [luo4 yu3] /(dialect) to rain/\n"
        "# commentary\n"
        "# commentary\n"
        "㕥 㕥 [shen1] /old variant of 呻[shen1]/";
    
    Cedict dict;
    dict.init(excerpt);
    ASSERT_EQ(dict.size(), 3);

    DictEntries entries = dict.get("参");
    ASSERT_EQ(entries.size(), 1);
    DictEntry entry = entries[0];
    EXPECT_EQ(entry.hanzi, "参");
    EXPECT_EQ(entry.traditional, "㕘");
    EXPECT_EQ(entry.pinyin, "can1");
    EXPECT_EQ(entry.translation, "variant of 參|参[can1]");

    entries = dict.get("落雨");
    ASSERT_EQ(entries.size(), 1);
    entry = entries[0];
    EXPECT_EQ(entry.hanzi, "落雨");
    EXPECT_EQ(entry.traditional, "落雨");
    EXPECT_EQ(entry.pinyin, "luo4 yu3");
    EXPECT_EQ(entry.translation, "(dialect) to rain");

    entries = dict.get("㕥");
    ASSERT_EQ(entries.size(), 1);
    entry = entries[0];
    EXPECT_EQ(entry.hanzi, "㕥");
    EXPECT_EQ(entry.traditional, "㕥");
    EXPECT_EQ(entry.pinyin, "shen1");
    EXPECT_EQ(entry.translation, "old variant of 呻[shen1]");
}

TEST(CedictTest, BadFormatReturnEmptyEntry)
{
    std::string excerpt = "㕘 参 [can1] /variant of 參|参[can1]/\n"
        "落雨 落雨 [luo4 yu3 /(dialect) to rain [luo4 yu3]/\n" // miss ]
        "落雨 落雨 [luo4 yu3] (dialect) to rain/\n" // miss /
        "落雨 [luo4 yu3] /(dialect) to rain/"; // miss traditional
    
    Cedict dict;
    dict.init(excerpt);

    ASSERT_EQ(dict.size(), 1);

    DictEntries entries = dict.get("参");
    ASSERT_EQ(entries.size(), 1);
    DictEntry entry = entries[0];
    EXPECT_EQ(entry.hanzi, "参");
    EXPECT_EQ(entry.traditional, "㕘");
    EXPECT_EQ(entry.pinyin, "can1");
    EXPECT_EQ(entry.translation, "variant of 參|参[can1]");

    entries = dict.get("落雨");
    EXPECT_TRUE(entries.empty());
}