#include <gtest/gtest.h>
#include <string>
#include "compiler/lexer.hpp" 
#include "compiler/tokenTypes.hpp" 
#include "error_handling/errorTypes.hpp" 

TEST(LexerTest, ignoreSpacesAndIdentifyText) 
{
    std::string source = "word1\t word2    $";
    Errors errors;
    Tokens tokens = tokenize(source, errors);

    ASSERT_EQ(tokens.size(), 3); 
    
    EXPECT_EQ(tokens[0].type, TokenType::TEXT);
    EXPECT_EQ(tokens[0].data, "word1");
    EXPECT_EQ(tokens[1].type, TokenType::TEXT);
    EXPECT_EQ(tokens[1].data, "word2");
    EXPECT_EQ(tokens[2].type, TokenType::TEXT);
    EXPECT_EQ(tokens[2].data, "$");
}

TEST(LexerTest, chineseFormatIsCorrect) 
{
    std::string source = "猫 | cat\n";
    Errors errors;
    Tokens tokens = tokenize(source, errors);
    
    ASSERT_EQ(tokens.size(), 4);

    EXPECT_EQ(tokens[0].type, TokenType::HANZI);
    EXPECT_EQ(tokens[0].data, "猫");
    
    EXPECT_EQ(tokens[1].type, TokenType::SEPARATOR_MARKER);
    EXPECT_EQ(tokens[1].data, "|");
    
    EXPECT_EQ(tokens[2].type, TokenType::TEXT);
    EXPECT_EQ(tokens[2].data, "cat");

    EXPECT_EQ(tokens[3].type, TokenType::NEWLINE);
    EXPECT_EQ(tokens[3].data, "\n");
}

TEST(LexerTest, readSpecialCharacterCorrectly)
{
    std::string source = "#|## \n ## #";
    Errors errors;
    Tokens tokens = tokenize(source, errors);

    ASSERT_EQ(tokens.size(), 6);

    EXPECT_EQ(tokens[0].type, TokenType::TITLE_MARKER);
    EXPECT_EQ(tokens[0].data, "#");

    EXPECT_EQ(tokens[1].type, TokenType::SEPARATOR_MARKER);
    EXPECT_EQ(tokens[1].data, "|");

    EXPECT_EQ(tokens[2].type, TokenType::MODULE_MARKER);
    EXPECT_EQ(tokens[2].data, "##");

    EXPECT_EQ(tokens[3].type, TokenType::NEWLINE);
    EXPECT_EQ(tokens[3].data, "\n");

    EXPECT_EQ(tokens[4].type, TokenType::MODULE_MARKER);
    EXPECT_EQ(tokens[4].data, "##");

    EXPECT_EQ(tokens[5].type, TokenType::TITLE_MARKER);
    EXPECT_EQ(tokens[5].data, "#");
}

TEST(LexerTest, mixedUtf8AndAsciiMakeText) 
{
    std::string source = "Apple苹果";
    Errors errors;
    Tokens tokens = tokenize(source, errors);

    ASSERT_EQ(tokens.size(), 1);
    
    EXPECT_EQ(tokens[0].type, TokenType::TEXT);
    EXPECT_EQ(tokens[0].data, "Apple苹果");
}

TEST(LexerTest, nullStringIsSupported) 
{
    std::string source = "";
    Errors errors;
    Tokens tokens = tokenize(source, errors);
    EXPECT_EQ(tokens.size(), 0);
}