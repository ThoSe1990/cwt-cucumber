#include <gtest/gtest.h>

#include "../src/cwt/compiler/scenario_outline.hpp"

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

  EXPECT_EQ(s.get_chunk().at(0), to_uint(op_code::constant));
  EXPECT_EQ(s.get_chunk().at(1), 1);
  EXPECT_EQ(s.get_chunk().at(2), to_uint(op_code::print));
  EXPECT_EQ(s.get_chunk().at(3), 0);
  EXPECT_EQ(s.get_chunk().at(4), to_uint(op_code::constant));
  EXPECT_EQ(s.get_chunk().at(5), 0);
  EXPECT_EQ(s.get_chunk().at(6), to_uint(op_code::println));
  EXPECT_EQ(s.get_chunk().at(7), 5);
  EXPECT_EQ(s.get_chunk().at(8), to_uint(op_code::init_scenario));
  EXPECT_EQ(s.get_chunk().at(9), to_uint(op_code::get_var));
  EXPECT_EQ(s.get_chunk().at(10), 3);
  EXPECT_EQ(s.get_chunk().at(11), to_uint(op_code::get_var));
  EXPECT_EQ(s.get_chunk().at(12), 4);
  EXPECT_EQ(s.get_chunk().at(13), to_uint(op_code::jump_if_failed));
  EXPECT_EQ(s.get_chunk().at(14), 21);
  EXPECT_EQ(s.get_chunk().at(15), to_uint(op_code::hook));
  EXPECT_EQ(s.get_chunk().at(16), to_uint(hook_type::before_step));
  EXPECT_EQ(s.get_chunk().at(17), to_uint(op_code::call_step));
  EXPECT_EQ(s.get_chunk().at(18), 5);
  EXPECT_EQ(s.get_chunk().at(19), to_uint(op_code::hook));
  EXPECT_EQ(s.get_chunk().at(20), to_uint(hook_type::after_step));
  EXPECT_EQ(s.get_chunk().at(21), to_uint(op_code::constant));
  EXPECT_EQ(s.get_chunk().at(22), 5);
  EXPECT_EQ(s.get_chunk().at(23), to_uint(op_code::constant));
  EXPECT_EQ(s.get_chunk().at(24), 2);
  EXPECT_EQ(s.get_chunk().at(25), to_uint(op_code::step_result));
  EXPECT_EQ(s.get_chunk().at(26), to_uint(op_code::func_return));
}

TEST(compiler_scenario_outline, scenarios_constants)
{
  compiler::cucumber cuke(compiler_scenario_outline::script);
  compiler::feature f(&cuke);
  compiler::scenario_outline s(&f);
  s.compile();
  ASSERT_EQ(s.get_chunk().constants_count(), 6);
  EXPECT_EQ(s.get_chunk().constant(0).type(), value_type::string);
  EXPECT_EQ(s.get_chunk().constant(0).as<std::string>(), ":3");
  EXPECT_EQ(s.get_chunk().constant(1).type(), value_type::string);
  EXPECT_EQ(s.get_chunk().constant(1).as<std::string>(), "Scenario Outline: A Scenario");
  EXPECT_EQ(s.get_chunk().constant(2).type(), value_type::string);
  EXPECT_EQ(s.get_chunk().constant(2).as<std::string>(), ":4");
  EXPECT_EQ(s.get_chunk().constant(3).type(), value_type::string);
  EXPECT_EQ(s.get_chunk().constant(3).as<std::string>(), "first var");
  EXPECT_EQ(s.get_chunk().constant(4).type(), value_type::string);
  EXPECT_EQ(s.get_chunk().constant(4).as<std::string>(), "second var");
  EXPECT_EQ(s.get_chunk().constant(5).type(), value_type::string);
  EXPECT_EQ(s.get_chunk().constant(5).as<std::string>(), "Given A Step with <first var> and <second var>");
}

