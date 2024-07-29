#include <gtest/gtest.h>

#include "../src/compiler/cucumber.hpp"

#define PRINT_STACK 1
#include "../src/debug.hpp"

using namespace cuke::internal;

TEST(compiler_single_line, feature_chunk_1_scenario)
{
  const char* script = R"*(
  Feature: Hello World
  
  Scenario: A Scenario
  Given A Step

  Scenario: Another Scenario
  Given A Step
)*";
  file test_file{"", script, {7}};
  compiler::cucumber cuke(test_file);
  cuke.set_options(options{.quiet = true});
  cuke.compile();
  chunk* f = cuke.get_chunk().constant(0).as<function>().get();

  ASSERT_EQ(f->size(), 14);

  std::size_t i = 0;
  EXPECT_EQ(f->at(i++), to_uint(op_code::reset_context));
  EXPECT_EQ(f->at(i++), to_uint(op_code::hook_before));
  EXPECT_EQ(f->at(i++), 0);
  EXPECT_EQ(f->at(i++), to_uint(op_code::constant));
  EXPECT_EQ(f->at(i++), 2);
  EXPECT_EQ(f->at(i++), to_uint(op_code::define_var));
  EXPECT_EQ(f->at(i++), 3);
  EXPECT_EQ(f->at(i++), to_uint(op_code::get_var));
  EXPECT_EQ(f->at(i++), 3);
  EXPECT_EQ(f->at(i++), to_uint(op_code::call));
  EXPECT_EQ(f->at(i++), 0);
  EXPECT_EQ(f->at(i++), to_uint(op_code::hook_after));
  EXPECT_EQ(f->at(i++), 0);
  EXPECT_EQ(f->at(i++), to_uint(op_code::func_return));

  disassemble_chunk(*f, "feature");
}
TEST(compiler_single_line, feature_chunk_3_scenarios)
{
  const char* script = R"*(
  Feature: Hello World
  
  Scenario: A Scenario
  Given A Step

  Scenario: Another Scenario
  Given A Step
  Scenario: Another Scenario
  Given A Step
  Scenario: Another Scenario
  Given A Step
)*";
  file test_file{"", script, {9, 11, 4}};
  compiler::cucumber cuke(test_file);
  cuke.set_options(options{.quiet = true});
  cuke.compile();
  chunk* f = cuke.get_chunk().constant(0).as<function>().get();

  ASSERT_EQ(f->size(), 40);

  std::size_t i = 0;
  for ([[maybe_unused]] std::size_t const_idx : {2, 4, 6})
  {
    EXPECT_EQ(f->at(i++), to_uint(op_code::reset_context));
    EXPECT_EQ(f->at(i++), to_uint(op_code::hook_before));
    EXPECT_EQ(f->at(i++), 0);
    EXPECT_EQ(f->at(i++), to_uint(op_code::constant));
    EXPECT_EQ(f->at(i++), const_idx);
    EXPECT_EQ(f->at(i++), to_uint(op_code::define_var));
    EXPECT_EQ(f->at(i++), const_idx + 1);
    EXPECT_EQ(f->at(i++), to_uint(op_code::get_var));
    EXPECT_EQ(f->at(i++), const_idx + 1);
    EXPECT_EQ(f->at(i++), to_uint(op_code::call));
    EXPECT_EQ(f->at(i++), 0);
    EXPECT_EQ(f->at(i++), to_uint(op_code::hook_after));
    EXPECT_EQ(f->at(i++), 0);
  }
  EXPECT_EQ(f->at(i++), to_uint(op_code::func_return));

  disassemble_chunk(*f, "feature");
}

TEST(compiler_single_line, feature_chunk_0_scenarios)
{
  const char* script = R"*(
  Feature: Hello World
  
  Scenario: A Scenario
  Given A Step

  Scenario: Another Scenario
  Given A Step
)*";
  file test_file{"", script, {3}};
  compiler::cucumber cuke(test_file);
  cuke.set_options(options{.quiet = true});
  cuke.compile();
  chunk* f = cuke.get_chunk().constant(0).as<function>().get();

  ASSERT_EQ(f->size(), 1);
  EXPECT_EQ(f->at(0), to_uint(op_code::func_return));

  disassemble_chunk(*f, "feature");
}

