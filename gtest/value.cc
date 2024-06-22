#include <gtest/gtest.h>

#include "../src/value.hpp"
#include "../src/chunk.hpp"

using namespace cwt::details;

TEST(value, init_obj)
{
  cuke::value v;
  EXPECT_EQ(v.type(), cuke::value_type::nil);
}

TEST(value, bool_value)
{
  cuke::value v{true};
  EXPECT_EQ(v.type(), cuke::value_type::boolean);
  EXPECT_EQ(v.as<bool>(), true);
}
TEST(value, invalid_access)
{
  cuke::value v{true};
  EXPECT_THROW(v.as<int>(), std::runtime_error);
}
TEST(value, ulong_value)
{
  cuke::value v{123ul};
  EXPECT_EQ(v.type(), cuke::value_type::integral);
  EXPECT_EQ(v.as<int>(), 123);
  EXPECT_EQ(v.as<unsigned int>(), 123);
  EXPECT_EQ(v.as<long>(), 123);
  EXPECT_EQ(v.as<unsigned long>(), 123);
}
TEST(value, long_value_1)
{
  cuke::value v(123l);
  EXPECT_EQ(v.type(), cuke::value_type::integral);
  EXPECT_EQ(v.as<int>(), 123);
  EXPECT_EQ(v.as<unsigned int>(), 123);
  EXPECT_EQ(v.as<long>(), 123);
  EXPECT_EQ(v.as<unsigned long>(), 123);
}
TEST(value, std_size_t)
{
  std::size_t i = 234;
  cuke::value v(i);
  EXPECT_EQ(v.as<std::size_t>(), i);
}
TEST(value, int_as_std_size_t)
{
  int i = 234;
  cuke::value v(i);
  EXPECT_EQ(v.as<std::size_t>(), i);
}
TEST(value, long_value_2)
{
  long number = 99;
  cuke::value v(number);
  EXPECT_EQ(v.type(), cuke::value_type::integral);
}
TEST(value, float_value)
{
  cuke::value v{1.1f};
  EXPECT_EQ(v.type(), cuke::value_type::floating);
  EXPECT_EQ(v.as<float>(), 1.1f);
}
TEST(value, double_value)
{
  cuke::value v{1.1};
  EXPECT_EQ(v.type(), cuke::value_type::_double);
  EXPECT_EQ(v.as<double>(), 1.1);
}
TEST(value, string_value)
{
  cuke::value v{std::string{"hello value"}};
  EXPECT_EQ(v.type(), cuke::value_type::string);
  EXPECT_EQ(v.as<std::string>(), "hello value");
}
TEST(value, string_view_value)
{
  cuke::value v{std::string_view{"hello value"}};
  EXPECT_EQ(v.type(), cuke::value_type::string);
  EXPECT_EQ(v.as<std::string_view>(), "hello value");
}
TEST(value, function_value)
{
  cuke::value v(function{std::make_unique<chunk>(std::string("some name"))});
  EXPECT_EQ(v.type(), cuke::value_type::function);
  EXPECT_EQ(v.as<function>()->name(), "some name");
}

