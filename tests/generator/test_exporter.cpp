#include <gtest/gtest.h>
#include <generator/exporter.hpp>
#include <compiler/types.hpp>

TEST(ExporterTest, ValidInjection)
{
    Errors err;
    std::string base = "<html> null /* {{JSON_INJECT}} */ </html>";
    std::string json = "{\"data\": 42}";
    
    std::string result;
    injectJson(base, json, result, err);
    EXPECT_EQ(result, "<html> {\"data\": 42} </html>");
}

TEST(ExporterTest, EmptyJsonProduceValidHtml)
{
    Errors err;
    std::string base = "<html> const DATA = null /* {{JSON_INJECT}} */; </html>";
    std::string json = "";
    
    std::string result;
    injectJson(base, json, result, err);
    EXPECT_EQ(result, "<html> const DATA = null; </html>");
}

TEST(ExporterTest, minimalHtmlDontCrash)
{
    Errors err;
    std::string base = "null /* {{JSON_INJECT}} */";
    std::string json = "big json\n"
        "some more json\n"
        "one more line";
    
    std::string result;
    injectJson(base, json, result, err);
    EXPECT_EQ(result, "big json\n" "some more json\n" "one more line");
}

TEST(ExporterTest, MultipleJsonTagsReturnEmpty)
{
    Errors err;
    std::string base = "null /* {{JSON_INJECT}} */\n" " fill...\n" "null /* {{JSON_INJECT}} */";
    std::string json = "{}";

    std::string result;
    injectJson(base, json, result, err);
    EXPECT_TRUE(result.empty()) << result;
}

TEST(ExporterTest, MissingJsonTagsReturnEmpty)
{
    Errors err;
    std::string base = "<html> pas de marqueur </html>";
    std::string json = "{}";

    std::string result;
    injectJson(base, json, result, err);
    EXPECT_TRUE(result.empty());
}