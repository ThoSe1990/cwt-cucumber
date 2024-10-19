#include <gtest/gtest.h>

#include "../src/test_runner.hpp"
#include "../src/parser.hpp"

class run_scenarios_tags : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    call_count = 0;
    test_value = 0;
    cuke::registry().clear();
    cuke::registry().push_step(cuke::internal::step(
        [](const cuke::value_array&) { run_scenarios_tags::call_count++; },
        "a step"));
    cuke::registry().push_step(cuke::internal::step(
        [](const cuke::value_array& values) { 
        run_scenarios_tags::test_value = values.at(0).as<std::size_t>();
        run_scenarios_tags::call_count++; },
        "a step with {int}"));
  }

  static std::size_t call_count;
  static std::size_t test_value;
};
std::size_t run_scenarios_tags::call_count = 0;
std::size_t run_scenarios_tags::test_value = 0;

TEST_F(run_scenarios_tags, tagged_feature)
{
  const char* script = R"*(
    @tag1
    Feature: a feature 
    Scenario: a scenario 
    Given a step 
  )*";
  cuke::parser p;
  p.parse_script(script);

  cuke::internal::tag_expression tags("@tag1");
  cuke::test_runner runner({}, &tags);

  p.for_each_scenario(runner);
  EXPECT_EQ(run_scenarios_tags::call_count, 1);
}
TEST_F(run_scenarios_tags, tagged_scenario)
{
  const char* script = R"*(
    Feature: a feature 
    
    @tag1
    Scenario: a scenario 
    Given a step 
  )*";
  cuke::parser p;
  p.parse_script(script);

  cuke::internal::tag_expression tags("@tag1");
  cuke::test_runner runner({}, &tags);

  p.for_each_scenario(runner);
  EXPECT_EQ(run_scenarios_tags::call_count, 1);
}
 
TEST_F(run_scenarios_tags, tagged_scenarios_1)
{
  const char* script = R"*(
    @tag1
    Feature: a feature 
    
    @tag2
    Scenario: a scenario 
    Given a step 

    @tag3
    Scenario: a scenario 
    Given a step 
  )*";
  cuke::parser p;
  p.parse_script(script);

  cuke::internal::tag_expression tags("not @tag1");
  cuke::test_runner runner({}, &tags);

  p.for_each_scenario(runner);
  EXPECT_EQ(run_scenarios_tags::call_count, 0);
}
 
