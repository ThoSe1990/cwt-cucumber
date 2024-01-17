#include <gtest/gtest.h>

#include "../src/cwt/compiler.hpp"
#include "../src/cwt/chunk.hpp"

using namespace cwt::details;

TEST(compiler, init_object) { compiler c(""); }

TEST(compiler, empty_script)
{
  testing::internal::CaptureStderr();
  compiler c("");
  function func = c.compile();
  EXPECT_TRUE(c.error());
  EXPECT_EQ(std::string("[line 1] Error at end: Expect FeatureLine\n"),
            testing::internal::GetCapturedStderr());
}
TEST(compiler, invalid_begin)
{
  testing::internal::CaptureStderr();
  compiler c("asdfsadf");
  function func = c.compile();
  EXPECT_TRUE(c.error());
  EXPECT_EQ(std::string("[line 1] Error  at 'asdfsadf': Expect FeatureLine\n"),
            testing::internal::GetCapturedStderr());
}

TEST(compiler, main_chunk_name)
{
  testing::internal::CaptureStderr();
  compiler c("Feature:");
  function main = c.compile();
  EXPECT_TRUE(c.error());
  EXPECT_EQ(main->name(), std::string{"script"});
  EXPECT_EQ(std::string("[line 1] Error at end: Expect ScenarioLine\n"),
            testing::internal::GetCapturedStderr());
}
TEST(compiler, main_chunk_other_language)
{
  testing::internal::CaptureStderr();
  const char* script = R"*(

# language: de

Funktion:
)*";
  compiler c(script);
  function main = c.compile();
  EXPECT_TRUE(c.error());
  EXPECT_EQ(main->name(), std::string{"script"});
  EXPECT_EQ(std::string("[line 6] Error at end: Expect ScenarioLine\n"),
            testing::internal::GetCapturedStderr());
}
TEST(compiler, main_chunk_ignore_linebreaks)
{
  testing::internal::CaptureStderr();
  const char* script = R"*(
  
  Feature:
)*";
  compiler c(script);
  function main = c.compile();
  EXPECT_TRUE(c.error());
  EXPECT_EQ(main->name(), std::string{"script"});
  EXPECT_EQ(std::string("[line 4] Error at end: Expect ScenarioLine\n"),
            testing::internal::GetCapturedStderr());
}
TEST(compiler, main_chunk_code)
{
  compiler c("Feature:");
  function main = c.compile();

  EXPECT_EQ(main->size(), 9);

  EXPECT_EQ(main->at(0), to_uint(op_code::constant));
  EXPECT_EQ(main->at(1), 0);
  EXPECT_EQ(main->at(2), to_uint(op_code::define_var));
  EXPECT_EQ(main->at(3), 1);
  EXPECT_EQ(main->at(4), to_uint(op_code::get_var));
  EXPECT_EQ(main->at(5), 1);
  EXPECT_EQ(main->at(6), to_uint(op_code::call));
  EXPECT_EQ(main->at(7), 0);
  EXPECT_EQ(main->at(8), to_uint(op_code::func_return));
}
TEST(compiler, main_chunk_constants)
{
  compiler c("Feature:");
  function main = c.compile();

  EXPECT_EQ(main->constants_count(), 2);
  EXPECT_EQ(main->constant(0).type(), value_type::function);
  EXPECT_EQ(main->constant(1).type(), value_type::string);
}
TEST(compiler, feature_chunk)
{
  compiler c("Feature:");
  function main = c.compile();
  const function& feature = main->constant(0).as<function>();
  EXPECT_EQ(feature->name(), std::string(":1"));
  EXPECT_EQ(feature->back(), to_uint(op_code::func_return));
}
TEST(compiler, regular_scenario)
{
  testing::internal::CaptureStderr();
  const char* script = R"*(
  Feature: A Fancy Feature
  Scenario: A Scenario)*";
  compiler c(script);
  function main = c.compile();
  EXPECT_EQ(main->name(), std::string{"script"});
  EXPECT_EQ(main->size(), 9);
  EXPECT_EQ(std::string("[line 3] Error at end: Expect StepLine\n"),
            testing::internal::GetCapturedStderr());
  EXPECT_TRUE(c.error());
}
TEST(compiler, feature_chunk_code)
{
  const char* script = R"*(
  Feature: A Fancy Feature
  Scenario: A Scenario
)*";
  compiler c(script);
  function main = c.compile();
  const function& feature = main->constant(0).as<function>();
  EXPECT_EQ(feature->size(), 30);

  EXPECT_EQ(feature->at(0), to_uint(op_code::constant));
  EXPECT_EQ(feature->at(1), 0);  // idx to string value to print
  EXPECT_EQ(feature->at(2), to_uint(op_code::print));
  EXPECT_EQ(feature->at(3), 0);  // TODO enum for color represents the color

  EXPECT_EQ(feature->at(4), to_uint(op_code::constant));
  EXPECT_EQ(feature->at(5), 1);  // idx to string value to print
  EXPECT_EQ(feature->at(6), to_uint(op_code::println));
  EXPECT_EQ(feature->at(7), 0);  // TODO enum for color represents the color

  EXPECT_EQ(feature->at(8), to_uint(op_code::constant));
  EXPECT_EQ(feature->at(9), 2);  // reset_context
  EXPECT_EQ(feature->at(10), to_uint(op_code::hook));
  EXPECT_EQ(feature->at(11), 0);  // tag count (hook)

  EXPECT_EQ(feature->at(12), to_uint(op_code::constant));
  EXPECT_EQ(feature->at(13), 3);  // before
  EXPECT_EQ(feature->at(14), to_uint(op_code::hook));
  EXPECT_EQ(feature->at(15), 0);  // tag count (hook)

  EXPECT_EQ(feature->at(16), to_uint(op_code::constant));
  EXPECT_EQ(feature->at(17), 4);  // function: scenario call
  EXPECT_EQ(feature->at(18), to_uint(op_code::define_var));
  EXPECT_EQ(feature->at(19), 5);
  EXPECT_EQ(feature->at(20), to_uint(op_code::get_var));
  EXPECT_EQ(feature->at(21), 5);
  EXPECT_EQ(feature->at(22), to_uint(op_code::call));
  EXPECT_EQ(feature->at(23), 0);

  EXPECT_EQ(feature->at(24), to_uint(op_code::constant));
  EXPECT_EQ(feature->at(25), 6);  // after
  EXPECT_EQ(feature->at(26), to_uint(op_code::hook));
  EXPECT_EQ(feature->at(27), 0);  // after

  EXPECT_EQ(feature->at(28), to_uint(op_code::scenario_result));
  EXPECT_EQ(feature->at(29), to_uint(op_code::func_return));
}

