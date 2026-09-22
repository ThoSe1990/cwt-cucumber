#include <gtest/gtest.h>

#include "../src/test_runner.hpp"
#include "../src/parser.hpp"
#include "../src/asserts.hpp"
#include "../src/test_results.hpp"
#include "../src/options.hpp"

namespace
{
const char* one_scenario = R"*(
  Feature: a feature
  Scenario: a scenario
  Given a step
)*";

void run(const char* script)
{
  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);
}
}  // namespace

class hook_results : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    cuke::registry().clear();
    cuke::results::test_results().clear();
    cuke::registry().push_step(cuke::internal::step_definition(
        [](const cuke::value_array&, const auto&, const auto&, const auto&) {},
        "a step"));
  }
  void TearDown() override { cuke::internal::get_program_args(0, {}).clear(); }
};

// A BEFORE hook runs before any step exists, so the failure sink has no step
// to write to. It must not read an empty vector.
TEST_F(hook_results, a_failing_before_hook_does_not_crash)
{
  cuke::registry().push_hook_before(
      cuke::internal::hook([]() { cuke::equal(1, 2); }));
  run(one_scenario);
  SUCCEED();
}

// A hook that fails is recorded against the hook, not against a step.
TEST_F(hook_results, a_failing_before_hook_is_recorded)
{
  cuke::registry().push_hook_before(
      cuke::internal::hook([]() { cuke::equal(1, 2); }));
  run(one_scenario);

  const auto& scenario =
      cuke::results::test_results().data().at(0).scenarios.at(0);
  ASSERT_EQ(scenario.before.size(), 1);
  EXPECT_EQ(scenario.before.at(0).status, cuke::results::test_status::failed);
  EXPECT_FALSE(scenario.before.at(0).error_msg.empty());
}

TEST_F(hook_results, a_failing_after_hook_is_not_charged_to_the_last_step)
{
  cuke::registry().push_hook_after(
      cuke::internal::hook([]() { cuke::equal(1, 2); }));
  run(one_scenario);

  const auto& scenario =
      cuke::results::test_results().data().at(0).scenarios.at(0);
  ASSERT_EQ(scenario.after.size(), 1);
  EXPECT_EQ(scenario.after.at(0).status, cuke::results::test_status::failed);
  ASSERT_EQ(scenario.steps.size(), 1);
  EXPECT_EQ(scenario.steps.at(0).status, cuke::results::test_status::passed);
  EXPECT_TRUE(scenario.steps.at(0).error_msg.empty());
}

TEST_F(hook_results, a_passing_hook_is_recorded_as_passed)
{
  cuke::registry().push_hook_before(cuke::internal::hook([]() {}));
  cuke::registry().push_hook_after(cuke::internal::hook([]() {}));
  run(one_scenario);

  const auto& scenario =
      cuke::results::test_results().data().at(0).scenarios.at(0);
  ASSERT_EQ(scenario.before.size(), 1);
  ASSERT_EQ(scenario.after.size(), 1);
  EXPECT_EQ(scenario.before.at(0).status, cuke::results::test_status::passed);
  EXPECT_EQ(scenario.after.at(0).status, cuke::results::test_status::passed);
}

TEST_F(hook_results, a_failing_hook_fails_the_scenario)
{
  cuke::registry().push_hook_before(
      cuke::internal::hook([]() { cuke::equal(1, 2); }));
  run(one_scenario);

  EXPECT_EQ(cuke::results::test_results().scenarios_failed(), 1);
  EXPECT_EQ(cuke::results::final_result(), cuke::results::test_status::failed);
}

// A scenario with no @Before/@After hooks registered at all must not report
// an execution that never happened.
TEST_F(hook_results, no_hooks_registered_leaves_both_arrays_empty)
{
  run(one_scenario);

  const auto& scenario =
      cuke::results::test_results().data().at(0).scenarios.at(0);
  EXPECT_TRUE(scenario.before.empty());
  EXPECT_TRUE(scenario.after.empty());
}

// Two failing BEFORE hooks must each get their own record with their own
// message, not one record that the second hook overwrites.
TEST_F(hook_results, two_failing_before_hooks_each_carry_their_own_message)
{
  cuke::registry().push_hook_before(cuke::internal::hook(
      []() { cuke::equal(1, 2, std::string("first hook failed")); }));
  cuke::registry().push_hook_before(cuke::internal::hook(
      []() { cuke::equal(1, 2, std::string("second hook failed")); }));
  run(one_scenario);

  const auto& scenario =
      cuke::results::test_results().data().at(0).scenarios.at(0);
  ASSERT_EQ(scenario.before.size(), 2);
  EXPECT_EQ(scenario.before.at(0).status, cuke::results::test_status::failed);
  EXPECT_EQ(scenario.before.at(0).error_msg, "first hook failed");
  EXPECT_EQ(scenario.before.at(1).status, cuke::results::test_status::failed);
  EXPECT_EQ(scenario.before.at(1).error_msg, "second hook failed");
}

// A hook whose tag expression does not match the scenario is not run, so it
// must not get a record either.
TEST_F(hook_results, a_hook_with_a_nonmatching_tag_gets_no_record)
{
  cuke::registry().push_hook_before(cuke::internal::hook([]() {}, "@nope"));
  run(one_scenario);

  const auto& scenario =
      cuke::results::test_results().data().at(0).scenarios.at(0);
  EXPECT_TRUE(scenario.before.empty());
}

// A BEFORE_ALL hook runs before any feature or scenario exists, so
// fail_current has neither a hook cursor nor a current step to write to.
// Before this, the failure was silently dropped and the run exited 0; it
// must instead be a run-level failure that final_result() honours.
TEST_F(hook_results, a_failing_before_all_hook_fails_the_run)
{
  cuke::registry().push_hook_before_all(
      cuke::internal::hook([]() { cuke::equal(1, 2); }));

  cuke::test_runner runner;
  runner.setup();

  EXPECT_EQ(cuke::results::final_result(), cuke::results::test_status::failed);
}

TEST_F(hook_results, clearing_results_clears_a_run_level_hook_failure)
{
  cuke::registry().push_hook_before_all(
      cuke::internal::hook([]() { cuke::equal(1, 2); }));

  cuke::test_runner runner;
  runner.setup();
  ASSERT_EQ(cuke::results::final_result(), cuke::results::test_status::failed);

  cuke::results::test_results().clear();
  EXPECT_EQ(cuke::results::final_result(), cuke::results::test_status::passed);
}

// --dry-run skips only steps; hooks still run. A failing AFTER hook on a
// dry-run scenario must fail that scenario, otherwise the terminal and the
// JSON report disagree about it again - the disagreement this branch exists
// to remove.
TEST_F(hook_results, a_failing_after_hook_fails_a_dry_run_scenario)
{
  cuke::registry().push_hook_after(
      cuke::internal::hook([]() { cuke::equal(1, 2); }));

  const char* argv[] = {"program", "--dry-run"};
  int argc = sizeof(argv) / sizeof(argv[0]);
  [[maybe_unused]] auto& args = cuke::internal::get_program_args(argc, argv);

  run(one_scenario);

  const auto& scenario =
      cuke::results::test_results().data().at(0).scenarios.at(0);
  EXPECT_EQ(scenario.status, cuke::results::test_status::failed);
}

