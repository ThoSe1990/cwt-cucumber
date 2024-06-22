#include <gtest/gtest.h>

#include "../src/compiler/scenario_outline.hpp"

#define PRINT_STACK 1
#include "../src/debug.hpp"

using namespace cwt::details;

TEST(compiler_scenario_outline_strings, values_wo_quotes_1)
{
const char* script = R"*(
  Feature: Hello World
  Scenario Outline: A Scenario
  Given A Step with <first var> and <second var>
  
  Examples:
  | first var         | second var   |
  | this is a string  | and this too |
)*";

  compiler::cucumber cuke(script);
  compiler::feature f(&cuke);
  compiler::scenario_outline s(&f);
  s.compile();
  
  ASSERT_EQ(f.get_chunk().size(), 47);
  ASSERT_EQ(f.get_chunk().constants_count(), 12);
  EXPECT_EQ(f.get_chunk().constant(8).as<std::string>(), std::string("this is a string"));
  EXPECT_EQ(f.get_chunk().constant(9).as<std::string>(), std::string("and this too"));

  disassemble_chunk(f.get_chunk(), "Feature");
  disassemble_chunk(s.get_chunk(), "Scenario");
}
TEST(compiler_scenario_outline_strings, values_w_quotes)
{
const char* script = R"*(
  Feature: Hello World
  Scenario Outline: A Scenario
  Given A Step with <first var> and <second var>
  
  Examples:
  | first var         | second var   |
  | "this is a string"  | and this too |
)*";

  compiler::cucumber cuke(script);
  compiler::feature f(&cuke);
  compiler::scenario_outline s(&f);
  s.compile();
  
  ASSERT_EQ(f.get_chunk().size(), 47);
  ASSERT_EQ(f.get_chunk().constants_count(), 12);
  EXPECT_EQ(f.get_chunk().constant(8).as<std::string>(), std::string("this is a string"));
  EXPECT_EQ(f.get_chunk().constant(9).as<std::string>(), std::string("and this too"));

  disassemble_chunk(f.get_chunk(), "Feature");
  disassemble_chunk(s.get_chunk(), "Scenario");
}

TEST(compiler_scenario_outline_strings, values_wo_quotes_2)
{
const char* script = R"*(
  Feature: Hello World
  Scenario Outline: A Scenario
  Given A Step with <first var> and <second var>
  
  Examples:
  | first var         | second var |
  | -1 can be an int  | 1 too      |
)*";

  compiler::cucumber cuke(script);
  compiler::feature f(&cuke);
  compiler::scenario_outline s(&f);
  s.compile();
  
  ASSERT_EQ(f.get_chunk().size(), 47);
  ASSERT_EQ(f.get_chunk().constants_count(), 12);
  EXPECT_EQ(f.get_chunk().constant(8).as<std::string>(), std::string("-1 can be an int"));
  EXPECT_EQ(f.get_chunk().constant(9).as<std::string>(), std::string("1 too"));

  disassemble_chunk(f.get_chunk(), "Feature");
  disassemble_chunk(s.get_chunk(), "Scenario");
}

TEST(compiler_scenario_outline_strings, values_wo_quotes_3)
{
const char* script = R"*(
  Feature: Hello World
  Scenario Outline: A Scenario
  Given A Step with <first var> and <second var>
  
  Examples:
  | first var | second var |
  | single    | word       |
)*";

  compiler::cucumber cuke(script);
  compiler::feature f(&cuke);
  compiler::scenario_outline s(&f);
  s.compile();
  
  ASSERT_EQ(f.get_chunk().size(), 47);
  ASSERT_EQ(f.get_chunk().constants_count(), 12);
  EXPECT_EQ(f.get_chunk().constant(8).as<std::string>(), std::string("single"));
  EXPECT_EQ(f.get_chunk().constant(9).as<std::string>(), std::string("word"));

  disassemble_chunk(f.get_chunk(), "Feature");
  disassemble_chunk(s.get_chunk(), "Scenario");
}
