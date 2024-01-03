#include <gtest/gtest.h>

#include "../src/cwt/value.hpp"
#include "../src/cwt/object.hpp"

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
TEST(value, invalid_access)
{
  value v{true};
  EXPECT_THROW(v.as<int>(), std::runtime_error);
}
TEST(value, int_value)
{
  value v{123ul};
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
TEST(value, function_value)
{
  function test_function{0, chunk{}, "some name"};
  value v{test_function};
  EXPECT_EQ(v.type(), value_type::function);
  EXPECT_EQ(v.as<function>().name, "some name");
}

TEST(value, emplace_value)
{
  value v;
  v.emplace_or_replace(123);
  EXPECT_EQ(v.type(), value_type::integral);
  EXPECT_EQ(v.as<int>(), 123);
}
TEST(value, replace_value_1)
{
  value v{123};
  v.emplace_or_replace(999);
  EXPECT_EQ(v.type(), value_type::integral);
  EXPECT_EQ(v.as<int>(), 999);
}
TEST(value, replace_value_2)
{
  value v{123};
  v.emplace_or_replace(std::string{"other value"});
  EXPECT_EQ(v.type(), value_type::string);
  EXPECT_EQ(v.as<std::string>(), std::string{"other value"});
}
