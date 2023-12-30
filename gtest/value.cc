#include <gtest/gtest.h>

#include "../src/cwt/value.hpp"

using namespace cwt::details;

TEST(value, init_obj)
{
  value v;
  EXPECT_EQ(v.type(), value_type::nil);
}

TEST(value, bool_value)
{
  value v{true};
  EXPECT_EQ(v.type(), value_type::boolean);
  EXPECT_EQ(v.as<bool>(), true);
}
TEST(value, int_value)
{
  value v{123};
  EXPECT_EQ(v.type(), value_type::integral);
  EXPECT_EQ(v.as<int>(), 123);
  EXPECT_EQ(v.as<unsigned int>(), 123);
  EXPECT_EQ(v.as<long>(), 123);
  EXPECT_EQ(v.as<unsigned long>(), 123);
}
TEST(value, float_value)
{
  value v{1.1f};
  EXPECT_EQ(v.type(), value_type::floating);
  EXPECT_EQ(v.as<float>(), 1.1f);
}
TEST(value, double_value)
{
  value v{1.1};
  EXPECT_EQ(v.type(), value_type::floating);
  EXPECT_EQ(v.as<double>(), 1.1);
}
TEST(value, string_value)
{
  value v{std::string{"hello value"}};
  EXPECT_EQ(v.type(), value_type::string);
  EXPECT_EQ(v.as<std::string>(), "hello value");
}
TEST(value, string_view_value)
{
  value v{std::string_view{"hello value"}};
  EXPECT_EQ(v.type(), value_type::string);
  EXPECT_EQ(v.as<std::string_view>(), "hello value");
}

