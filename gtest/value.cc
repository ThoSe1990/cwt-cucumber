#include <gtest/gtest.h>
#include "get_args.hpp"
#include "registry.hpp"

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
TEST(value, bool_value_implicit)
{
  cuke::value v{"true"};
  bool b = true;
  EXPECT_TRUE(b);
}
TEST(value, integer_values_implicit)
{
  cuke::value v{"123"};
  int i = v;
  unsigned int ui = v;
  long l = v;
  unsigned long ul = v;
  long long ll = v;
  unsigned long long ull = v;
  std::size_t st = v;
  EXPECT_EQ(i, 123);
  EXPECT_EQ(ui, 123);
  EXPECT_EQ(l, 123);
  EXPECT_EQ(ul, 123);
  EXPECT_EQ(ll, 123);
  EXPECT_EQ(ull, 123);
  EXPECT_EQ(st, 123);
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
TEST(value, string_value_implicit)
{
  cuke::value v{std::string{"hello value"}};
  std::string str = v;
  EXPECT_EQ(str, "hello value");
}
TEST(value, string_view_value)
{
  cuke::value v{"hello value"};
  EXPECT_EQ(v.as<std::string_view>(), "hello value");
}
TEST(value, string_view_value_implicit)
{
  cuke::value v{"hello value"};
  std::string_view sv = v;
  EXPECT_EQ(sv, "hello value");
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

TEST(value_array, get_arg_w_iterator)
{
  cuke::value_array v{cuke::value("1"), cuke::value("2"), cuke::value("3"),
                      cuke::value("4")};
  int i =
      cuke::internal::get_param_value(v.begin() + 1, v.size() - 1, 2).as<int>();
  EXPECT_EQ(i, 3);
}
