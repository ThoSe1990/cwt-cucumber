#include <gtest/gtest.h>

#include "../src/cwt/value.hpp"
#include "../src/cwt/chunk.hpp"

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
  value v(function{"some name", std::make_unique<chunk>()});
  EXPECT_EQ(v.type(), value_type::function);
  EXPECT_EQ(v.as<function>().name, "some name");
}
TEST(value, native_value)
{
  auto test_func = [](const value_array& arr)
  {
    ASSERT_EQ(arr.size(), 1);
    EXPECT_EQ(arr.at(0).as<int>(), 99);
  };
  value v(native{test_func});
  value_array arr;
  arr.emplace_back(99);
  v.as<native>().func(arr);
  EXPECT_EQ(v.type(), value_type::native);
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
TEST(value, copy_integer)
{
  value v1{123};
  value v2(v1);
  EXPECT_EQ(v2.as<int>(), v1.as<int>());
  EXPECT_EQ(v2.as<int>(), 123);
  EXPECT_EQ(v2.type(), value_type::integral);
}
TEST(value, copy_double)
{
  value v1{2.2};
  value v2(v1);
  EXPECT_EQ(v2.as<double>(), v1.as<double>());
  EXPECT_EQ(v2.as<double>(), 2.2);
  EXPECT_EQ(v2.type(), value_type::floating);
}
TEST(value, copy_boolean)
{
  value v1{true};
  value v2(v1);
  EXPECT_EQ(v2.as<bool>(), v1.as<bool>());
  EXPECT_EQ(v2.as<bool>(), true);
  EXPECT_EQ(v2.type(), value_type::boolean);
}
TEST(value, copy_string)
{
  value v1{std::string("hello copy")};
  value v2(v1);
  EXPECT_EQ(v2.as<std::string>(), v1.as<std::string>());
  EXPECT_EQ(v2.as<std::string>(), std::string("hello copy"));
  EXPECT_EQ(v2.type(), value_type::string);
}
TEST(value, copy_function)
{
  value v1(function{"some name", std::make_unique<chunk>()});
  v1.as<function>().chunk_data->emplace_constant(0, std::string{"some value"});
  v1.as<function>().chunk_data->emplace_constant(0, 123);
  value v2(v1);
  EXPECT_EQ(v2.type(), value_type::function);
  EXPECT_EQ(v2.type(), v1.type());
  EXPECT_EQ(v2.as<function>().chunk_data->size(), 4);
  EXPECT_EQ(v2.as<function>().chunk_data->constants_count(), 2);
  EXPECT_EQ(v2.as<function>().chunk_data->constant(0).as<std::string>(),
            std::string("some value"));
  EXPECT_EQ(v2.as<function>().chunk_data->constant(1).as<int>(), 123);
}
TEST(value, copy_native)
{
  value_array arr;
  auto test_func = [](const value_array& arr)
  { throw std::runtime_error("only in this test..."); };
  value v1(native{test_func});
  value v2(v1);

  EXPECT_EQ(v2.type(), value_type::native);
  EXPECT_EQ(v2.type(), v1.type());
  EXPECT_THROW(v1.as<native>().func(arr), std::runtime_error);
  EXPECT_THROW(v2.as<native>().func(arr), std::runtime_error);
}