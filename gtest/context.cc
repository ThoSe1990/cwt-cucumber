#include <gtest/gtest.h>

#include "../src/cwt/context.hpp"

TEST(context, init_string)
{
  cuke::context<std::string>("hello world");
  EXPECT_EQ(cuke::context<std::string>(), std::string("hello world"));
}
