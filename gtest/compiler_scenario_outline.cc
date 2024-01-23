#include <gtest/gtest.h>

#include "../src/cwt/compiler/scenario.hpp"

using namespace cwt::details;

TEST(compiler_scenario_outline, first_scenario)
{
const char* script = R"*(
  Feature: Hello World
  Scenario Outline: A Scenario
  Given A Step with <var> 
  
  Examples:
  | var |
  | 1   |
)*";
  
  compiler::cucumber cuke(script);
  compiler::feature f(&cuke);
  f.compile();


}
