#include <gtest/gtest.h>

#include "../src/cwt/scanner.hpp"

using namespace cwt::details;

TEST(scanner, init_obj) { cwt::details::scanner scanner("some script"); }

TEST(scanner, scan_vertical)
{
  token t = scanner("|").scan_token();
  EXPECT_EQ(t.type, token_type::vertical);
  EXPECT_EQ(t.value.size(), 1);
  EXPECT_STREQ(t.value.data(), "|");
}
TEST(scanner, scan_minus)
{
  EXPECT_EQ(scanner("-").scan_token().type, token_type::minus);
}
TEST(scanner, asterisk)
{
  EXPECT_EQ(scanner("*").scan_token().type, token_type::step);
}
TEST(scanner, tag)
{
  EXPECT_EQ(scanner("@my_tag").scan_token().type, token_type::tag);
}
TEST(scanner, language_1)
{
  EXPECT_EQ(scanner("#language:").scan_token().type, token_type::language);
}
TEST(scanner, language_2)
{
  EXPECT_EQ(scanner("#  language:").scan_token().type, token_type::language);
}
TEST(scanner, variable)
{
  EXPECT_EQ(scanner("<some variable>").scan_token().type, token_type::variable);
}
TEST(scanner, variable_missing_closing)
{
  EXPECT_EQ(scanner("<some variable").scan_token().type, token_type::error);
}
TEST(scanner, string)
{
  EXPECT_EQ(scanner("\"some string\"").scan_token().type,
            token_type::string_value);
}
TEST(scanner, string_missing_closing)
{
  EXPECT_EQ(scanner("\"some string").scan_token().type, token_type::error);
}
TEST(scanner, long_value)
{
  EXPECT_EQ(scanner("99").scan_token().type, token_type::long_value);
}
TEST(scanner, double_value)
{
  EXPECT_EQ(scanner("99.99").scan_token().type, token_type::double_value);
}
TEST(scanner, double_value_2)
{
  EXPECT_EQ(scanner("99.").scan_token().type, token_type::double_value);
}
TEST(scanner, arbitrary_text)
{
  EXPECT_EQ(scanner("foo").scan_token().type, token_type::text);
}
TEST(scanner, arbitrary_text_2)
{
  scanner s("some text");
  EXPECT_EQ(s.scan_token().type, token_type::text);
  EXPECT_EQ(s.scan_token().type, token_type::text);
}
TEST(scanner, statement_1)
{
  scanner s("Step with <var> and 99");

  EXPECT_EQ(s.scan_token().type, token_type::text);
  EXPECT_EQ(s.scan_token().type, token_type::text);
  EXPECT_EQ(s.scan_token().type, token_type::text);

  EXPECT_EQ(s.scan_token().type, token_type::variable);

  EXPECT_EQ(s.scan_token().type, token_type::text);

  EXPECT_EQ(s.scan_token().type, token_type::long_value);
}

TEST(scanner, statement_2)
{
  scanner s("Step with \"some string value\" and 99.99");

  EXPECT_EQ(s.scan_token().type, token_type::text);
  EXPECT_EQ(s.scan_token().type, token_type::text);
  EXPECT_EQ(s.scan_token().type, token_type::text);

  EXPECT_EQ(s.scan_token().type, token_type::string_value);

  EXPECT_EQ(s.scan_token().type, token_type::text);

  EXPECT_EQ(s.scan_token().type, token_type::double_value);
}
TEST(scanner, table_header)
{
  scanner s("| some | variables | here |");

  EXPECT_EQ(s.scan_token().type, token_type::vertical);
  EXPECT_EQ(s.scan_token().type, token_type::text);
  EXPECT_EQ(s.scan_token().type, token_type::vertical);
  EXPECT_EQ(s.scan_token().type, token_type::text);
  EXPECT_EQ(s.scan_token().type, token_type::vertical);
  EXPECT_EQ(s.scan_token().type, token_type::text);
  EXPECT_EQ(s.scan_token().type, token_type::vertical);
}