TEST(value, nil_value)
{
  cuke::value v(nil_value{});
  EXPECT_EQ(v.type(), cuke::value_type::nil);
}
TEST(value, emplace_value)
{
  cuke::value v;
  v.emplace_or_replace(123);
  EXPECT_EQ(v.type(), cuke::value_type::integral);
  EXPECT_EQ(v.as<int>(), 123);
}
TEST(value, replace_value_1)
{
  cuke::value v{123};
  v.emplace_or_replace(999);
  EXPECT_EQ(v.type(), cuke::value_type::integral);
  EXPECT_EQ(v.as<int>(), 999);
}
TEST(value, replace_value_2)
{
  cuke::value v{123};
  v.emplace_or_replace(std::string{"other value"});
  EXPECT_EQ(v.type(), cuke::value_type::string);
  EXPECT_EQ(v.as<std::string>(), std::string{"other value"});
}
TEST(value, copy_integer)
{
  cuke::value v1{123};
  cuke::value v2(v1);
  EXPECT_EQ(v2.as<int>(), v1.as<int>());
  EXPECT_EQ(v2.as<int>(), 123);
  EXPECT_EQ(v2.type(), cuke::value_type::integral);
}
TEST(value, copy_double)
{
  cuke::value v1{2.2};
  cuke::value v2(v1);
  EXPECT_EQ(v2.as<double>(), v1.as<double>());
  EXPECT_EQ(v2.as<double>(), 2.2);
  EXPECT_EQ(v2.type(), cuke::value_type::_double);
}
TEST(value, copy_boolean)
{
  cuke::value v1{true};
  cuke::value v2(v1);
  EXPECT_EQ(v2.as<bool>(), v1.as<bool>());
  EXPECT_EQ(v2.as<bool>(), true);
  EXPECT_EQ(v2.type(), cuke::value_type::boolean);
}
TEST(value, copy_string)
{
  cuke::value v1{std::string("hello copy")};
  cuke::value v2(v1);
  EXPECT_EQ(v2.as<std::string>(), v1.as<std::string>());
  EXPECT_EQ(v2.as<std::string>(), std::string("hello copy"));
  EXPECT_EQ(v2.type(), cuke::value_type::string);
}
TEST(value, assign_string)
{
  cuke::value v1{std::string("hello copy")};
  cuke::value v2 = v1;
  EXPECT_EQ(v2.as<std::string>(), v1.as<std::string>());
  EXPECT_EQ(v2.as<std::string>(), std::string("hello copy"));
  EXPECT_EQ(v2.type(), cuke::value_type::string);
}
TEST(value, copy_function_object)
{
  cuke::value v1(function{std::make_unique<chunk>(std::string("some name"))});
  EXPECT_EQ(v1.as<function>()->make_constant(std::string{"some value"}), 0);
  EXPECT_EQ(v1.as<function>()->make_constant(123), 1);
  cuke::value v2(v1);
  EXPECT_EQ(v1.type(), cuke::value_type::function);
  EXPECT_EQ(v2.type(), cuke::value_type::function);
  EXPECT_EQ(v2.as<function>()->size(), 0);
  EXPECT_EQ(v2.as<function>()->constants_count(), 2);
  EXPECT_EQ(v2.as<function>()->constant(0).as<std::string>(),
            std::string("some value"));
  EXPECT_EQ(v2.as<function>()->constant(1).as<int>(), 123);
}
TEST(value, assign_function_object)
{
  cuke::value v1(function{std::make_unique<chunk>(std::string("some name"))});
  EXPECT_EQ(v1.as<function>()->make_constant(std::string{"some value"}), 0);
  EXPECT_EQ(v1.as<function>()->make_constant(123), 1);
  cuke::value v2 = v1;
  EXPECT_EQ(v1.type(), cuke::value_type::function);
  EXPECT_EQ(v2.type(), cuke::value_type::function);
  EXPECT_EQ(v2.as<function>()->size(), 0);
  EXPECT_EQ(v2.as<function>()->constants_count(), 2);
  EXPECT_EQ(v2.as<function>()->constant(0).as<std::string>(),
            std::string("some value"));
  EXPECT_EQ(v2.as<function>()->constant(1).as<int>(), 123);
}
TEST(value, move_function_object)
{
  cuke::value v1(function{std::make_unique<chunk>(std::string("some name"))});
  EXPECT_EQ(v1.as<function>()->make_constant(std::string{"some value"}), 0);
  EXPECT_EQ(v1.as<function>()->make_constant(123), 1);
  cuke::value v2(std::move(v1));

  EXPECT_EQ(v1.type(), cuke::value_type::nil);
  EXPECT_EQ(v2.type(), cuke::value_type::function);
  EXPECT_EQ(v2.as<function>()->size(), 0);
  EXPECT_EQ(v2.as<function>()->constants_count(), 2);
  EXPECT_EQ(v2.as<function>()->constant(0).as<std::string>(),
            std::string("some value"));
  EXPECT_EQ(v2.as<function>()->constant(1).as<int>(), 123);
}

TEST(value, to_string_integral)
{
  cuke::value v(55);
  EXPECT_EQ(v.to_string(), std::string("55"));
}
TEST(value, to_string_floating)
{
  cuke::value v(55.55f);
  EXPECT_EQ(v.to_string(), std::to_string(55.55f));
}
TEST(value, to_string_double)
{
  cuke::value v(55.55);
  EXPECT_EQ(v.to_string(), std::to_string(55.55));
}
TEST(value, to_string_bool)
{
  cuke::value v(true);
  EXPECT_EQ(v.to_string(), std::string("1"));
}
TEST(value, to_string_string)
{
  cuke::value v(std::string("hello world"));
  EXPECT_EQ(v.to_string(), std::string("hello world"));
}