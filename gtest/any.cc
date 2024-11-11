
#include <gtest/gtest.h>

#include "../src/any.hpp"

using namespace cuke::internal;

TEST(any, init_obj) { any a(234); }
TEST(any, init_int)
{
  any a(234);
  int i = a;
  EXPECT_EQ(i, 234);
}
TEST(any, assign_int)
{
  any a(234);
  int i = 99;
  i = a;
  EXPECT_EQ(i, 234);
}

struct foo
{
  std::string value;
};
TEST(any, init_string_in_struct)
{
  any a(std::string("hello"));
  foo f{a};
  EXPECT_EQ(f.value, std::string("hello"));
}
