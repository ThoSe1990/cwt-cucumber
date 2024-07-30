
#include <gtest/gtest.h>

#include "../src/ast/registry.hpp"

TEST(registry, steps)
{
  cuke::registry::clear();
  EXPECT_EQ(cuke::registry::steps().size(), 0); 
 
  cuke::registry::push_step(
      cuke::internal::step([](const cuke::value_array&) {}, "a step"));
  EXPECT_EQ(cuke::registry::steps().size(), 1);

  cuke::registry::clear();
  EXPECT_EQ(cuke::registry::steps().size(), 0);
}
