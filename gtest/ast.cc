#include <gtest/gtest.h>

#include "../src/ast/ast.hpp"


TEST(ast, init_obj)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given A Step with 123
)*";
  cuke::ast::parser p(script);
}

TEST(ast_tests, run_simple_scenario)
{
  const char* script = R"*(
  Feature:
)*";
  cuke::ast::parser p(script);
  cuke::ast::node head = p.compile();
  EXPECT_EQ(head.infos.type, cuke::ast::node_type::feature);
}
