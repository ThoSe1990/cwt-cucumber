#include <gtest/gtest.h>

#include "../src/scanner.hpp"

using namespace cuke::internal;

TEST(english_keywords, feature)
{
  EXPECT_EQ(scanner("Feature:").scan_token().type, token_type::feature);
}
TEST(english_keywords, scenario)
{
  EXPECT_EQ(scanner("Scenario:").scan_token().type, token_type::scenario);
}
TEST(english_keywords, scenario_outline)
{
  EXPECT_EQ(scanner("Scenario Outline:").scan_token().type,
            token_type::scenario_outline);
}
TEST(english_keywords, background)
{
  EXPECT_EQ(scanner("Background:").scan_token().type, token_type::background);
}
TEST(english_keywords, examples)
{
  EXPECT_EQ(scanner("Examples:").scan_token().type, token_type::examples);
}
TEST(english_keywords, given)
{
  EXPECT_EQ(scanner("Given").scan_token().type, token_type::step);
}
TEST(english_keywords, when)
{
  EXPECT_EQ(scanner("When").scan_token().type, token_type::step);
}
TEST(english_keywords, then)
{
  EXPECT_EQ(scanner("Then").scan_token().type, token_type::step);
}
TEST(english_keywords, and)
{
  EXPECT_EQ(scanner("And").scan_token().type, token_type::step);
}
TEST(english_keywords, but)
{
  EXPECT_EQ(scanner("But").scan_token().type, token_type::step);
}
