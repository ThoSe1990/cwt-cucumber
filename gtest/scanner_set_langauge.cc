#include <gtest/gtest.h>

#include "../src/scanner.hpp"

using namespace cwt::details;

TEST(set_language, langauge_1)
{
  const char* script = R"*(
  # language:  de
  Funktionalität:
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::feature);
}
TEST(set_language, langauge_2)
{
  const char* script = R"*(
  #language: de
  Funktionalität:
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::feature);
}
TEST(set_language, langauge_3)
{
  const char* script = R"*(
      #language:      de
  Funktionalität:
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::feature);
}
TEST(set_language, langauge_4)
{
  const char* script = R"*(
  #  language:      de
  Funktionalität:
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::feature);
}
TEST(set_language, langauge_5)
{
  const char* script = R"*(
  # some unnecessary comment
  # language:  de
  Funktionalität:
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::feature);
}
TEST(set_language, langauge_6)
{
  const char* script = R"*(
  # some unnecessary comment
  # language:  de
  # some unnecessary comment

  Funktionalität:
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::feature);
}

TEST(set_language, language_7)
{
  const char* script = R"*(
  # some unnecessary comment
  # and this will remain english!
  # and more ... language:  de
  
  Feature:
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::feature);
}