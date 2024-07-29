#include <iterator>
#include <gtest/gtest.h>

#include "../src/chunk.hpp"

using namespace cuke::internal;

void add_3_bytes(chunk& c)
{
  c.push_byte(op_code::constant, 1);
  c.push_byte(op_code::constant, 2);
  c.push_byte(op_code::constant, 3);
}

TEST(chunk, init_obj)
{
  chunk c;
  EXPECT_EQ(c.size(), 0);
}

TEST(chunk, size)
{
  chunk c;
  add_3_bytes(c);
  EXPECT_EQ(c.size(), 3);
}

TEST(chunk, iterator_1)
{
  chunk c;
  add_3_bytes(c);
  std::size_t count = 0;
  for (auto it = c.cbegin(); it != c.cend(); ++it)
  {
    ++count;
  }
  EXPECT_EQ(count, 3);
}
TEST(chunk, iterator_2)
{
  chunk c;
  add_3_bytes(c);
  std::size_t count = 0;
  for (auto it = c.cbegin(); it != c.cend(); it = std::next(it))
  {
    ++count;
  }
  EXPECT_EQ(count, 3);
}

TEST(chunk, byte_access_out_of_range)
{
  chunk c;
  EXPECT_THROW({ auto val = c[1]; }, std::out_of_range);
}
TEST(chunk, constant_access_out_of_range)
{
  chunk c;
  EXPECT_THROW({ const auto& val = c.constant(1); }, std::out_of_range);
}
TEST(chunk, add_constant_integer)
{
  chunk c;
  EXPECT_EQ(c.make_constant(123), 0);
  EXPECT_EQ(c.size(), 0);
  EXPECT_EQ(c.constants_count(), 1);
  EXPECT_EQ(c.constant(0).as<unsigned int>(), 123);
}

TEST(chunk, add_constant_string)
{
  chunk c;
  EXPECT_EQ(c.make_constant(std::string("hello world")), 0);
  EXPECT_EQ(c.constant(0).as<std::string>(), std::string("hello world"));
}

TEST(chunk, add_constant_string_view)
{
  chunk c;
  EXPECT_EQ(c.make_constant(std::string_view("hello world")), 0);
  EXPECT_EQ(c.constant(0).as<std::string_view>(), std::string_view("hello world"));
}
