#include <gtest/gtest.h>

#include "../src/cwt/compiler/scenario_outline.hpp"

using namespace cwt::details;

TEST(compiler_scenario_outline, first_scenario)
{
const char* script = R"*(
  Feature: Hello World
  Scenario Outline: A Scenario
  Given A Step with <va  r1> and <va r2>
  
  Examples:
  | va  r1 | var 2 |
  | 1    | 2    |
  | 3    | 4    |
)*";
  
  compiler::cucumber cuke(script);
  compiler::feature f(&cuke);
  compiler::scenario_outline s(&f);
  s.compile();

}