TEST(compiler_single_line, feature_chunk_single_examples_1)
{
  const char* script = R"*(
  Feature: Hello World
  
  Scenario Outline: A Scenario
  Given A Step with <value>

  Examples:
  | value |
  | 1     |
  | 2     |
  | 3     |
)*";
  file test_file{"", script, {9}};
  compiler::cucumber cuke(test_file);
  cuke.set_options(options{.quiet = true});
  cuke.compile();
  chunk* f = cuke.get_chunk().constant(0).as<function>().get();

  ASSERT_EQ(f->size(), 22);

  std::size_t i = 0;
  EXPECT_EQ(f->at(i++), to_uint(op_code::constant));
  EXPECT_EQ(f->at(i++), 2);
  EXPECT_EQ(f->at(i++), to_uint(op_code::define_var));
  EXPECT_EQ(f->at(i++), 3);
  EXPECT_EQ(f->at(i++), to_uint(op_code::constant));
  EXPECT_EQ(f->at(i++), 5);
  EXPECT_EQ(f->at(i++), to_uint(op_code::define_var));
  EXPECT_EQ(f->at(i++), 4);
  EXPECT_EQ(f->at(i++), to_uint(op_code::constant));
  EXPECT_EQ(f->at(i++), 6);
  EXPECT_EQ(f->at(i++), to_uint(op_code::set_var));
  EXPECT_EQ(f->at(i++), 4);
  EXPECT_EQ(f->at(i++), to_uint(op_code::reset_context));
  EXPECT_EQ(f->at(i++), to_uint(op_code::hook_before));
  EXPECT_EQ(f->at(i++), 0);
  EXPECT_EQ(f->at(i++), to_uint(op_code::get_var));
  EXPECT_EQ(f->at(i++), 3);
  EXPECT_EQ(f->at(i++), to_uint(op_code::call));
  EXPECT_EQ(f->at(i++), 0);
  EXPECT_EQ(f->at(i++), to_uint(op_code::hook_after));
  EXPECT_EQ(f->at(i++), 0);
  EXPECT_EQ(f->at(i++), to_uint(op_code::func_return));

  disassemble_chunk(*f, "feature");
}

TEST(compiler_single_line, feature_chunk_single_examples_2)
{
  const char* script = R"*(
  Feature: Hello World
  
  Scenario Outline: A Scenario
  Given A Step with <value>

  Examples:
  | value |
  | 1     |
  | 2     |
  | 3     |
)*";
  file test_file{"", script, {9, 11}};
  compiler::cucumber cuke(test_file);
  cuke.set_options(options{.quiet = true});
  cuke.compile();
  chunk* f = cuke.get_chunk().constant(0).as<function>().get();

  ASSERT_EQ(f->size(), 35);

  std::size_t i = 0;
  EXPECT_EQ(f->at(i++), to_uint(op_code::constant));
  EXPECT_EQ(f->at(i++), 2);
  EXPECT_EQ(f->at(i++), to_uint(op_code::define_var));
  EXPECT_EQ(f->at(i++), 3);
  EXPECT_EQ(f->at(i++), to_uint(op_code::constant));
  EXPECT_EQ(f->at(i++), 5);
  EXPECT_EQ(f->at(i++), to_uint(op_code::define_var));
  EXPECT_EQ(f->at(i++), 4);
  for (std::size_t const_idx : {6, 7})
  {
    EXPECT_EQ(f->at(i++), to_uint(op_code::constant));
    EXPECT_EQ(f->at(i++), const_idx);
    EXPECT_EQ(f->at(i++), to_uint(op_code::set_var));
    EXPECT_EQ(f->at(i++), 4);
    EXPECT_EQ(f->at(i++), to_uint(op_code::reset_context));
    EXPECT_EQ(f->at(i++), to_uint(op_code::hook_before));
    EXPECT_EQ(f->at(i++), 0);
    EXPECT_EQ(f->at(i++), to_uint(op_code::get_var));
    EXPECT_EQ(f->at(i++), 3);
    EXPECT_EQ(f->at(i++), to_uint(op_code::call));
    EXPECT_EQ(f->at(i++), 0);
    EXPECT_EQ(f->at(i++), to_uint(op_code::hook_after));
    EXPECT_EQ(f->at(i++), 0);
  }
  EXPECT_EQ(f->at(i++), to_uint(op_code::func_return));

  disassemble_chunk(*f, "feature");
}
TEST(compiler_single_line, scenario_and_example)
{
  const char* script = R"*(
  Feature: Hello World
  
  Scenario: A Scenario
  Given A Step

  Scenario Outline: A Scenario
  Given A Step with <value>

  Examples:
  | value |
  | 1     |
  | 2     |
  | 3     |
)*";
  file test_file{"", script, {4, 14}};
  compiler::cucumber cuke(test_file);
  cuke.set_options(options{.quiet = true});
  cuke.compile();
  chunk* f = cuke.get_chunk().constant(0).as<function>().get();

  ASSERT_EQ(f->size(), 35);

  disassemble_chunk(*f, "feature");
}