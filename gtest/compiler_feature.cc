#include <gtest/gtest.h>

#include "../src/cwt/compiler/feature.hpp"

#define PRINT_STACK 1
#include "../src/cwt/debug.hpp"

using namespace cwt::details;

TEST(compiler_feature, chunk_size_wo_scenario)
{
  testing::internal::CaptureStderr();
  compiler::cucumber cuke("Feature: Hello World");
  compiler::feature f(&cuke);
  f.compile();
  EXPECT_EQ(f.get_chunk().size(), 9);
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

  std::size_t i = 0;

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
  EXPECT_EQ(f.get_chunk().name(), ":2");
  EXPECT_EQ(f.get_chunk().constant(0).type(), value_type::string);
  EXPECT_EQ(f.get_chunk().constant(0).as<std::string>(), ":2");
  EXPECT_EQ(f.get_chunk().constant(1).type(), value_type::string);
  EXPECT_EQ(f.get_chunk().constant(1).as<std::string>(),
            "Feature: Hello World");
  EXPECT_EQ(f.get_chunk().constant(2).type(), value_type::function);
  EXPECT_EQ(f.get_chunk().constant(2).as<function>()->name(), ":3");
  EXPECT_EQ(f.get_chunk().constant(3).type(), value_type::string);
  EXPECT_EQ(f.get_chunk().constant(3).as<std::string>(), ":3");
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

// TEST(compiler_feature, chunk_with_tags_4)
// {
//   const char* script = R"*(
//   @tag1 @tag2
//   Feature: Hello World
// )*";
//   testing::internal::CaptureStderr();

//   compiler::cucumber cuke(script);
//   cuke.set_tag_expression("@tag1 or @tag2");
//   cuke.compile();

//   EXPECT_EQ(cuke.get_chunk().size(), 9);
//   EXPECT_EQ(std::string("[line 4] Error at end: Expect ScenarioLine\n"),
//             testing::internal::GetCapturedStderr());
// }
// TEST(compiler_feature, chunk_with_tags_5)
// {
//   const char* script = R"*(
//   @tag1 @tag2
//   Feature: Hello World
// )*";

//   compiler::cucumber cuke(script);
//   cuke.set_tag_expression("not @tag1 and not @tag2");
//   cuke.compile();

//   EXPECT_EQ(cuke.get_chunk().size(), 1);
//   EXPECT_EQ(cuke.get_chunk().at(0), to_uint(op_code::func_return));
// }