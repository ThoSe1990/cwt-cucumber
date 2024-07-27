#include <gtest/gtest.h>

#include "../src/ast/parser.hpp"

struct scenario_visitor
{
  void visit(const cuke::ast::scenario_node& s)
  {
    std::cout << "Hello scenario!!" << std::endl;
    std::cout << s.name() << std::endl;
  }
  void visit(const cuke::ast::scenario_outline_node&) {}
};

TEST(ast_visitor, for_each_scenario)
{
  const char* script = R"*(
  Feature: A Feature

    Scenario: a scenario 
    Given a step 
  )*";
  cuke::parser p;
  p.parse_script(script);
  scenario_visitor visitor;
  p.for_each_scenario(visitor);
}
