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
TEST(name_filter, first_match)
{
  cuke::name_filter filter("Login Scenario");
  std::string match("Login Scenario");
  EXPECT_TRUE(filter.matches(match));
}
TEST(name_filter, no_match)
{
  cuke::name_filter filter("Login Scenario");
  std::string match("Logout Scenario");
  EXPECT_FALSE(filter.matches(match));
}
