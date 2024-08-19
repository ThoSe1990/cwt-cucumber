
#include <gtest/gtest.h>

#include "../src/registry.hpp"

class registry_tests : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    cuke::registry().clear();
    EXPECT_EQ(cuke::registry().steps().size(), 0);
    EXPECT_EQ(cuke::registry().hooks_before().size(), 0);
    EXPECT_EQ(cuke::registry().hooks_after().size(), 0);
    EXPECT_EQ(cuke::registry().hooks_before_step().size(), 0);
    EXPECT_EQ(cuke::registry().hooks_after_step().size(), 0);
  }
  void TearDown() override
  {
    cuke::registry().clear();
    EXPECT_EQ(cuke::registry().steps().size(), 0);
    EXPECT_EQ(cuke::registry().hooks_before().size(), 0);
    EXPECT_EQ(cuke::registry().hooks_after().size(), 0);
    EXPECT_EQ(cuke::registry().hooks_before_step().size(), 0);
    EXPECT_EQ(cuke::registry().hooks_after_step().size(), 0);
  }
};

TEST_F(registry_tests, steps)
{
  cuke::registry().push_step(
      cuke::internal::step([](const cuke::value_array&) {}, "a step"));
  EXPECT_EQ(cuke::registry().steps().size(), 1);
}

TEST_F(registry_tests, scenario_hook_before)
{
  cuke::registry().push_hook_before(cuke::internal::hook([]() {}, ""));
  EXPECT_EQ(cuke::registry().hooks_before().size(), 1);
}
TEST_F(registry_tests, scenario_hook_after)
{
  cuke::registry().push_hook_after(cuke::internal::hook([]() {}, ""));
  EXPECT_EQ(cuke::registry().hooks_after().size(), 1);
}
TEST_F(registry_tests, scenario_hook_before_tag)
{
  cuke::registry().push_hook_before(cuke::internal::hook([]() {}, "@tag1"));
  EXPECT_EQ(cuke::registry().hooks_before().size(), 1);
  EXPECT_TRUE(cuke::registry().hooks_before().at(0).valid_tag(
      std::vector<std::string>{std::string("@tag1")}));
}
TEST_F(registry_tests, scenario_hook_after_tag)
{
  cuke::registry().push_hook_after(cuke::internal::hook([]() {}, "@tag1"));
  EXPECT_EQ(cuke::registry().hooks_after().size(), 1);
  EXPECT_TRUE(cuke::registry().hooks_after().at(0).valid_tag(
      std::vector<std::string>{std::string("@tag1")}));
}
TEST_F(registry_tests, step_hook_before)
{
  cuke::registry().push_hook_before_step(cuke::internal::hook([]() {}));
  EXPECT_EQ(cuke::registry().hooks_before_step().size(), 1);
}
TEST_F(registry_tests, step_hook_after)
{
  cuke::registry().push_hook_after_step(cuke::internal::hook([]() {}));
  EXPECT_EQ(cuke::registry().hooks_after_step().size(), 1);
}
