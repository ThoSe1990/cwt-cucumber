#include <iterator>
#include <gtest/gtest.h>

#include "../src/cwt/chunk.hpp"

using namespace cwt::details;

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
  for (auto it = c.begin(); it != c.end(); ++it)
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
  for (auto it = c.begin(); it != c.end(); it = std::next(it))
  {
    ++count;
  }
  EXPECT_EQ(count, 3);
}
