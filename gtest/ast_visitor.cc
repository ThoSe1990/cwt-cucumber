#include <gtest/gtest.h>

#include "../src/ast/parser.hpp"

struct scenario_visitor
{
  void visit(const cuke::ast::scenario_node&) {}
  // void visit(const cuke::ast::scenario_outline_node&) {}
  // this is not implemented, its commmented:
};

TEST(ast_iterator, for_each_scenario)
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
