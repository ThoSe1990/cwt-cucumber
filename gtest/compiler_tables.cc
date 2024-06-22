#include <gtest/gtest.h>

#include "../src/compiler/scenario.hpp"

#define PRINT_STACK 1
#include "../src/debug.hpp"

using namespace cwt::details;


TEST(compiler_tables, chunk_w_datatable_2_2)
{
const char* script = R"*(
  Feature: Hello World
  Scenario: A Scenario
  Given A Step with a datatable
  | "v1" | "v2" |
  | 1    | 2    |
)*";

  compiler::cucumber cuke(script);
  compiler::feature f(&cuke);
  compiler::scenario s(&f);
  s.compile();

  disassemble_chunk(s.get_chunk(), "scenario");

  ASSERT_TRUE(s.no_error());
  ASSERT_EQ(s.get_chunk().size(), 22);

  std::size_t i = 0;
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::print_linebreak));
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::constant));
  EXPECT_EQ(s.get_chunk().at(i++), 1);
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::print));
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(color::standard));
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::print_indent));
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::constant));
  EXPECT_EQ(s.get_chunk().at(i++), 0);
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::println));
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(color::black));
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::init_scenario));
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::jump_if_failed));
  EXPECT_EQ(s.get_chunk().at(i++), 17);
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::hook_before_step));
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::call_step));
  EXPECT_EQ(s.get_chunk().at(i++), 3);
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::hook_after_step));
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::constant));
  EXPECT_EQ(s.get_chunk().at(i++), 3);
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::constant));
  EXPECT_EQ(s.get_chunk().at(i++), 2);
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::print_step_result));
}
TEST(compiler_tables, chunk_w_datatable_2_2_no_quotes)
{
const char* script = R"*(
  Feature: Hello World
  Scenario: A Scenario
  Given A Step with a datatable
  | v1 | v2 |
  | 1  | 2  |
)*";

  compiler::cucumber cuke(script);
  compiler::feature f(&cuke);
  compiler::scenario s(&f);
  s.compile();

  disassemble_chunk(s.get_chunk(), "scenario");

  ASSERT_TRUE(s.no_error());
  ASSERT_EQ(s.get_chunk().size(), 22);

}