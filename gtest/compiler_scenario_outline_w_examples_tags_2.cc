#include <gtest/gtest.h>

#include "../src/compiler/scenario_outline.hpp"

#define PRINT_STACK 1
#include "../src/debug.hpp"

using namespace cwt::details;

namespace w_examples_tags_2
{
const char* script = R"*(
  Feature: Hello World
  Scenario Outline: A Scenario
  Given A Step with <first var> and <second var>
  
  @tag1
  Examples:
  | first var | second var |
  | 1         | 2          |
  

  @tag2 @tag3
  Scenario Outline: A Scenario
  Given A Step with <first var> and <second var>
  Examples:
  | first var | second var |
  | 3         | 4          |
)*";
}  // namespace w_examples_tags_2

TEST(compiler_scenario_outline_w_examples_tags_2, scenarios_code_1)
{
  compiler::cucumber cuke(w_examples_tags_2::script);
  compiler::feature f(&cuke);
  f.compile();
  disassemble_chunk(f.get_chunk(), "scenario outline");
  EXPECT_EQ(f.get_chunk().size(), 96);
}
TEST(compiler_scenario_outline_w_examples_tags_2, scenarios_code_2)
{
  compiler::cucumber cuke(w_examples_tags_2::script);
  compiler::feature f(&cuke);
  f.set_options(options{.tags = compiler::tag_expression("@tag1")});
  f.compile();
  disassemble_chunk(f.get_chunk(), "scenario outline");
  EXPECT_EQ(f.get_chunk().size(), 51);
}
TEST(compiler_scenario_outline_w_examples_tags_2, scenarios_code_3)
{
  compiler::cucumber cuke(w_examples_tags_2::script);
  compiler::feature f(&cuke);
  f.set_options(options{.tags = compiler::tag_expression("not @tag2")});
  f.compile();
  disassemble_chunk(f.get_chunk(), "scenario outline");
  EXPECT_EQ(f.get_chunk().size(), 51);
}
TEST(compiler_scenario_outline_w_examples_tags_2, scenarios_code_4)
{
  compiler::cucumber cuke(w_examples_tags_2::script);
  compiler::feature f(&cuke);
  f.set_options(options{.tags = compiler::tag_expression("not @tag1")});
  f.compile();
  disassemble_chunk(f.get_chunk(), "scenario outline");
  EXPECT_EQ(f.get_chunk().size(), 59);
}
TEST(compiler_scenario_outline_w_examples_tags_2, scenarios_code_5)
{
  compiler::cucumber cuke(w_examples_tags_2::script);
  compiler::feature f(&cuke);
  f.set_options(options{.tags = compiler::tag_expression("@tag1 or @tag2")});
  f.compile();
  disassemble_chunk(f.get_chunk(), "scenario outline");
  EXPECT_EQ(f.get_chunk().size(), 96);
}
TEST(compiler_scenario_outline_w_examples_tags_2, scenarios_code_6)
{
  compiler::cucumber cuke(w_examples_tags_2::script);
  compiler::feature f(&cuke);
  f.set_options(options{.tags = compiler::tag_expression("@tag2")});
  f.compile();
  disassemble_chunk(f.get_chunk(), "scenario outline");
  EXPECT_EQ(f.get_chunk().size(), 59);
}
TEST(compiler_scenario_outline_w_examples_tags_2, scenarios_code_7)
{
  compiler::cucumber cuke(w_examples_tags_2::script);
  compiler::feature f(&cuke);
  f.set_options(options{.tags = compiler::tag_expression("@tag2 and @tag3")});
  f.compile();
  disassemble_chunk(f.get_chunk(), "scenario outline");
  EXPECT_EQ(f.get_chunk().size(), 59);
}