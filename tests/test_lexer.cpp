#include <gtest/gtest.h>
#include <string>
#include "lexer.hpp" 
#include "types.hpp" 

TEST(LexerTest, ignoreSpacesAndIdentifyWords) 
{
    std::string source = "word1   word2";
    std::vector<Error> errors;
    Tokens tokens = tokenize(source, errors);

    ASSERT_EQ(tokens.size(), 2); 
    
    EXPECT_EQ(tokens[0].type, TokenType::TEXT);
    EXPECT_EQ(tokens[0].data, "word1");
    
    EXPECT_EQ(tokens[1].type, TokenType::TEXT);
    EXPECT_EQ(tokens[1].data, "word2");
}

TEST(LexerTest, chineseFormatIsCorrect) 
{
    std::string source = "猫 | cat\n";
    std::vector<Error> errors;
    Tokens tokens = tokenize(source, errors);
    
    ASSERT_EQ(tokens.size(), 4);

    EXPECT_EQ(tokens[0].type, TokenType::HANZI);
    EXPECT_EQ(tokens[0].data, "猫");
    
    EXPECT_EQ(tokens[1].type, TokenType::SPECIAL_CHAR);
    EXPECT_EQ(tokens[1].data, "|");
    
    EXPECT_EQ(tokens[2].type, TokenType::TEXT);
    EXPECT_EQ(tokens[2].data, "cat");

    EXPECT_EQ(tokens[3].type, TokenType::NEWLINE);
    EXPECT_EQ(tokens[3].data, "\n");
}

TEST(LexerTest, readSpecialCharacterCorrectly)
{
    std::string source = "#|## \n ## #";
    std::vector<Error> errors;
    Tokens tokens = tokenize(source, errors);

    ASSERT_EQ(tokens.size(), 6);

    EXPECT_EQ(tokens[0].type, TokenType::SPECIAL_CHAR);
    EXPECT_EQ(tokens[0].data, "#");

    EXPECT_EQ(tokens[1].type, TokenType::SPECIAL_CHAR);
    EXPECT_EQ(tokens[1].data, "|");

    EXPECT_EQ(tokens[2].type, TokenType::SPECIAL_CHAR);
    EXPECT_EQ(tokens[2].data, "##");

    EXPECT_EQ(tokens[3].type, TokenType::NEWLINE);
    EXPECT_EQ(tokens[3].data, "\n");

    EXPECT_EQ(tokens[4].type, TokenType::SPECIAL_CHAR);
    EXPECT_EQ(tokens[4].data, "##");

    EXPECT_EQ(tokens[5].type, TokenType::SPECIAL_CHAR);
    EXPECT_EQ(tokens[5].data, "#");
}

TEST(LexerTest, mixedUtf8AndAsciiMakeText) 
{
    std::string source = "Apple苹果";
    std::vector<Error> errors;
    Tokens tokens = tokenize(source, errors);

    ASSERT_EQ(tokens.size(), 1);
    
    EXPECT_EQ(tokens[0].type, TokenType::TEXT);
    EXPECT_EQ(tokens[0].data, "Apple苹果");
}

TEST(LexerTest, registerUnknownCharacters) 
{
    std::string source = "cat $ 猫";
    std::vector<Error> errors;
    Tokens tokens = tokenize(source, errors);

    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[1].type, TokenType::UNKNOWN_CHAR);
    EXPECT_EQ(tokens[1].data, "$");
}

TEST(LexerTest, nullStringIsSupported) 
{
    std::string source = "";
    std::vector<Error> errors;
    Tokens tokens = tokenize(source, errors);
    EXPECT_EQ(tokens.size(), 0);
}