TEST(compiler, feature_constants)
{
  const char* script = R"*(
  Feature: A Fancy Feature
  Scenario: A Scenario
  Given Any Step
)*";
  compiler c(script);
  function main = c.compile();
  const function& feature = main->constant(0).as<function>();
  ASSERT_EQ(feature->constants_count(), 7);
  EXPECT_EQ(feature->name(), ":2");
  EXPECT_EQ(feature->constant(0).type(), value_type::string);
  EXPECT_EQ(feature->constant(0).as<std::string>(), "Feature: A Fancy Feature");
  EXPECT_EQ(feature->constant(1).type(), value_type::string);
  EXPECT_EQ(feature->constant(1).as<std::string>(), ":2");
  EXPECT_EQ(feature->constant(2).type(), value_type::string);
  EXPECT_EQ(feature->constant(2).as<std::string>(), "reset_context");
  EXPECT_EQ(feature->constant(3).type(), value_type::string);
  EXPECT_EQ(feature->constant(3).as<std::string>(), "before");
  EXPECT_EQ(feature->constant(4).type(), value_type::function);
  EXPECT_EQ(feature->constant(4).as<function>()->name(), ":3");
  EXPECT_EQ(feature->constant(5).type(), value_type::string);
  EXPECT_EQ(feature->constant(5).as<std::string>(), ":3");
  EXPECT_EQ(feature->constant(6).type(), value_type::string);
  EXPECT_EQ(feature->constant(6).as<std::string>(), "after");

  EXPECT_TRUE(c.no_error());
}

