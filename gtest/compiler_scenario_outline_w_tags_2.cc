#include <gtest/gtest.h>

#include "../src/cwt/compiler/scenario_outline.hpp"

#define PRINT_STACK 1
#include "../src/cwt/debug.hpp"

using namespace cwt::details;

namespace compiler_scenario_outline_w_tags_2
{
const char* script = R"*(
  Feature: Hello World
  
  @tag1 @tag2
  Scenario Outline: A Scenario
  Given A Step with <first var> and <second var>
  
  @tag3
  Examples:
  | first var | second var |
  | 1         | 2          |

  Examples:
  | first var | second var |
  | 3         | 4          |
)*";
}  // namespace compiler_scenario_outline_w_tags_2

TEST(compiler_scenario_outline_w_tags_2, scenarios_code_1)
{
  compiler::cucumber cuke(compiler_scenario_outline_w_tags_2::script);
  compiler::feature f(&cuke);
  f.compile();
  disassemble_chunk(f.get_chunk(), "scenario outline");
  EXPECT_EQ(f.get_chunk().size(), 100);
}
