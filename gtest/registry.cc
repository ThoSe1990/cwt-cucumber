
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
  cuke::registry().push_step(cuke::internal::step_definition(
      [](const cuke::value_array&, const auto&, const auto&, const auto&) {},
      "a step"));
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
namespace
{
bool g_before_hook_called = false;
bool g_after_hook_called = false;
}  // namespace

// A downstream caller compiled against the pre-callback API only ever
// passed the tags. Both single-argument overloads must keep compiling and
// still run every hook whose tag expression matches.
TEST_F(registry_tests, run_hook_before_with_only_tags_still_compiles)
{
  g_before_hook_called = false;
  cuke::registry().push_hook_before(
      cuke::internal::hook([]() { g_before_hook_called = true; }, ""));
  cuke::registry().run_hook_before(std::vector<std::string>{});
  EXPECT_TRUE(g_before_hook_called);
}
TEST_F(registry_tests, run_hook_after_with_only_tags_still_compiles)
{
  g_after_hook_called = false;
  cuke::registry().push_hook_after(
      cuke::internal::hook([]() { g_after_hook_called = true; }, ""));
  cuke::registry().run_hook_after(std::vector<std::string>{});
  EXPECT_TRUE(g_after_hook_called);
}

// main's single-argument run_hook_before/run_hook_after are `const
// noexcept`. Since C++17 noexcept is part of the function type, so a
// downstream caller that takes &registry::run_hook_before through that
// exact pointer-to-member type still fails to compile if the restored
// overload is only `const`. Pin the type, not just callability.
TEST_F(registry_tests, run_hook_before_after_keep_the_noexcept_pointer_type)
{
  using hook_ptr_t =
      void (cuke::internal::registry::*)(const std::vector<std::string>&)
          const noexcept;
  [[maybe_unused]] hook_ptr_t before_ptr =
      &cuke::internal::registry::run_hook_before;
  [[maybe_unused]] hook_ptr_t after_ptr =
      &cuke::internal::registry::run_hook_after;
  SUCCEED();
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
TEST_F(registry_tests, default_order)
{
  using namespace cuke::internal;
  cuke::registry().push_step(step_definition(
      [](const cuke::value_array&, const auto&, const auto&, const auto&) {},
      "a step"));
  cuke::registry().push_step(step_definition(
      [](const cuke::value_array&, const auto&, const auto&, const auto&) {},
      "another step"));
  cuke::registry().push_step(step_definition(
      [](const cuke::value_array&, const auto&, const auto&, const auto&) {},
      "a given step", step_definition::type::given));

  ASSERT_EQ(cuke::registry().steps().size(), 3);

  EXPECT_EQ(cuke::registry().steps().at(0).step_type(),
            step_definition::type::step);
  EXPECT_EQ(cuke::registry().steps().at(1).step_type(),
            step_definition::type::step);
  EXPECT_EQ(cuke::registry().steps().at(2).step_type(),
            step_definition::type::given);
}
TEST_F(registry_tests, sort_by_type_1)
{
  using namespace cuke::internal;
  cuke::registry().push_step(step_definition(
      [](const cuke::value_array&, const auto&, const auto&, const auto&) {},
      "a step"));
  cuke::registry().push_step(step_definition(
      [](const cuke::value_array&, const auto&, const auto&, const auto&) {},
      "another step"));
  cuke::registry().push_step(step_definition(
      [](const cuke::value_array&, const auto&, const auto&, const auto&) {},
      "a given step", step_definition::type::given));

  ASSERT_EQ(cuke::registry().steps().size(), 3);

  cuke::registry().sort_steps_by_type();

  EXPECT_EQ(cuke::registry().steps().at(0).step_type(),
            step_definition::type::given);
  EXPECT_EQ(cuke::registry().steps().at(1).step_type(),
            step_definition::type::step);
  EXPECT_EQ(cuke::registry().steps().at(2).step_type(),
            step_definition::type::step);
}
TEST_F(registry_tests, sort_by_type_2)
{
  using namespace cuke::internal;
  cuke::registry().push_step(step_definition(
      [](const cuke::value_array&, const auto&, const auto&, const auto&) {},
      "a", step_definition::type::when));
  cuke::registry().push_step(step_definition(
      [](const cuke::value_array&, const auto&, const auto&, const auto&) {},
      "b", step_definition::type::then));
  cuke::registry().push_step(step_definition(
      [](const cuke::value_array&, const auto&, const auto&, const auto&) {},
      "c", step_definition::type::given));
  cuke::registry().push_step(step_definition(
      [](const cuke::value_array&, const auto&, const auto&, const auto&) {},
      "d", step_definition::type::given));
  cuke::registry().push_step(step_definition(
      [](const cuke::value_array&, const auto&, const auto&, const auto&) {},
      "e", step_definition::type::then));
  cuke::registry().push_step(step_definition(
      [](const cuke::value_array&, const auto&, const auto&, const auto&) {},
      "f", step_definition::type::when));
  cuke::registry().push_step(step_definition(
      [](const cuke::value_array&, const auto&, const auto&, const auto&) {},
      "g", step_definition::type::when));
  cuke::registry().push_step(step_definition(
      [](const cuke::value_array&, const auto&, const auto&, const auto&) {},
      "h", step_definition::type::step));
  cuke::registry().push_step(step_definition(
      [](const cuke::value_array&, const auto&, const auto&, const auto&) {},
      "i", step_definition::type::when));

  ASSERT_EQ(cuke::registry().steps().size(), 9);

  cuke::registry().sort_steps_by_type();

  EXPECT_EQ(cuke::registry().steps().at(0).step_type(),
            step_definition::type::given);
  EXPECT_EQ(cuke::registry().steps().at(1).step_type(),
            step_definition::type::given);
  EXPECT_EQ(cuke::registry().steps().at(2).step_type(),
            step_definition::type::when);
  EXPECT_EQ(cuke::registry().steps().at(3).step_type(),
            step_definition::type::when);
  EXPECT_EQ(cuke::registry().steps().at(4).step_type(),
            step_definition::type::when);
  EXPECT_EQ(cuke::registry().steps().at(5).step_type(),
            step_definition::type::when);
  EXPECT_EQ(cuke::registry().steps().at(6).step_type(),
            step_definition::type::then);
  EXPECT_EQ(cuke::registry().steps().at(7).step_type(),
            step_definition::type::then);
  EXPECT_EQ(cuke::registry().steps().at(8).step_type(),
            step_definition::type::step);
}
