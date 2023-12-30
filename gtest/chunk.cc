#include <gtest/gtest.h>

#include "../src/cwt/chunk.hpp"

using namespace cwt::details;

TEST(chunk, init_obj)
{
  chunk c;
  EXPECT_EQ(c.size(), 0);
}

