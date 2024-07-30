#include <gtest/gtest.h>

#include "../src/ast/parser.hpp"

class count_scenarios
{
 public:
  void visit(const cuke::ast::feature_node&) {}
  void visit(const cuke::ast::scenario_node&) { ++m_calls; }
  void visit(const cuke::ast::scenario_outline_node&) { ++m_calls; }

  [[nodiscard]] std::size_t calls() const noexcept { return m_calls; }

 private:
  std::size_t m_calls{0};
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
  count_scenarios visitor;
  p.for_each_scenario(visitor);
  EXPECT_EQ(visitor.calls(), 1);
}
TEST(ast_visitor, for_each_scenario_outline)
{
  const char* script = R"*(
  Feature: A Feature

    Scenario Outline: a scenario Outline 
    Given a step 
  )*";
  cuke::parser p;
  p.parse_script(script);
  count_scenarios visitor;
  p.for_each_scenario(visitor);
  EXPECT_EQ(visitor.calls(), 1);
}
TEST(ast_visitor, for_each_scenario_and_scenario_outline)
{
  const char* script = R"*(
  Feature: A Feature
    
    Scenario: a scenario 
    Given a step 

    Scenario Outline: a scenario outline 
    Given a step with <var>
    
    Examples:  
    | var |
    | 1   |
    
  )*";
  cuke::parser p;
  p.parse_script(script);
  ASSERT_FALSE(p.error());
  count_scenarios visitor;
  p.for_each_scenario(visitor);
  EXPECT_EQ(visitor.calls(), 2);
}
