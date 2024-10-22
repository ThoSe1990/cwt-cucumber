#include <gtest/gtest.h>

#include "../src/value.hpp"

using namespace cuke::internal;

TEST(value, init_obj)
{
  cuke::value v;
  EXPECT_TRUE(v.is_nil());
}

TEST(value, bool_value)
{
  cuke::value v{"true"};
  EXPECT_EQ(v.as<bool>(), true);
}
TEST(value, long_value)
{
  cuke::value v{"123"};
  EXPECT_EQ(v.as<int>(), 123);
  EXPECT_EQ(v.as<unsigned int>(), 123);
  EXPECT_EQ(v.as<long>(), 123);
  EXPECT_EQ(v.as<unsigned long>(), 123);
}
TEST(value, std_size_t)
{
  cuke::value v("234");
  EXPECT_EQ(v.as<std::size_t>(), 234);
}
TEST(value, float_value)
{
  cuke::value v{"1.1"};
  EXPECT_EQ(v.as<float>(), 1.1f);
  EXPECT_EQ(v.as<double>(), 1.1);
}
TEST(value, string_value)
{
  cuke::value v{std::string{"hello value"}};
  EXPECT_EQ(v.as<std::string>(), "hello value");
}
TEST(value, string_view_value)
{
  cuke::value v{"hello value"};
  EXPECT_EQ(v.as<std::string_view>(), "hello value");
}
TEST(value, emplace_value)
{
  cuke::value v("456");
  v.emplace_or_replace("123");
  EXPECT_EQ(v.as<int>(), 123);
}
TEST(value, to_string_string)
{
  cuke::value v(std::string("hello world"));
  EXPECT_EQ(v.to_string(), std::string("hello world"));
}
