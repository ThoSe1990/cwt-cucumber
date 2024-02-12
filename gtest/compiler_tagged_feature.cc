#include <gtest/gtest.h>

#include "../src/compiler/scenario_outline.hpp"

#define PRINT_STACK 1
#include "../src/debug.hpp"

using namespace cwt::details;

TEST(compiler_tagged_feature, scenarios_code_1)
{
  const char* script = R"*(
  @tag1 @tag2
  Feature: Hello World
  Scenario: A Scenario
  Given A Step
)*";
  compiler::cucumber cuke(script);
  cuke.compile();
  chunk* feature_chunk = cuke.get_chunk().constant(0).as<function>().get();
  EXPECT_EQ(feature_chunk->size(), 32);
  disassemble_chunk(cuke.get_chunk(), "script");
  disassemble_chunk(*feature_chunk, "feature");
}
TEST(compiler_tagged_feature, scenarios_code_2)
{
  const char* script = R"*(
  @tag1 @tag2
  Feature: Hello World
  Scenario: A Scenario
  Given A Step
)*";
  compiler::cucumber cuke(script);
  cuke.set_options(options{.tags = compiler::tag_expression("@tag1")});
  cuke.compile();
  chunk* f = cuke.get_chunk().constant(0).as<function>().get();
  EXPECT_EQ(f->size(), 32);
  disassemble_chunk(cuke.get_chunk(), "script");
  disassemble_chunk(*f, "feature");
}
TEST(compiler_tagged_feature, scenarios_code_3)
{
  const char* script = R"*(
  @tag1 @tag2
  Feature: Hello World
  Scenario: A Scenario
  Given A Step
)*";
  compiler::cucumber cuke(script);
  cuke.set_options(options{.tags = compiler::tag_expression("not @tag1")});
  cuke.compile();
  EXPECT_EQ(cuke.get_chunk().size(), 1);
  disassemble_chunk(cuke.get_chunk(), "script");
}