TEST(compiler, scenario_chunk_code)
{
  const char* script = R"*(
  Feature: A Fancy Feature
  Scenario: A Scenario
  Given Any Step
)*";
  compiler c(script);
  function main = c.compile();
  const function& feature = main->constant(0).as<function>();
  const function& scenario = feature->constant(4).as<function>();

  EXPECT_TRUE(c.no_error());
  EXPECT_EQ(scenario->name(), ":3");
  ASSERT_EQ(scenario->size(), 32);
  ASSERT_EQ(scenario->constants_count(), 8);

  // EXPECT_EQ(scenario->at(0), to_uint(op_code::constant));
  // EXPECT_EQ(scenario->at(1), 0);
  // EXPECT_EQ(scenario->at(2), to_uint(op_code::print));
  // EXPECT_EQ(scenario->at(3), 0);  // TODO color
  // EXPECT_EQ(scenario->at(4), to_uint(op_code::constant));
  // EXPECT_EQ(scenario->at(5), 1);
  // EXPECT_EQ(scenario->at(6), to_uint(op_code::println));
  // EXPECT_EQ(scenario->at(7), 0);  // TODO color

  // EXPECT_EQ(scenario->at(8), to_uint(op_code::init_scenario));

  // EXPECT_EQ(scenario->at(9), to_uint(op_code::jump_if_failed));
  // EXPECT_EQ(scenario->at(10), 21);

  // EXPECT_EQ(scenario->at(11), to_uint(op_code::constant));
  // EXPECT_EQ(scenario->at(12), 2);  // before_step
  // EXPECT_EQ(scenario->at(13), to_uint(op_code::hook));
  // EXPECT_EQ(scenario->at(14), 0);  // tag count (hook)

  // EXPECT_EQ(scenario->at(15), to_uint(op_code::call_step));
  // EXPECT_EQ(scenario->at(16), 3);  // step string

  // EXPECT_EQ(scenario->at(17), to_uint(op_code::constant));
  // EXPECT_EQ(scenario->at(18), 4);  // after_step
  // EXPECT_EQ(scenario->at(19), to_uint(op_code::hook));
  // EXPECT_EQ(scenario->at(20), 0);  // tag count (hook)

  // // TODO we'll see what we need here ...
  // EXPECT_EQ(scenario->at(21), to_uint(op_code::step_result));
  // EXPECT_EQ(scenario->at(22), 5);

  // EXPECT_EQ(scenario->at(23), to_uint(op_code::func_return));
}

TEST(compiler, scenario_chunk_constants)
{
  const char* script = R"*(
  Feature: A Fancy Feature
  Scenario: A Scenario
  Given Any Step
)*";
  compiler c(script);
  function main = c.compile();
  const function& feature = main->constant(0).as<function>();
  const function& scenario = feature->constant(4).as<function>();
  ASSERT_EQ(scenario->constants_count(), 8);
  ASSERT_EQ(scenario->name(), ":3");
  // EXPECT_EQ(scenario->constant(0).type(), value_type::string);
  // EXPECT_EQ(scenario->constant(0).as<std::string>(), "Scenario: A Scenario");
  // EXPECT_EQ(scenario->constant(1).type(), value_type::string);
  // EXPECT_EQ(scenario->constant(1).as<std::string>(), ":3");
  // EXPECT_EQ(scenario->constant(2).type(), value_type::string);
  // EXPECT_EQ(scenario->constant(2).as<std::string>(), "before_step");
  // EXPECT_EQ(scenario->constant(3).type(), value_type::string);
  // EXPECT_EQ(scenario->constant(3).as<std::string>(), "TODO string step name");
  // EXPECT_EQ(scenario->constant(4).type(), value_type::string);
  // EXPECT_EQ(scenario->constant(4).as<std::string>(), "after_step");
  // EXPECT_EQ(scenario->constant(5).type(), value_type::string);
  // EXPECT_EQ(scenario->constant(5).as<std::string>(), "TODO result here");
}