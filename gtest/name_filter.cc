#include <gtest/gtest.h>

#include "../src/test_runner.hpp"
#include "util.hpp"

TEST(name_filter_util, string_to_vector1)
{
  auto v = cuke::internal::to_vector("pattern1:pattern2:pattern3", ':');
  EXPECT_EQ(v.size(), 3);
  EXPECT_EQ(v[0], std::string{"pattern1"});
  EXPECT_EQ(v[1], std::string{"pattern2"});
  EXPECT_EQ(v[2], std::string{"pattern3"});
}

TEST(name_filter_util, string_to_vector2)
{
  auto v = cuke::internal::to_vector("pattern1,pattern2,pattern3", ':');
  EXPECT_EQ(v.size(), 1);
  EXPECT_EQ(v[0], std::string{"pattern1,pattern2,pattern3"});
}

TEST(name_filter, create_object) { cuke::name_filter filter(""); }
TEST(name_filter, empty_filter)
{
  cuke::name_filter filter("");
  EXPECT_TRUE(filter.matches(""));
  EXPECT_TRUE(filter.matches("a pattern"));
}
TEST(name_filter, first_match)
{
  cuke::name_filter filter("Login Scenario");
  EXPECT_TRUE(filter.matches("Login Scenario"));
}
TEST(name_filter, no_match)
{
  cuke::name_filter filter("Login Scenario");
  EXPECT_FALSE(filter.matches("Logout Scenario"));
}
TEST(name_filter, question_mark)
{
  cuke::name_filter filter("Log?n");

  EXPECT_TRUE(filter.matches("Login"));
  EXPECT_TRUE(filter.matches("Logon"));
  EXPECT_FALSE(filter.matches("Log?inout"));
}
TEST(name_filter, asterisk_prefix)
{
  cuke::name_filter filter("Scen*");
  EXPECT_TRUE(filter.matches("Scenario"));
  EXPECT_FALSE(filter.matches("AllScenario"));
}
TEST(name_filter, asterisk_sufix)
{
  cuke::name_filter filter("*Scenarios");
  EXPECT_TRUE(filter.matches("AllScenarios"));
  EXPECT_FALSE(filter.matches("Scenario"));
}
