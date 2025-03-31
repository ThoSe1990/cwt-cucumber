#include <gtest/gtest.h>

#include "../src/registry.hpp"
#include "../src/parser.hpp"
#include "ast.hpp"

class run_scenarios_rule : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    cuke::registry().clear();
    cuke::registry().push_step(cuke::internal::step_definition(
        [](const cuke::value_array&, const auto&, const auto&, const auto&)
        { }, "a step"));
    cuke::registry().push_step(cuke::internal::step_definition(
        [](const cuke::value_array&, const auto&, const auto&, const auto&)
        { }, "with {int}"));
  }
};

TEST_F(run_scenarios_rule, rule_1)
{
  const char* script = R"*(
    Feature: a feature 
    Rule: now here comes a rule 
    
    Example: we can use example too 
    * a step 
  )*";

  cuke::parser p;
  p.parse_script(script);
  EXPECT_FALSE(p.error());
  EXPECT_EQ(p.head().feature().scenarios().size(), 1);
}
TEST_F(run_scenarios_rule, rule_2)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario: First Scenario 
    Given a step 

    Rule: now here comes a rule 
    
    Example: we can use example too 
    * a step 
  )*";

  cuke::parser p;
  p.parse_script(script);
  EXPECT_FALSE(p.error());
  ASSERT_EQ(p.head().feature().scenarios().size(), 2);

  auto scenario = static_cast<cuke::ast::scenario_node*>(p.head().feature().scenarios().at(1).get());
  ASSERT_TRUE(scenario->rule().has_value());
  EXPECT_EQ(scenario->rule().value().name(), std::string("now here comes a rule"));
}
TEST_F(run_scenarios_rule, rule_error_w_tags_1)
{
  const char* script = R"*(
    Feature: a feature 

    @tag1
    Rule: now here comes a rule 
    
    Example: we can use example too 
    * a step 
  )*";

  cuke::parser p;
  p.parse_script(script);
  EXPECT_TRUE(p.error());
}
TEST_F(run_scenarios_rule, rule_error_w_tags_2)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario: we can use example too 
    * a step 
    
    @tag1
    Rule: we can't tag rules! 
    
    Example: we can use example too 
    * a step 
  )*";

  cuke::parser p;
  p.parse_script(script);
  EXPECT_TRUE(p.error());
}
TEST_F(run_scenarios_rule, rule_example_w_tags)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario: First Scenario 
    Given a step 

    Rule: now here comes a rule 

    @tag1 
    Example: we can use example too 
    * a step 
  )*";

  cuke::parser p;
  p.parse_script(script);
  EXPECT_FALSE(p.error());
  ASSERT_EQ(p.head().feature().scenarios().size(), 2);

  auto scenario = static_cast<cuke::ast::scenario_node*>(p.head().feature().scenarios().at(1).get());
  ASSERT_TRUE(scenario->rule().has_value());
  EXPECT_EQ(scenario->rule().value().name(), std::string("now here comes a rule"));
  ASSERT_EQ(scenario->tags().size(), 1);
  EXPECT_EQ(scenario->tags().at(0), "@tag1");
}
TEST_F(run_scenarios_rule, rule_w_description)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario: First Scenario 
    Given a step 

    Rule: now here comes a rule 
    and some
    descriptive text here
    Example: we can use example too 
    * a step 
  )*";

  cuke::parser p;
  p.parse_script(script);
  EXPECT_FALSE(p.error());
  ASSERT_EQ(p.head().feature().scenarios().size(), 2);
  auto scenario = static_cast<cuke::ast::scenario_node*>(p.head().feature().scenarios().at(1).get());
  ASSERT_TRUE(scenario->rule().has_value());
  EXPECT_EQ(scenario->rule().value().name(), std::string("now here comes a rule"));
  ASSERT_EQ(scenario->rule().value().description().size(), 2);
  ASSERT_EQ(scenario->rule().value().description().at(0), std::string("and some"));
  ASSERT_EQ(scenario->rule().value().description().at(1), std::string("descriptive text here"));
}
TEST_F(run_scenarios_rule, rule_with_scenario_outline)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario: First Scenario 
    Given a step 

    Rule: now here comes a rule 
    
    Scenario Outline: A scenario outline  
    * a step 
    * with <value>
  
    Examples: 
    | value | 
    | 1 | 
    | 23 | 
  )*";

  cuke::parser p;
  p.parse_script(script);
  EXPECT_FALSE(p.error());
  ASSERT_EQ(p.head().feature().scenarios().size(), 2);

  auto scenario = static_cast<cuke::ast::scenario_outline_node*>(p.head().feature().scenarios().at(1).get());
  ASSERT_TRUE(scenario->rule().has_value());
  EXPECT_EQ(scenario->rule().value().name(), std::string("now here comes a rule"));
}
TEST_F(run_scenarios_rule, multiple_rules)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario: First Scenario 
    Given a step 

    Rule: first rule
    
    some description here ... 

    Scenario Outline: A scenario outline  
    * a step 
    * with <value>
  
    Examples: 
    | value | 
    | 1 | 
    | 23 | 

    Rule: next rule  
    some description here ... 

    Scenario: First Scenario 
    Given a step 
    Rule: and another rule  
    some description here ... 
    Scenario: First Scenario 
    Given a step 
  )*";

  cuke::parser p;
  p.parse_script(script);
  EXPECT_FALSE(p.error());
  ASSERT_EQ(p.head().feature().scenarios().size(), 4);
  
  {
    auto scenario = static_cast<cuke::ast::scenario_node*>(p.head().feature().scenarios().at(0).get());
    ASSERT_FALSE(scenario->rule().has_value());
  }
  {
    auto scenario = static_cast<cuke::ast::scenario_outline_node*>(p.head().feature().scenarios().at(1).get());
    ASSERT_TRUE(scenario->rule().has_value());
    EXPECT_EQ(scenario->rule().value().name(), std::string("first rule"));
  }
  {
    auto scenario = static_cast<cuke::ast::scenario_node*>(p.head().feature().scenarios().at(2).get());
    ASSERT_TRUE(scenario->rule().has_value());
    EXPECT_EQ(scenario->rule().value().name(), std::string("next rule"));
  }
  {
    auto scenario = static_cast<cuke::ast::scenario_node*>(p.head().feature().scenarios().at(3).get());
    ASSERT_TRUE(scenario->rule().has_value());
    EXPECT_EQ(scenario->rule().value().name(), std::string("and another rule"));
  }
  // ASSERT_TRUE(scenario->rule().has_value());
  // EXPECT_EQ(scenario->rule().value().name(), std::string("now here comes a rule"));
}
