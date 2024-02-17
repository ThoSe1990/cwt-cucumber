#include <gtest/gtest.h>

#include "../src/compiler/feature.hpp"

#define PRINT_STACK 1
#include "../src/debug.hpp"

using namespace cwt::details;

TEST(compiler_feature, chunk_size_wo_scenario)
{
  testing::internal::CaptureStderr();
  compiler::cucumber cuke("Feature: Hello World");
  compiler::feature f(&cuke);
  f.compile();
  EXPECT_EQ(f.get_chunk().size(), 10);
  EXPECT_EQ(std::string("[line 1] Error at end: Expect ScenarioLine\n"),
            testing::internal::GetCapturedStderr());

  disassemble_chunk(f.get_chunk(), "feature");
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
  EXPECT_EQ(f.get_chunk().size(), 23);
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

  std::size_t i = 0;

  EXPECT_EQ(f.get_chunk().at(i++), to_uint(op_code::print_linebreak));
  EXPECT_EQ(f.get_chunk().at(i++), to_uint(op_code::constant));
  EXPECT_EQ(f.get_chunk().at(i++), 1);  // idx to string value to print
  EXPECT_EQ(f.get_chunk().at(i++), to_uint(op_code::print));
  EXPECT_EQ(f.get_chunk().at(i++), to_uint(color::standard));
  EXPECT_EQ(f.get_chunk().at(i++), to_uint(op_code::print_indent));
  EXPECT_EQ(f.get_chunk().at(i++), to_uint(op_code::constant));
  EXPECT_EQ(f.get_chunk().at(i++), 0);  // idx to string value to print
  EXPECT_EQ(f.get_chunk().at(i++), to_uint(op_code::println));
  EXPECT_EQ(f.get_chunk().at(i++), to_uint(color::black));

  EXPECT_EQ(f.get_chunk().at(i++), to_uint(op_code::reset_context));
  EXPECT_EQ(f.get_chunk().at(i++), to_uint(op_code::hook_before));
  EXPECT_EQ(f.get_chunk().at(i++), 0);  // tag count (hook)

  EXPECT_EQ(f.get_chunk().at(i++), to_uint(op_code::constant));
  EXPECT_EQ(f.get_chunk().at(i++), 2);  // function: scenario call
  EXPECT_EQ(f.get_chunk().at(i++), to_uint(op_code::define_var));
  EXPECT_EQ(f.get_chunk().at(i++), 3);
  EXPECT_EQ(f.get_chunk().at(i++), to_uint(op_code::get_var));
  EXPECT_EQ(f.get_chunk().at(i++), 3);
  EXPECT_EQ(f.get_chunk().at(i++), to_uint(op_code::call));
  EXPECT_EQ(f.get_chunk().at(i++), 0);

  EXPECT_EQ(f.get_chunk().at(i++), to_uint(op_code::hook_after));
  EXPECT_EQ(f.get_chunk().at(i++), 0);  // tag count (hook)
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
  EXPECT_EQ(f.get_chunk().name(), "line:2");
  EXPECT_EQ(f.get_chunk().constant(0).type(), cuke::value_type::string);
  EXPECT_EQ(f.get_chunk().constant(0).as<std::string>(), "line:2");
  EXPECT_EQ(f.get_chunk().constant(1).type(), cuke::value_type::string);
  EXPECT_EQ(f.get_chunk().constant(1).as<std::string>(),
            "Feature: Hello World");
  EXPECT_EQ(f.get_chunk().constant(2).type(), cuke::value_type::function);
  EXPECT_EQ(f.get_chunk().constant(2).as<function>()->name(), "line:3");
  EXPECT_EQ(f.get_chunk().constant(3).type(), cuke::value_type::string);
  EXPECT_EQ(f.get_chunk().constant(3).as<std::string>(), "line:3");
}

TEST(compiler_feature, chunk_with_tags_1)
{
  const char* script = R"*(
  @tag1 @tag2
  Feature: Hello World
)*";
  testing::internal::CaptureStderr();
  compiler::cucumber cuke(script);
  cuke.compile();
  EXPECT_EQ(cuke.get_chunk().size(), 9);
  EXPECT_EQ(std::string("[line 4] Error at end: Expect ScenarioLine\n"),
            testing::internal::GetCapturedStderr());
}
TEST(compiler_feature, chunk_with_tags_2)
{
  const char* script = R"*(
  @tag1 @tag2


  Feature: Hello World
)*";
  testing::internal::CaptureStderr();
  compiler::cucumber cuke(script);
  cuke.compile();
  EXPECT_EQ(cuke.get_chunk().size(), 9);
  EXPECT_EQ(std::string("[line 6] Error at end: Expect ScenarioLine\n"),
            testing::internal::GetCapturedStderr());
}
TEST(compiler_feature, chunk_with_tags_3)
{
  const char* script = R"*(
  @tag1 @tag2
  # some comments
  # inbetween
  Feature: Hello World
)*";
  testing::internal::CaptureStderr();
  compiler::cucumber cuke(script);
  cuke.compile();
  EXPECT_EQ(cuke.get_chunk().size(), 9);
  EXPECT_EQ(std::string("[line 6] Error at end: Expect ScenarioLine\n"),
            testing::internal::GetCapturedStderr());
}