#include <gtest/gtest.h>

#include "../src/cwt/compiler/scenario.hpp"

#define PRINT_STACK 1
#include "../src/cwt/debug.hpp"

using namespace cwt::details;

TEST(compiler_scenario, chunk_wo_step)
{
const char* script = R"*(
  Feature: Hello World
  Scenario: A Scenario
)*";
  testing::internal::CaptureStderr();
  compiler::cucumber cuke(script);
  compiler::feature f(&cuke);
  compiler::scenario s(&f);
  s.compile();
  
  ASSERT_EQ(s.get_chunk().size(), 11);

  std::size_t i = 0;
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::print_linebreak));
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::constant));
  EXPECT_EQ(s.get_chunk().at(i++), 1);
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::print));
  EXPECT_EQ(s.get_chunk().at(i++), 0);
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::print_indent));
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::constant));
  EXPECT_EQ(s.get_chunk().at(i++), 0);
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::println));
  EXPECT_EQ(s.get_chunk().at(i++), 5);
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::init_scenario));

  EXPECT_EQ(std::string("[line 4] Error at end: Expect StepLine\n"),
            testing::internal::GetCapturedStderr());
}

TEST(compiler_scenario, chunk_w_step)
{
const char* script = R"*(
  Feature: Hello World
  Scenario: A Scenario
  Given A Step
)*";

  compiler::cucumber cuke(script);
  compiler::feature f(&cuke);
  compiler::scenario s(&f);
  s.compile();
  ASSERT_EQ(s.get_chunk().size(), 22);

  std::size_t i = 0;
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::print_linebreak));
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::constant));
  EXPECT_EQ(s.get_chunk().at(i++), 1);
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::print));
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(color::standard));
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::print_indent));
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::constant));
  EXPECT_EQ(s.get_chunk().at(i++), 0);
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::println));
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(color::black));
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::init_scenario));
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::jump_if_failed));
  EXPECT_EQ(s.get_chunk().at(i++), 17);
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::hook_before_step));
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::call_step));
  EXPECT_EQ(s.get_chunk().at(i++), 3);
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::hook_after_step));
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::constant));
  EXPECT_EQ(s.get_chunk().at(i++), 3);
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::constant));
  EXPECT_EQ(s.get_chunk().at(i++), 2);
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::print_step_result));
}

TEST(compiler_scenario, chunk_size_3_steps)
{
const char* script = R"*(
  Feature: Hello World
  Scenario: A Scenario
  Given First Step
  When Second Step
  Then Third Step
)*";

  compiler::cucumber cuke(script);
  compiler::feature f(&cuke);
  compiler::scenario s(&f);
  s.compile();
  EXPECT_EQ(s.get_chunk().size(), 44);
}

TEST(compiler_scenario, chunk_size_2_scenarios)
{
const char* script = R"*(
  Feature: Hello World
  Scenario: A Scenario
  Given A Step
  Scenario: Another Scenario
  Given A Step
)*";
  
  compiler::cucumber cuke(script);
  compiler::feature f(&cuke);
  f.compile();
  EXPECT_EQ(f.get_chunk().size(), 35);
}


TEST(compiler_scenario, chunk_w_tags_1)
{
const char* script = R"*(
  Feature: Hello World
  @tag1 @tag2
  Scenario: A Scenario
)*";
  compiler::cucumber cuke(script);
  cuke.compile();
  
  ASSERT_EQ(cuke.get_chunk().size(), 9);
}

TEST(compiler_scenario, chunk_w_tags_2)
{
const char* script = R"*(
  Feature: Hello World
  @tag1 @tag2
  Scenario: A Scenario
)*";
  compiler::cucumber cuke(script);
  compiler::feature f(&cuke);
  f.set_options(options{.tags=compiler::tag_expression("@some_other_tag")});
  f.compile();
  
  disassemble_chunk(f.get_chunk(), "chunk w/o matching tags");

  EXPECT_EQ(f.get_chunk().size(), 9);
}

TEST(compiler_scenario, chunk_w_tags_3)
{
const char* script = R"*(
  Feature: Hello World
  @tag1 @tag2
  Scenario: A Scenario
)*";
  compiler::cucumber cuke(script);
  compiler::feature f(&cuke);
  f.set_options(options{.tags=compiler::tag_expression("@tag1 and @tag2")});
  f.compile();
  
  disassemble_chunk(f.get_chunk(), "chunk with matching tags");

  EXPECT_EQ(f.get_chunk().size(), 30);
}