TEST_F(run_scenarios_tags, tagged_scenarios_2)
{
  const char* script = R"*(
    @tag1
    Feature: a feature 
    
    @tag2
    Scenario: a scenario 
    Given a step 

    @tag3
    Scenario: a scenario 
    Given a step 
  )*";
  cuke::parser p;
  p.parse_script(script);

  cuke::internal::tag_expression tags("@tag1");
  cuke::test_runner runner({}, &tags);

  p.for_each_scenario(runner);
  EXPECT_EQ(run_scenarios_tags::call_count, 2);
}
TEST_F(run_scenarios_tags, tagged_scenarios_3)
{
  const char* script = R"*(
    @tag1
    Feature: a feature 
    
    @tag2
    Scenario: a scenario 
    Given a step 

    @tag3
    Scenario: a scenario 
    Given a step 
  )*";
  cuke::parser p;
  p.parse_script(script);

  cuke::internal::tag_expression tags("@tag1 or @tag2 and not @tag3");
  cuke::test_runner runner({}, &tags);

  p.for_each_scenario(runner);
  EXPECT_EQ(run_scenarios_tags::call_count, 1);
}
TEST_F(run_scenarios_tags, tagged_scenario_outline_1)
{
  const char* script = R"*(
    Feature: a feature 
   
    @tag1
    Scenario Outline: a scenario outline
    Given a step with <value> 
    
    Examples: 
    | value | 
    | 1     | 
    Examples: 
    | value | 
    | 1     | 
  )*";
  cuke::parser p;
  p.parse_script(script);

  cuke::internal::tag_expression tags("@tag1");
  cuke::test_runner runner({}, &tags);

  p.for_each_scenario(runner);
  EXPECT_EQ(run_scenarios_tags::call_count, 2);
}
TEST_F(run_scenarios_tags, tagged_scenario_outline_2)
{
  const char* script = R"*(
    Feature: a feature 
   
    @tag1
    Scenario Outline: a scenario outline
    Given a step with <value> 
    
    Examples: 
    | value | 
    | 1     | 
    @tag2 
    Examples: 
    | value | 
    | 99    | 
  )*";
  cuke::parser p;
  p.parse_script(script);

  cuke::internal::tag_expression tags("@tag1 and @tag2");
  cuke::test_runner runner({}, &tags);

  p.for_each_scenario(runner);
  EXPECT_EQ(run_scenarios_tags::call_count, 1);
  EXPECT_EQ(run_scenarios_tags::test_value, 99);
}
TEST_F(run_scenarios_tags, tagged_scenario_outline_3)
{
  const char* script = R"*(
    @tag1
    Feature: a feature 
   
    @tag2
    Scenario Outline: a scenario outline
    Given a step with <value> 
     
    Examples: 
    | value | 
    | 99    | 
    @tag3 
    Examples: 
    | value | 
    | 1     | 
  )*";
  cuke::parser p;
  p.parse_script(script);

  cuke::internal::tag_expression tags("@tag1 and @tag2");
  cuke::test_runner runner({}, &tags);

  p.for_each_scenario(runner);
  EXPECT_EQ(run_scenarios_tags::call_count, 2);
}
TEST_F(run_scenarios_tags, tagged_scenario_outline_4)
{
  const char* script = R"*(
    @tag1
    Feature: a feature 
   
    @tag2
    Scenario Outline: a scenario outline
    Given a step with <value> 
     
    Examples: 
    | value | 
    | 1     | 
    @tag3 
    Examples: 
    | value | 
    | 99    | 
  )*";
  cuke::parser p;
  p.parse_script(script);

  cuke::internal::tag_expression tags("@tag1 and @tag2 and @tag3");
  cuke::test_runner runner({}, &tags);

  p.for_each_scenario(runner);
  EXPECT_EQ(run_scenarios_tags::call_count, 1);
  EXPECT_EQ(run_scenarios_tags::test_value, 99);
}
TEST_F(run_scenarios_tags, tagged_scenario_and_scenario_outline_1)
{
  const char* script = R"*(
    @tag1
    Feature: a feature 
   
    Scenario: a scenario 
    Given a step 

    @tag2
    Scenario Outline: a scenario outline
    Given a step with <value> 
     
    Examples: 
    | value | 
    | 1     | 
    @tag3 
    Examples: 
    | value | 
    | 99    | 
  )*";
  cuke::parser p;
  p.parse_script(script);

  cuke::internal::tag_expression tags("@tag1 and not @tag2");
  cuke::test_runner runner({}, &tags);

  p.for_each_scenario(runner);
  EXPECT_EQ(run_scenarios_tags::call_count, 1);
  EXPECT_EQ(run_scenarios_tags::test_value, 0);
}
TEST_F(run_scenarios_tags, tagged_scenario_and_scenario_outline_2)
{
  const char* script = R"*(
    @tag1
    Feature: a feature 
   
    Scenario: a scenario 
    Given a step 

    @tag2
    Scenario Outline: a scenario outline
    Given a step with <value> 
     
    Examples: 
    | value | 
    | 1     | 
    @tag3 
    Examples: 
    | value | 
    | 99    | 
  )*";
  cuke::parser p;
  p.parse_script(script);

  cuke::internal::tag_expression tags("@tag1 and @tag3");
  cuke::test_runner runner({}, &tags);

  p.for_each_scenario(runner);
  EXPECT_EQ(run_scenarios_tags::call_count, 1);
  EXPECT_EQ(run_scenarios_tags::test_value, 99);
}
