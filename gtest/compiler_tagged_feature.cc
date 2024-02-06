#include <gtest/gtest.h>

#include "../src/cwt/compiler/scenario_outline.hpp"

#define PRINT_STACK 1
#include "../src/cwt/debug.hpp"

using namespace cwt::details;

TEST(compiler_tagged_feature, scenarios_code_1)
{
const char* script = R"*(
  @tag1 @tag2
  Feature: Hello World
  Scenario: A Scenario
)*";
  compiler::cucumber cuke(script);
  cuke.compile();
  chunk* feature_chunk = cuke.get_chunk().constant(0).as<function>().get(); 
  EXPECT_EQ(feature_chunk->size(), 32);
  disassemble_chunk(cuke.get_chunk(), "script");
  disassemble_chunk(*feature_chunk, "feature");
}