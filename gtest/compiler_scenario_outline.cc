#include <gtest/gtest.h>

#include "../src/compiler/scenario_outline.hpp"
#define PRINT_STACK 1
#include "../src/debug.hpp"

using namespace cwt::details;

namespace compiler_scenario_outline
{
const char* script = R"*(
  Feature: Hello World
  Scenario Outline: A Scenario
  Given A Step with <first var> and <second var>
  
  Examples:
  | first var | second var |
  | 1         | 2          |
  | 3         | 4          |
)*";
}  // namespace compiler_scenario_outline

TEST(compiler_scenario_outline, scenarios_code)
{
  compiler::cucumber cuke(compiler_scenario_outline::script);
  compiler::feature f(&cuke);
  compiler::scenario_outline s(&f);
  s.compile();
  ASSERT_EQ(s.get_chunk().size(), 27);

  std::size_t i = 0;

  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::print_linebreak));
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::constant));
  EXPECT_EQ(s.get_chunk().at(i++), 1);
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::print));
  EXPECT_EQ(s.get_chunk().at(i++), 0);
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::print_indent));
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::constant));
  EXPECT_EQ(s.get_chunk().at(i++), 0);
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::println));
  EXPECT_EQ(s.get_chunk().at(i++), 5);
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::init_scenario));
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::get_var));
  EXPECT_EQ(s.get_chunk().at(i++), 3);
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::get_var));
  EXPECT_EQ(s.get_chunk().at(i++), 4);
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::jump_if_failed));
  EXPECT_EQ(s.get_chunk().at(i++), 21);
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::hook_before_step));
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::call_step));
  EXPECT_EQ(s.get_chunk().at(i++), 5);
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::hook_after_step));
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::constant));
  EXPECT_EQ(s.get_chunk().at(i++), 5);
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::constant));
  EXPECT_EQ(s.get_chunk().at(i++), 2);
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::print_step_result));
  EXPECT_EQ(s.get_chunk().at(i++), to_uint(op_code::func_return));
}

TEST(compiler_scenario_outline, scenarios_constants)
{
  compiler::cucumber cuke(compiler_scenario_outline::script);
  compiler::feature f(&cuke);
  compiler::scenario_outline s(&f);
  s.compile();
  ASSERT_EQ(s.get_chunk().constants_count(), 6);
  EXPECT_EQ(s.get_chunk().constant(0).type(), cuke::value_type::string);
  EXPECT_EQ(s.get_chunk().constant(0).as<std::string>(), "line:3");
  EXPECT_EQ(s.get_chunk().constant(1).type(), cuke::value_type::string);
  EXPECT_EQ(s.get_chunk().constant(1).as<std::string>(),
            "Scenario Outline: A Scenario");
  EXPECT_EQ(s.get_chunk().constant(2).type(), cuke::value_type::string);
  EXPECT_EQ(s.get_chunk().constant(2).as<std::string>(), "line:4");
  EXPECT_EQ(s.get_chunk().constant(3).type(), cuke::value_type::string);
  EXPECT_EQ(s.get_chunk().constant(3).as<std::string>(), "first var");
  EXPECT_EQ(s.get_chunk().constant(4).type(), cuke::value_type::string);
  EXPECT_EQ(s.get_chunk().constant(4).as<std::string>(), "second var");
  EXPECT_EQ(s.get_chunk().constant(5).type(), cuke::value_type::string);
  EXPECT_EQ(s.get_chunk().constant(5).as<std::string>(),
            "A Step with <first var> and <second var>");
}

