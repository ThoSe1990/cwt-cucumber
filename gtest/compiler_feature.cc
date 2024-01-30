#include <gtest/gtest.h>

#include "../src/cwt/compiler/feature.hpp"

using namespace cwt::details;

TEST(compiler_feature, chunk_size_wo_scenario)
{
  testing::internal::CaptureStderr();
  compiler::cucumber cuke("Feature: Hello World");
  compiler::feature f(&cuke);
  f.compile();
  EXPECT_EQ(f.get_chunk().size(), 8);
  EXPECT_EQ(std::string("[line 1] Error at end: Expect ScenarioLine\n"),
            testing::internal::GetCapturedStderr());
}
TEST(compiler_feature, chunk_size)
{
const char* script = R"*(
  Feature: Hello World
  Scenario: A Scenario
)*";
  testing::internal::CaptureStderr();
  compiler::cucumber cuke(script);
  compiler::feature f(&cuke);
  f.compile();
  EXPECT_EQ(f.get_chunk().size(), 22);
  EXPECT_EQ(std::string("[line 4] Error at end: Expect StepLine\n"),
            testing::internal::GetCapturedStderr());
}
TEST(compiler_feature, feature_chunk_code)
{
const char* script = R"*(
  Feature: Hello World
  Scenario: A Scenario
)*";

  compiler::cucumber cuke(script);
  compiler::feature f(&cuke);
  f.compile();

  EXPECT_EQ(f.get_chunk().at(0), to_uint(op_code::constant));
  EXPECT_EQ(f.get_chunk().at(1), 1);  // idx to string value to print
  EXPECT_EQ(f.get_chunk().at(2), to_uint(op_code::print));
  EXPECT_EQ(f.get_chunk().at(3), to_uint(color::standard));  // TODO enum for color represents the color

  EXPECT_EQ(f.get_chunk().at(4), to_uint(op_code::constant));
  EXPECT_EQ(f.get_chunk().at(5), 0);  // idx to string value to print
  EXPECT_EQ(f.get_chunk().at(6), to_uint(op_code::println));
  EXPECT_EQ(f.get_chunk().at(7), to_uint(color::black));  // TODO enum for color represents the color

  EXPECT_EQ(f.get_chunk().at(8), to_uint(op_code::reset_context));
  EXPECT_EQ(f.get_chunk().at(9), to_uint(op_code::hook_before));
  EXPECT_EQ(f.get_chunk().at(10), 0);  // tag count (hook)

  EXPECT_EQ(f.get_chunk().at(11), to_uint(op_code::constant));
  EXPECT_EQ(f.get_chunk().at(12), 2);  // function: scenario call
  EXPECT_EQ(f.get_chunk().at(13), to_uint(op_code::define_var));
  EXPECT_EQ(f.get_chunk().at(14), 3);
  EXPECT_EQ(f.get_chunk().at(15), to_uint(op_code::get_var));
  EXPECT_EQ(f.get_chunk().at(16), 3);
  EXPECT_EQ(f.get_chunk().at(17), to_uint(op_code::call));
  EXPECT_EQ(f.get_chunk().at(18), 0);

  EXPECT_EQ(f.get_chunk().at(19), to_uint(op_code::hook_after));
  EXPECT_EQ(f.get_chunk().at(20), 0);  // tag count (hook)

  EXPECT_EQ(f.get_chunk().at(21), to_uint(op_code::scenario_result));
}

TEST(compiler_feature, feature_constants)
{
const char* script = R"*(
  Feature: Hello World
  Scenario: A Scenario
)*";
  compiler::cucumber cuke(script);
  compiler::feature f(&cuke);
  f.compile();

  ASSERT_EQ(f.get_chunk().constants_count(), 4);
  EXPECT_EQ(f.get_chunk().name(), ":2");
  EXPECT_EQ(f.get_chunk().constant(0).type(), value_type::string);
  EXPECT_EQ(f.get_chunk().constant(0).as<std::string>(), ":2");
  EXPECT_EQ(f.get_chunk().constant(1).type(), value_type::string);
  EXPECT_EQ(f.get_chunk().constant(1).as<std::string>(), "Feature: Hello World");
  EXPECT_EQ(f.get_chunk().constant(2).type(), value_type::function);
  EXPECT_EQ(f.get_chunk().constant(2).as<function>()->name(), ":3");
  EXPECT_EQ(f.get_chunk().constant(3).type(), value_type::string);
  EXPECT_EQ(f.get_chunk().constant(3).as<std::string>(), ":3");
}

