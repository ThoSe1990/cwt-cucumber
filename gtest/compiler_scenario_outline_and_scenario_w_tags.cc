#include <gtest/gtest.h>

#include "../src/cwt/compiler/scenario_outline.hpp"

#define PRINT_STACK 1
#include "../src/cwt/debug.hpp"

using namespace cwt::details;

namespace compiler_scenario_and_scenario_outline_w_tags
{
const char* script = R"*(
  Feature: Hello World
  
  Scenario Outline: A Scenario
  Given A Step with <first var> and <second var>
  
  Examples:
  | first var | second var |
  | 1         | 2          |

  @tag1 @tag2
  Scenario: A Scenario
  Given A Step
  
  Scenario: A Scenario
  Given A Step
)*";
}  // namespace compiler_scenario_and_scenario_outline_w_tags

TEST(compiler_scenario_and_scenario_outline_w_tags, scenarios_code_1)
{
  compiler::cucumber cuke(compiler_scenario_and_scenario_outline_w_tags::script);
  compiler::feature f(&cuke);
  f.compile();
  disassemble_chunk(f.get_chunk(), "scenario outline");
  EXPECT_EQ(f.get_chunk().size(), 79);
}
// TEST(compiler_scenario_and_scenario_outline_w_tags, scenarios_code_2)
// {
//   compiler::cucumber cuke(compiler_scenario_and_scenario_outline_w_tags::script);
//   compiler::feature f(&cuke);
//   f.set_tag_expression("@some_other_tag");
//   f.compile();
//   disassemble_chunk(f.get_chunk(), "scenario outline");
//   EXPECT_EQ(f.get_chunk().size(), 58);
// }
// TEST(compiler_scenario_and_scenario_outline_w_tags, scenarios_code_3)
// {
//   compiler::cucumber cuke(compiler_scenario_and_scenario_outline_w_tags::script);
//   compiler::feature f(&cuke);
//   f.set_tag_expression("@tag1");
//   f.compile();
//   disassemble_chunk(f.get_chunk(), "scenario outline");
//   EXPECT_EQ(f.get_chunk().size(), 79);
// }
// TEST(compiler_scenario_and_scenario_outline_w_tags, scenarios_code_4)
// {
//   compiler::cucumber cuke(compiler_scenario_and_scenario_outline_w_tags::script);
//   compiler::feature f(&cuke);
//   f.set_tag_expression("@tag1 and @tag2");
//   f.compile();
//   disassemble_chunk(f.get_chunk(), "scenario outline");
//   EXPECT_EQ(f.get_chunk().size(), 79);
// }
// TEST(compiler_scenario_and_scenario_outline_w_tags, scenarios_code_5)
// {
//   compiler::cucumber cuke(compiler_scenario_and_scenario_outline_w_tags::script);
//   compiler::feature f(&cuke);
//   f.set_tag_expression("@tag1 or @tag2");
//   f.compile();
//   disassemble_chunk(f.get_chunk(), "scenario outline");
//   EXPECT_EQ(f.get_chunk().size(), 79);
// }
TEST(compiler_scenario_and_scenario_outline_w_tags, scenarios_code_6)
{
  compiler::cucumber cuke(compiler_scenario_and_scenario_outline_w_tags::script);
  compiler::feature f(&cuke);
  
  f.set_options(options{.tags=compiler::tag_expression("@tag1 xor @tag2")});
  
  f.compile();
  disassemble_chunk(f.get_chunk(), "scenario outline");
  EXPECT_EQ(f.get_chunk().size(), 58);
}
