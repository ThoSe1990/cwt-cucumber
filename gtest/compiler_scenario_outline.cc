#include <gtest/gtest.h>

#include "../src/cwt/compiler/scenario_outline.hpp"

using namespace cwt::details;

TEST(compiler_scenario_outline, first_scenario)
{
const char* script = R"*(
  Feature: Hello World
  Scenario Outline: A Scenario
  Given A Step with <var> 
  
  Examples:
  | var |
  | 111   |
  | 222   |
  Examples:
  | var |
  | 1.11   |
  | 2.22   |
  Examples:
  | var |
  | "1"   |
  | "2"   |
)*";
  
  compiler::cucumber cuke(script);
  cuke.compile();
  // compiler::feature f(&cuke);
  // compiler::scenario_outline s(&f);
  // s.compile();
  // s.finish_chunk();

}
