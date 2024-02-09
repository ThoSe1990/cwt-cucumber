#include <gtest/gtest.h>

#include "../src/cwt/compiler/scenario_outline.hpp"

#define PRINT_STACK 1
#include "../src/cwt/debug.hpp"

using namespace cwt::details;

namespace w_examples_tags
{
const char* script = R"*(
  Feature: Hello World
  Scenario Outline: A Scenario
  Given A Step with <first var> and <second var>
  
  @tag1
  Examples:
  | first var | second var |
  | 1         | 2          |
  
  Examples:
  | first var | second var |
  | 3         | 4          |

  @tag2
  Examples:
  | first var | second var |
  | 5         | 6          |

)*";
}  // namespace w_examples_tags

TEST(compiler_scenario_outline_w_examples_tags, scenarios_code_1)
{
  compiler::cucumber cuke(w_examples_tags::script);
  compiler::feature f(&cuke);
  f.compile();
  disassemble_chunk(f.get_chunk(), "scenario outline");
  EXPECT_EQ(f.get_chunk().size(), 121);
}
TEST(compiler_scenario_outline_w_examples_tags, scenarios_code_2)
{
  compiler::cucumber cuke(w_examples_tags::script);
  compiler::feature f(&cuke);
  f.set_options(options{.tags=compiler::tag_expression("@some_other_tag")});
  f.compile();
  disassemble_chunk(f.get_chunk(), "scenario outline");
  EXPECT_EQ(f.get_chunk().size(), 47);
}
TEST(compiler_scenario_outline_w_examples_tags, scenarios_code_3)
{
  compiler::cucumber cuke(w_examples_tags::script);
  compiler::feature f(&cuke);
  f.set_options(options{.tags=compiler::tag_expression("@tag1")});
  f.compile();
  disassemble_chunk(f.get_chunk(), "scenario outline");
  EXPECT_EQ(f.get_chunk().size(), 84);
}
TEST(compiler_scenario_outline_w_examples_tags, scenarios_code_4)
{
  compiler::cucumber cuke(w_examples_tags::script);
  compiler::feature f(&cuke);
  f.set_options(options{.tags=compiler::tag_expression("@tag2")});
  f.compile();
  disassemble_chunk(f.get_chunk(), "scenario outline");
  EXPECT_EQ(f.get_chunk().size(), 84);
}

TEST(compiler_scenario_outline_w_examples_tags, scenarios_code_5)
{
  compiler::cucumber cuke(w_examples_tags::script);
  compiler::feature f(&cuke);
  f.set_options(options{.tags=compiler::tag_expression("@tag1 and @tag2")});
  f.compile();
  disassemble_chunk(f.get_chunk(), "scenario outline");
  EXPECT_EQ(f.get_chunk().size(), 47);
}

TEST(compiler_scenario_outline_w_examples_tags, scenarios_code_6)
{
  compiler::cucumber cuke(w_examples_tags::script);
  compiler::feature f(&cuke);
  f.set_options(options{.tags=compiler::tag_expression("@tag1 or @tag2")});
  f.compile();
  disassemble_chunk(f.get_chunk(), "scenario outline");
  EXPECT_EQ(f.get_chunk().size(), 121);
}
