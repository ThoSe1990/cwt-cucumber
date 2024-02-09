#include <gtest/gtest.h>

#include "../src/cwt/compiler/scenario_outline.hpp"

#define PRINT_STACK 1
#include "../src/cwt/debug.hpp"

using namespace cwt::details;

namespace compiler_scenario_w_background
{
const char* script = R"*(
  Feature: First Feature

  Background:
  * Call me
  * Call me
  
  Scenario: First Scenario
  * Call me
)*";
}  // namespace compiler_scenario_w_background

TEST(compiler_scenario_w_background, scenario_chunk)
{
  compiler::cucumber cuke(compiler_scenario_w_background::script);

  cuke.compile();
  disassemble_chunk(cuke.get_chunk(), "script");

  chunk* f = cuke.get_chunk().constant(0).as<function>().get();
  disassemble_chunk(*f, "feature");

  chunk* s = f->constant(2).as<function>().get();
  disassemble_chunk(*s, "scenario");

  std::size_t i = 10;
  EXPECT_EQ(s->at(i++), to_uint(op_code::init_scenario));
  EXPECT_EQ(s->at(i++), to_uint(op_code::constant));
  EXPECT_EQ(s->at(i++), 2);
  EXPECT_EQ(s->at(i++), to_uint(op_code::define_var));
  EXPECT_EQ(s->at(i++), 3);
  EXPECT_EQ(s->at(i++), to_uint(op_code::get_var));
  EXPECT_EQ(s->at(i++), 3);
  EXPECT_EQ(s->at(i++), to_uint(op_code::call));
  EXPECT_EQ(s->at(i++), 0);
}

TEST(compiler_scenario_w_background, background_chunk)
{
  compiler::cucumber cuke(compiler_scenario_w_background::script);

  cuke.compile();
  disassemble_chunk(cuke.get_chunk(), "script");

  chunk* f = cuke.get_chunk().constant(0).as<function>().get();
  disassemble_chunk(*f, "feature");

  chunk* s = f->constant(2).as<function>().get();
  disassemble_chunk(*s, "scenario");

  chunk* b = s->constant(2).as<function>().get();
  disassemble_chunk(*b, "background");

  ASSERT_EQ(b->size(), 23);

  std::size_t i = 0;
  EXPECT_EQ(b->at(i++), to_uint(op_code::jump_if_failed));
  EXPECT_EQ(b->at(i++), 6);
  EXPECT_EQ(b->at(i++), to_uint(op_code::hook_before_step));
  EXPECT_EQ(b->at(i++), to_uint(op_code::call_step));
  EXPECT_EQ(b->at(i++), 1);
  EXPECT_EQ(b->at(i++), to_uint(op_code::hook_after_step));
  EXPECT_EQ(b->at(i++), to_uint(op_code::constant));
  EXPECT_EQ(b->at(i++), 1);
  EXPECT_EQ(b->at(i++), to_uint(op_code::constant));
  EXPECT_EQ(b->at(i++), 0);
  EXPECT_EQ(b->at(i++), to_uint(op_code::print_step_result));
  EXPECT_EQ(b->at(i++), to_uint(op_code::jump_if_failed));
  EXPECT_EQ(b->at(i++), 17);
  EXPECT_EQ(b->at(i++), to_uint(op_code::hook_before_step));
  EXPECT_EQ(b->at(i++), to_uint(op_code::call_step));
  EXPECT_EQ(b->at(i++), 3);
  EXPECT_EQ(b->at(i++), to_uint(op_code::hook_after_step));
  EXPECT_EQ(b->at(i++), to_uint(op_code::constant));
  EXPECT_EQ(b->at(i++), 3);
  EXPECT_EQ(b->at(i++), to_uint(op_code::constant));
  EXPECT_EQ(b->at(i++), 2);
  EXPECT_EQ(b->at(i++), to_uint(op_code::print_step_result));
  EXPECT_EQ(b->at(i++), to_uint(op_code::func_return));
}