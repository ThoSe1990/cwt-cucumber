#include <gtest/gtest.h>

#include "../src/ast/compiler.hpp"

using namespace cwt::details;

TEST(ast, init_obj)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given A Step with 123
)*";
  compiler compiler(script);
}



TEST(ast_tests, run_simple_scenario)
{
  const char* script = R"*(
  Feature:
)*";
  compiler compiler(script);
  cuke::ast::node head = compiler.compile();
  EXPECT_EQ(head.infos.type, cuke::ast::node_type::feature);
}
