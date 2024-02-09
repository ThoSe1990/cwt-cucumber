#include <gtest/gtest.h>

#include "../src/cwt/compiler/scenario_outline.hpp"

#define PRINT_STACK 1
#include "../src/cwt/debug.hpp"

using namespace cwt::details;

TEST(compiler_quiet, quiet_feature_chunk)
{
  const char* script = R"*(
  Feature: Hello World
  Scenario: A Scenario
  Given A Step
)*";
  compiler::cucumber cuke(script);
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

  disassemble_chunk(cuke.get_chunk(), "script");
  disassemble_chunk(*f, "scenario");
}
TEST(compiler_quiet, quiet_scenario_chunk)
{
  const char* script = R"*(
  Feature: Hello World
  Scenario: A Scenario
  Given A Step
)*";
  compiler::cucumber cuke(script);
  cuke.set_options(options{.quiet = true});
  cuke.compile();
  chunk* f = cuke.get_chunk().constant(0).as<function>().get();
  chunk* s = f->constant(2).as<function>().get();

  ASSERT_EQ(s->size(), 8);

  std::size_t i = 0;
  EXPECT_EQ(s->at(i++), to_uint(op_code::init_scenario));
  EXPECT_EQ(s->at(i++), to_uint(op_code::jump_if_failed));
  EXPECT_EQ(s->at(i++), 7);
  EXPECT_EQ(s->at(i++), to_uint(op_code::hook_before_step));
  EXPECT_EQ(s->at(i++), to_uint(op_code::call_step));
  EXPECT_EQ(s->at(i++), 3);
  EXPECT_EQ(s->at(i++), to_uint(op_code::hook_after_step));
  EXPECT_EQ(s->at(i++), to_uint(op_code::func_return));

  disassemble_chunk(cuke.get_chunk(), "script");
  disassemble_chunk(*s, "scenario");
}
TEST(compiler_quiet, quiet_feature_outline_chunk)
{
  const char* script = R"*(
  Feature: Hello World
  Scenario Outline: A Scenario
  Given A Step with <value>

  Examples:
  | value |
  | 1234  |
)*";
  compiler::cucumber cuke(script);
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

  disassemble_chunk(cuke.get_chunk(), "script");
  disassemble_chunk(*f, "feature");
}
TEST(compiler_quiet, quiet_scenario_outline_chunk)
{
  const char* script = R"*(
  Feature: Hello World
  Scenario Outline: A Scenario
  Given A Step with <value>

  Examples:
  | value |
  | 1234  |
)*";
  compiler::cucumber cuke(script);
  cuke.set_options(options{.quiet = true});
  cuke.compile();
  chunk* f = cuke.get_chunk().constant(0).as<function>().get();
  chunk* s = f->constant(2).as<function>().get();

  ASSERT_EQ(s->size(), 10);

  std::size_t i = 0;
  EXPECT_EQ(s->at(i++), to_uint(op_code::init_scenario));
  EXPECT_EQ(s->at(i++), to_uint(op_code::get_var));
  EXPECT_EQ(s->at(i++), 3);
  EXPECT_EQ(s->at(i++), to_uint(op_code::jump_if_failed));
  EXPECT_EQ(s->at(i++), 9);
  EXPECT_EQ(s->at(i++), to_uint(op_code::hook_before_step));
  EXPECT_EQ(s->at(i++), to_uint(op_code::call_step));
  EXPECT_EQ(s->at(i++), 4);
  EXPECT_EQ(s->at(i++), to_uint(op_code::hook_after_step));
  EXPECT_EQ(s->at(i++), to_uint(op_code::func_return));

  disassemble_chunk(cuke.get_chunk(), "script");
  disassemble_chunk(*s, "scenario outline");
}