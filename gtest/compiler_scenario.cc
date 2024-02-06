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
  
  ASSERT_EQ(s.get_chunk().size(), 10);

  EXPECT_EQ(s.get_chunk().at(0), to_uint(op_code::constant));
  EXPECT_EQ(s.get_chunk().at(1), 1);
  EXPECT_EQ(s.get_chunk().at(2), to_uint(op_code::print));
  EXPECT_EQ(s.get_chunk().at(3), 0);
  EXPECT_EQ(s.get_chunk().at(4), to_uint(op_code::print_indent));
  EXPECT_EQ(s.get_chunk().at(5), to_uint(op_code::constant));
  EXPECT_EQ(s.get_chunk().at(6), 0);
  EXPECT_EQ(s.get_chunk().at(7), to_uint(op_code::println));
  EXPECT_EQ(s.get_chunk().at(8), 5);
  EXPECT_EQ(s.get_chunk().at(9), to_uint(op_code::init_scenario));

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
  ASSERT_EQ(s.get_chunk().size(), 21);

  EXPECT_EQ(s.get_chunk().at(0), to_uint(op_code::constant));
  EXPECT_EQ(s.get_chunk().at(1), 1);
  EXPECT_EQ(s.get_chunk().at(2), to_uint(op_code::print));
  EXPECT_EQ(s.get_chunk().at(3), to_uint(color::standard));
  EXPECT_EQ(s.get_chunk().at(4), to_uint(op_code::print_indent));
  EXPECT_EQ(s.get_chunk().at(5), to_uint(op_code::constant));
  EXPECT_EQ(s.get_chunk().at(6), 0);
  EXPECT_EQ(s.get_chunk().at(7), to_uint(op_code::println));
  EXPECT_EQ(s.get_chunk().at(8), to_uint(color::black));
  EXPECT_EQ(s.get_chunk().at(9), to_uint(op_code::init_scenario));
  EXPECT_EQ(s.get_chunk().at(10), to_uint(op_code::jump_if_failed));
  EXPECT_EQ(s.get_chunk().at(11), 16);
  EXPECT_EQ(s.get_chunk().at(12), to_uint(op_code::hook_before_step));
  EXPECT_EQ(s.get_chunk().at(13), to_uint(op_code::call_step));
  EXPECT_EQ(s.get_chunk().at(14), 3);
  EXPECT_EQ(s.get_chunk().at(15), to_uint(op_code::hook_after_step));
  EXPECT_EQ(s.get_chunk().at(16), to_uint(op_code::constant));
  EXPECT_EQ(s.get_chunk().at(17), 3);
  EXPECT_EQ(s.get_chunk().at(18), to_uint(op_code::constant));
  EXPECT_EQ(s.get_chunk().at(19), 2);
  EXPECT_EQ(s.get_chunk().at(20), to_uint(op_code::step_result));
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
  EXPECT_EQ(s.get_chunk().size(), 43);
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
  EXPECT_EQ(f.get_chunk().size(), 38);
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
  f.set_tag_expression("@some_other_tag");
  f.compile();
  
  disassemble_chunk(f.get_chunk(), "chunk w/o matching tags");

  EXPECT_EQ(f.get_chunk().size(), 10);
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
  f.set_tag_expression("@tag1 and @tag2");
  f.compile();
  
  disassemble_chunk(f.get_chunk(), "chunk with matching tags");

  EXPECT_EQ(f.get_chunk().size(), 32);
}