TEST(compiler_scenario_outline, parent_feature_code)
{
  compiler::cucumber cuke(compiler_scenario_outline::script);
  compiler::feature f(&cuke);
  compiler::scenario_outline s(&f);
  s.compile();

  ASSERT_EQ(f.get_chunk().size(), 58);


  EXPECT_EQ(f.get_chunk().at(12), to_uint(op_code::constant));
  EXPECT_EQ(f.get_chunk().at(13), 5);
  EXPECT_EQ(f.get_chunk().at(14), to_uint(op_code::define_var));
  EXPECT_EQ(f.get_chunk().at(15), 4);
  EXPECT_EQ(f.get_chunk().at(16), to_uint(op_code::constant));
  EXPECT_EQ(f.get_chunk().at(17), 7);
  EXPECT_EQ(f.get_chunk().at(18), to_uint(op_code::define_var));
  EXPECT_EQ(f.get_chunk().at(19), 6);
  EXPECT_EQ(f.get_chunk().at(20), to_uint(op_code::constant));
  EXPECT_EQ(f.get_chunk().at(21), 8);
  EXPECT_EQ(f.get_chunk().at(22), to_uint(op_code::set_var));
  EXPECT_EQ(f.get_chunk().at(23), 4);
  EXPECT_EQ(f.get_chunk().at(24), to_uint(op_code::constant));
  EXPECT_EQ(f.get_chunk().at(25), 9);
  EXPECT_EQ(f.get_chunk().at(26), to_uint(op_code::set_var));
  EXPECT_EQ(f.get_chunk().at(27), 6);
  EXPECT_EQ(f.get_chunk().at(28), to_uint(op_code::hook));
  EXPECT_EQ(f.get_chunk().at(29), to_uint(hook_type::before));
  EXPECT_EQ(f.get_chunk().at(30), 0);
  EXPECT_EQ(f.get_chunk().at(31), to_uint(op_code::get_var));
  EXPECT_EQ(f.get_chunk().at(32), 3);
  EXPECT_EQ(f.get_chunk().at(33), to_uint(op_code::call));
  EXPECT_EQ(f.get_chunk().at(34), 0);
  EXPECT_EQ(f.get_chunk().at(35), to_uint(op_code::hook));
  EXPECT_EQ(f.get_chunk().at(36), to_uint(hook_type::after));
  EXPECT_EQ(f.get_chunk().at(37), 0);
  EXPECT_EQ(f.get_chunk().at(38), to_uint(op_code::scenario_result));

  EXPECT_EQ(f.get_chunk().at(39), to_uint(op_code::constant));
  EXPECT_EQ(f.get_chunk().at(40), 10);
  EXPECT_EQ(f.get_chunk().at(41), to_uint(op_code::set_var));
  EXPECT_EQ(f.get_chunk().at(42), 4);
  EXPECT_EQ(f.get_chunk().at(43), to_uint(op_code::constant));
  EXPECT_EQ(f.get_chunk().at(44), 11);
  EXPECT_EQ(f.get_chunk().at(45), to_uint(op_code::set_var));
  EXPECT_EQ(f.get_chunk().at(46), 6);
}
TEST(compiler_scenario_outline, parent_feature_constants)
{
  compiler::cucumber cuke(compiler_scenario_outline::script);
  compiler::feature f(&cuke);
  compiler::scenario_outline s(&f);
  s.compile();

  ASSERT_EQ(f.get_chunk().constants_count(), 12);
  EXPECT_EQ(f.get_chunk().constant(0).type(), value_type::string);
  EXPECT_EQ(f.get_chunk().constant(0).as<std::string>(), ":2");
  EXPECT_EQ(f.get_chunk().constant(1).type(), value_type::string);
  EXPECT_EQ(f.get_chunk().constant(1).as<std::string>(), "Feature: Hello World");
  EXPECT_EQ(f.get_chunk().constant(2).type(), value_type::function);
  EXPECT_EQ(f.get_chunk().constant(2).as<function>()->name(), ":3");
  EXPECT_EQ(f.get_chunk().constant(3).type(), value_type::string);
  EXPECT_EQ(f.get_chunk().constant(3).as<std::string>(), ":3");
  EXPECT_EQ(f.get_chunk().constant(4).type(), value_type::string);
  EXPECT_EQ(f.get_chunk().constant(4).as<std::string>(), "first var");
  EXPECT_EQ(f.get_chunk().constant(5).type(), value_type::nil);
  EXPECT_EQ(f.get_chunk().constant(6).type(), value_type::string);
  EXPECT_EQ(f.get_chunk().constant(6).as<std::string>(), "second var");
  EXPECT_EQ(f.get_chunk().constant(7).type(), value_type::nil);
  EXPECT_EQ(f.get_chunk().constant(8).type(), value_type::integral);
  EXPECT_EQ(f.get_chunk().constant(8).as<long>(), 1);
  EXPECT_EQ(f.get_chunk().constant(9).type(), value_type::integral);
  EXPECT_EQ(f.get_chunk().constant(9).as<long>(), 2);
  EXPECT_EQ(f.get_chunk().constant(10).type(), value_type::integral);
  EXPECT_EQ(f.get_chunk().constant(10).as<long>(), 3);
  EXPECT_EQ(f.get_chunk().constant(11).type(), value_type::integral);
  EXPECT_EQ(f.get_chunk().constant(11).as<long>(), 4);
}