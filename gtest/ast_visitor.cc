#include <gtest/gtest.h>

#include "../src/parser.hpp"

class count_scenarios
{
 public:
  void visit(const cuke::ast::feature_node&) {}
  void visit(const cuke::ast::scenario_node&) { ++m_calls; }
  void visit(const cuke::ast::scenario_outline_node&) { ++m_calls; }
  void setup()
  { 
    ASSERT_FALSE(m_setup_called); 
    m_setup_called = true; 
  }
  void teardown()
  { 
    ASSERT_FALSE(m_teardown_called); 
    m_teardown_called = true; 
  }

  [[nodiscard]] std::size_t calls() const noexcept { return m_calls; }
  [[nodiscard]] std::size_t setup_called() const noexcept { return m_setup_called; }
  [[nodiscard]] std::size_t teardown_called() const noexcept { return m_teardown_called; }

 private:
  std::size_t m_calls{0};
  bool m_setup_called = false;
  bool m_teardown_called = false;
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
  visitor.setup();
  p.for_each_scenario(visitor);
  visitor.teardown();
  EXPECT_EQ(visitor.calls(), 1);
  EXPECT_TRUE(visitor.setup_called());
  EXPECT_TRUE(visitor.teardown_called());
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
