#include <gtest/gtest.h>
#include "compiler/lexer.hpp"
#include "compiler/parser.hpp"
#include "compiler/types.hpp"

struct ParseResult 
{
    Tokens tokens;
    MainPage ast;
    std::vector<Error> errors;
};

bool parseString(const std::string& source, ParseResult& context)
{
    context.tokens = tokenize(source, context.errors);
    if (!context.errors.empty()) return false;

    Parser parser(context.tokens, context.ast, context.errors);
    parser.parse();
    return true;
}

TEST(ParserTest, ValidCompleteAST)
{
    std::string src = 
        "# HSK 1\n"
        "## Animals\n"
        "猫 | cat\n"
        "狗 | dog\n"
        "## Multiple string title\n"
        "水 | water\n"
        "## 汉语 \n"
        "## mix 汉语\n";

    ParseResult result;
    ASSERT_TRUE(parseString(src, result)) << "There is lexer errors";

    EXPECT_TRUE(result.errors.empty()) << "Shouldn't have errors";

    // check main page
    EXPECT_EQ(result.ast.title, "HSK 1");
    ASSERT_EQ(result.ast.modules.size(), 4);

    // check module 1
    EXPECT_EQ(result.ast.modules[0].title, "Animals");
    ASSERT_EQ(result.ast.modules[0].vocItems.size(), 2);
    EXPECT_EQ(result.ast.modules[0].vocItems[0].hanzi, "猫");
    EXPECT_EQ(result.ast.modules[0].vocItems[0].translation, "cat");
    EXPECT_EQ(result.ast.modules[0].vocItems[1].hanzi, "狗");
    EXPECT_EQ(result.ast.modules[0].vocItems[1].translation, "dog");

    // check module 2
    EXPECT_EQ(result.ast.modules[1].title, "Multiple string title");
    EXPECT_EQ(result.ast.modules[1].vocItems[0].translation, "water");

    // check module 3
    EXPECT_EQ(result.ast.modules[2].title, "汉语");

    // check module 4
    EXPECT_EQ(result.ast.modules[3].title, "mix 汉语");
}

TEST(ParserTest, ErrorMultipleMainPages)
{
    std::string src = 
        "# Title 1\n"
        "# Title 2\n";

    ParseResult result;
    ASSERT_TRUE(parseString(src, result)) << "There is lexer errors";
    ASSERT_EQ(result.errors.size(), 1);

    EXPECT_EQ(result.errors[0].phase, ErrorPhase::PARSER);
    EXPECT_EQ(result.errors[0].code, ErrorCode::MULTIPLE_MAIN_PAGES);
    EXPECT_EQ(result.errors[0].details, "Title");
}

TEST(ParserTest, ErrorMissingModuleTitleOrUnexpectedSymbol) 
{
    std::string src = 
        "###\n" // Lexer generate the tokens: ## # \n
        "## \n"
        "## " ;

    ParseResult result;
    ASSERT_TRUE(parseString(src, result)) << "There is lexer errors";
    ASSERT_EQ(result.errors.size(), 3);

    EXPECT_EQ(result.errors[0].code, ErrorCode::UNEXPECTED_SYMBOL);
    EXPECT_EQ(result.errors[0].details, "#");

    EXPECT_EQ(result.errors[1].code, ErrorCode::MISSING_TITLE);
    EXPECT_EQ(result.errors[1].details, "\n");

    EXPECT_EQ(result.errors[2].code, ErrorCode::MISSING_TITLE);
    EXPECT_EQ(result.errors[2].details, "EOF");
}

TEST(ParserTest, ErrorMissingSeparator)
{
    std::string src = 
        "## Section\n"
        "猫 cat\n";

    ParseResult result;
    ASSERT_TRUE(parseString(src, result)) << "There is lexer errors";
    ASSERT_EQ(result.errors.size(), 1);

    EXPECT_EQ(result.errors[0].code, ErrorCode::MISSING_SEPARATOR);
    EXPECT_EQ(result.errors[0].details, "cat");
}

TEST(ParserTest, ErrorVocabOutsideModule)
{
    std::string src = 
        "猫 | cat\n"
        "# mainPage \n"
        "猫 | cat\n"; 

    ParseResult result;
    ASSERT_TRUE(parseString(src, result)) << "There is lexer errors";
    ASSERT_EQ(result.errors.size(), 2);
    
    EXPECT_EQ(result.errors[0].code, ErrorCode::VOCAB_OUTSIDE_MODULE);
    EXPECT_EQ(result.errors[0].details, "猫");

    EXPECT_EQ(result.errors[1].code, ErrorCode::VOCAB_OUTSIDE_MODULE);
    EXPECT_EQ(result.errors[1].details, "猫");
}