TEST(compiler_scenario_outline, parent_feature_code)
{
  compiler::cucumber cuke(compiler_scenario_outline::script);
  compiler::feature f(&cuke);
  compiler::scenario_outline s(&f);
  s.compile();

  disassemble_chunk(f.get_chunk(), "scenario");

  ASSERT_EQ(f.get_chunk().size(), 72);

  std::size_t i = 14;

  EXPECT_EQ(f.get_chunk().at(i++), to_uint(op_code::constant));
  EXPECT_EQ(f.get_chunk().at(i++), 5);
  EXPECT_EQ(f.get_chunk().at(i++), to_uint(op_code::define_var));
  EXPECT_EQ(f.get_chunk().at(i++), 4);
  EXPECT_EQ(f.get_chunk().at(i++), to_uint(op_code::constant));
  EXPECT_EQ(f.get_chunk().at(i++), 7);
  EXPECT_EQ(f.get_chunk().at(i++), to_uint(op_code::define_var));
  EXPECT_EQ(f.get_chunk().at(i++), 6);
  EXPECT_EQ(f.get_chunk().at(i++), to_uint(op_code::constant));
  EXPECT_EQ(f.get_chunk().at(i++), 8);
  EXPECT_EQ(f.get_chunk().at(i++), to_uint(op_code::set_var));
  EXPECT_EQ(f.get_chunk().at(i++), 4);
  EXPECT_EQ(f.get_chunk().at(i++), to_uint(op_code::constant));
  EXPECT_EQ(f.get_chunk().at(i++), 9);
  EXPECT_EQ(f.get_chunk().at(i++), to_uint(op_code::set_var));
  EXPECT_EQ(f.get_chunk().at(i++), 6);
  EXPECT_EQ(f.get_chunk().at(i++), to_uint(op_code::reset_context));
  EXPECT_EQ(f.get_chunk().at(i++), to_uint(op_code::hook_before));
  EXPECT_EQ(f.get_chunk().at(i++), 0);
  EXPECT_EQ(f.get_chunk().at(i++), to_uint(op_code::get_var));
  EXPECT_EQ(f.get_chunk().at(i++), 3);
  EXPECT_EQ(f.get_chunk().at(i++), to_uint(op_code::call));
  EXPECT_EQ(f.get_chunk().at(i++), 0);
  EXPECT_EQ(f.get_chunk().at(i++), to_uint(op_code::hook_after));
  EXPECT_EQ(f.get_chunk().at(i++), 0);
  EXPECT_EQ(f.get_chunk().at(i++), to_uint(op_code::constant));
  EXPECT_EQ(f.get_chunk().at(i++), 10);
  EXPECT_EQ(f.get_chunk().at(i++), to_uint(op_code::print));
  EXPECT_EQ(f.get_chunk().at(i++), 0);
  EXPECT_EQ(f.get_chunk().at(i++), to_uint(op_code::constant));
  EXPECT_EQ(f.get_chunk().at(i++), 11);
  EXPECT_EQ(f.get_chunk().at(i++), to_uint(op_code::println));
  EXPECT_EQ(f.get_chunk().at(i++), 0);
  EXPECT_EQ(f.get_chunk().at(i++), to_uint(op_code::constant));
  EXPECT_EQ(f.get_chunk().at(i++), 12);
  EXPECT_EQ(f.get_chunk().at(i++), to_uint(op_code::set_var));
  EXPECT_EQ(f.get_chunk().at(i++), 4);
  EXPECT_EQ(f.get_chunk().at(i++), to_uint(op_code::constant));
  EXPECT_EQ(f.get_chunk().at(i++), 13);
  EXPECT_EQ(f.get_chunk().at(i++), to_uint(op_code::set_var));
  EXPECT_EQ(f.get_chunk().at(i++), 6);
}
TEST(compiler_scenario_outline, parent_feature_constants)
{
  compiler::cucumber cuke(compiler_scenario_outline::script);
  compiler::feature f(&cuke);
  compiler::scenario_outline s(&f);
  s.compile();

  ASSERT_EQ(f.get_chunk().constants_count(), 16);
  EXPECT_EQ(f.get_chunk().constant(0).type(), cuke::value_type::string);
  EXPECT_EQ(f.get_chunk().constant(0).as<std::string>(), "line:2");
  EXPECT_EQ(f.get_chunk().constant(1).type(), cuke::value_type::string);
  EXPECT_EQ(f.get_chunk().constant(1).as<std::string>(),
            "Feature: Hello World");
  EXPECT_EQ(f.get_chunk().constant(2).type(), cuke::value_type::function);
  EXPECT_EQ(f.get_chunk().constant(2).as<function>()->name(), "line:3");
  EXPECT_EQ(f.get_chunk().constant(3).type(), cuke::value_type::string);
  EXPECT_EQ(f.get_chunk().constant(3).as<std::string>(), "line:3");
  EXPECT_EQ(f.get_chunk().constant(4).type(), cuke::value_type::string);
  EXPECT_EQ(f.get_chunk().constant(4).as<std::string>(), "first var");
  EXPECT_EQ(f.get_chunk().constant(5).type(), cuke::value_type::nil);
  EXPECT_EQ(f.get_chunk().constant(6).type(), cuke::value_type::string);
  EXPECT_EQ(f.get_chunk().constant(6).as<std::string>(), "second var");
  EXPECT_EQ(f.get_chunk().constant(7).type(), cuke::value_type::nil);
  EXPECT_EQ(f.get_chunk().constant(8).type(), cuke::value_type::integral);
  EXPECT_EQ(f.get_chunk().constant(8).as<long>(), 1);
  EXPECT_EQ(f.get_chunk().constant(9).type(), cuke::value_type::integral);
  EXPECT_EQ(f.get_chunk().constant(9).as<long>(), 2);
  EXPECT_EQ(f.get_chunk().constant(10).type(), cuke::value_type::string);
  EXPECT_EQ(f.get_chunk().constant(11).type(), cuke::value_type::string);
  EXPECT_EQ(f.get_chunk().constant(12).type(), cuke::value_type::integral);
  EXPECT_EQ(f.get_chunk().constant(12).as<long>(), 3);
  EXPECT_EQ(f.get_chunk().constant(13).type(), cuke::value_type::integral);
  EXPECT_EQ(f.get_chunk().constant(13).as<long>(), 4);
}
