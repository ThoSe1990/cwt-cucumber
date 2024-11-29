#include <gtest/gtest.h>

#include "../src/parser.hpp"
#include "../src/test_runner.hpp"
#include "../src/test_results.hpp"
#include "../src/asserts.hpp"

class test_results_1 : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    cuke::results::test_results().clear();
    cuke::registry().clear();
    cuke::registry().push_step(cuke::internal::step(
        [](const cuke::value_array&, const auto&, const auto&, const auto&) {},
        "a step"  //,
        // cuke::internal::step::type::step, "some func name","this file", 99
        ));
    cuke::registry().push_step(cuke::internal::step(
        [](const cuke::value_array&, const auto&, const auto&, const auto&)
        { cuke::is_true(false); },
        "this fails"));
  }
};
TEST_F(test_results_1, scenario_passed)
{
  const char* script = R"*(
    Feature: A Feature 
    Scenario: First Scenario 
    Given a step 
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  ASSERT_EQ(cuke::results::test_results().data().size(), 1);
  ASSERT_EQ(cuke::results::features_back().scenarios.size(), 1);
  ASSERT_EQ(cuke::results::scenarios_back().steps.size(), 1);

  const auto& feature = cuke::results::features_back();
  EXPECT_EQ(feature.status, cuke::results::test_status::passed);
  EXPECT_EQ(feature.id, std::string("A Feature"));
  EXPECT_EQ(feature.name, std::string("A Feature"));
  EXPECT_TRUE(feature.description.empty());
  EXPECT_EQ(feature.uri, std::string("<no file>:2"));
  EXPECT_EQ(feature.keyword, std::string("Feature"));

  const auto& scenario = cuke::results::scenarios_back();
  EXPECT_EQ(scenario.status, cuke::results::test_status::passed);
  EXPECT_EQ(scenario.id, std::string("A Feature;First Scenario"));
  EXPECT_EQ(scenario.name, std::string("First Scenario"));
  EXPECT_EQ(scenario.keyword, std::string("Scenario"));
  EXPECT_EQ(scenario.file, std::string("<no file>"));
  EXPECT_EQ(scenario.line, 3);
  EXPECT_EQ(scenario.tags.size(), 0);
  EXPECT_TRUE(scenario.description.empty());

  const auto& step = cuke::results::steps_back();
  EXPECT_EQ(step.status, cuke::results::test_status::passed);
  // NOTE: source location is set in step constructor in SetUp() function of
  // this suite
  // EXPECT_EQ(step.source_location, std::format("this file:{}", 99));
  EXPECT_EQ(step.keyword, std::string("Given"));
  EXPECT_EQ(step.name, std::string("a step"));
  EXPECT_EQ(step.id, std::string("A Feature;First Scenario;a step"));
  EXPECT_EQ(step.line, 4);
  EXPECT_EQ(step.file, std::string("<no file>"));
  EXPECT_TRUE(step.error_msg.empty());
  EXPECT_TRUE(step.doc_string.empty());
  EXPECT_EQ(step.table.row_count(), 0);
}
TEST_F(test_results_1, german_keys)
{
  const char* script = R"*(
    #language: de
    Funktionalität: A Feature 
    Szenario: First Scenario 
    Gegeben sei a step
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  ASSERT_EQ(cuke::results::test_results().data().size(), 1);
  ASSERT_EQ(cuke::results::features_back().scenarios.size(), 1);
  ASSERT_EQ(cuke::results::scenarios_back().steps.size(), 1);

  const auto& feature = cuke::results::features_back();
  EXPECT_EQ(feature.keyword, std::string("Funktionalität"));

  const auto& scenario = cuke::results::scenarios_back();
  EXPECT_EQ(scenario.keyword, std::string("Szenario"));

  const auto& step = cuke::results::steps_back();
  EXPECT_EQ(step.keyword, std::string("Gegeben sei"));
}

TEST_F(test_results_1, scenario_fails_1)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario: First Scenario 
    Given this fails 
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  ASSERT_EQ(cuke::results::test_results().data().size(), 1);
  EXPECT_EQ(cuke::results::features_back().status,
            cuke::results::test_status::failed);
  EXPECT_EQ(cuke::results::scenarios_back().status,
            cuke::results::test_status::failed);
  EXPECT_EQ(cuke::results::steps_back().status,
            cuke::results::test_status::failed);
  EXPECT_EQ(cuke::results::steps_back().error_msg,
            std::string("Expected given condition true, but its false:"));
}
TEST_F(test_results_1, scenario_fails_2)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario: First Scenario 
    Given this fails 
    And this is skipped
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  ASSERT_EQ(cuke::results::test_results().data().size(), 1);
  EXPECT_EQ(cuke::results::features_back().status,
            cuke::results::test_status::failed);
  EXPECT_EQ(cuke::results::scenarios_back().status,
            cuke::results::test_status::failed);
  EXPECT_EQ(cuke::results::steps_back().status,
            cuke::results::test_status::skipped);
}

TEST_F(test_results_1, undefined_step_1)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario: First Scenario 
    Given a step 
    And an undefined step ... 
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  ASSERT_EQ(cuke::results::test_results().data().size(), 1);
  EXPECT_EQ(cuke::results::features_back().status,
            cuke::results::test_status::failed);
  EXPECT_EQ(cuke::results::scenarios_back().status,
            cuke::results::test_status::failed);
  EXPECT_EQ(cuke::results::steps_back().status,
            cuke::results::test_status::undefined);
}
TEST_F(test_results_1, undefined_step_2)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario: First Scenario 
    Given this fails 
    And an undefined step ...
    Then this is skipped
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  ASSERT_EQ(cuke::results::test_results().data().size(), 1);
  EXPECT_EQ(cuke::results::features_back().status,
            cuke::results::test_status::failed);
  EXPECT_EQ(cuke::results::scenarios_back().status,
            cuke::results::test_status::failed);
  EXPECT_EQ(cuke::results::steps_back().status,
            cuke::results::test_status::skipped);
}

class test_results_2 : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    cuke::results::test_results().clear();
    cuke::registry().clear();
    cuke::registry().push_step(cuke::internal::step(
        [](const cuke::value_array&, const auto&, const auto&, const auto&) {},
        "a step with {int}"));
  }
};

TEST_F(test_results_2, scenario_outline)
{
  const char* script = R"*(
    Feature: A Feature 
    Scenario Outline: First Scenario 
    Given a step with <var>
  
    Examples: 
    | var | 
    | 123 |
    | 999 |
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  const auto& scenarios = cuke::results::test_results().back().scenarios;
  ASSERT_EQ(scenarios.size(), 2);

  const auto& s1 = scenarios.at(0);
  ASSERT_EQ(s1.steps.size(), 1);
  EXPECT_EQ(s1.steps.at(0).name, std::string("a step with 123"));

  const auto& s2 = scenarios.at(1);
  ASSERT_EQ(s2.steps.size(), 1);
  EXPECT_EQ(s2.steps.at(0).name, std::string("a step with 999"));
}
TEST_F(test_results_2, scenario_w_tags)
{
  const char* script = R"*(
    @tag1
    Feature: A Feature 
    @tag2
    Scenario: First Scenario 
    Given a step with 123
    
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  const auto& scenarios = cuke::results::test_results().back().scenarios;
  ASSERT_EQ(scenarios.size(), 1);
  ASSERT_EQ(scenarios.at(0).tags.size(), 2);
  EXPECT_EQ(scenarios.at(0).tags.at(0), std::string("@tag1"));
  EXPECT_EQ(scenarios.at(0).tags.at(1), std::string("@tag2"));
}

TEST_F(test_results_2, scenario_outline_w_tags)
{
  const char* script = R"*(
    @tag1
    Feature: A Feature 
    @tag2
    Scenario Outline: First Scenario 
    Given a step with <var>
    
    @tag3 
    Examples: 
    | var | 
    | 123 |
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  const auto& scenarios = cuke::results::test_results().back().scenarios;
  ASSERT_EQ(scenarios.size(), 1);
  ASSERT_EQ(scenarios.at(0).tags.size(), 3);
  EXPECT_EQ(scenarios.at(0).tags.at(0), std::string("@tag1"));
  EXPECT_EQ(scenarios.at(0).tags.at(1), std::string("@tag2"));
  EXPECT_EQ(scenarios.at(0).tags.at(2), std::string("@tag3"));
}
TEST_F(test_results_2, with_description)
{
  const char* script = R"*(
    Feature: A Feature 
    some feature 
    description given 
    Scenario: First Scenario 
    some scenario 
    description too
    Given a step with 123  
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(cuke::results::test_results().back().description,
            std::string("some feature description given"));

  const auto& scenarios = cuke::results::test_results().back().scenarios;

  ASSERT_EQ(scenarios.size(), 1);
  EXPECT_EQ(scenarios.at(0).description,
            std::string("some scenario description too"));
}
TEST_F(test_results_2, step_w_doc_string)
{
  const char* script = R"*(
    Feature: A Feature 
    Scenario: First Scenario 
    Given a step with 123 
    """
    some 
    doc string 
    appended
    """
    
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  const auto& scenarios = cuke::results::test_results().back().scenarios;
  ASSERT_EQ(scenarios.size(), 1);
  EXPECT_EQ(scenarios.at(0).steps.at(0).doc_string,
            std::string("some doc string appended"));
}
TEST_F(test_results_2, step_w_table)
{
  const char* script = R"*(
    Feature: A Feature 
    Scenario: First Scenario 
    Given a step with 123 
    | a | table |
    | 1 | 123   |
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  const auto& scenarios = cuke::results::test_results().back().scenarios;
  ASSERT_EQ(scenarios.size(), 1);
  const cuke::table& t = scenarios.at(0).steps.at(0).table;
  EXPECT_EQ(t[0][0].to_string(), std::string("a"));
  EXPECT_EQ(t[0][1].to_string(), std::string("table"));
  EXPECT_EQ(t[1][0].as<int>(), 1);
  EXPECT_EQ(t[1][1].as<int>(), 123);
}
TEST_F(test_results_2, germen_keywords)
{
  const char* script = R"*(
    #language: de
    Funktion: A Feature 
    u
    Given a step with 123 
    | a | table |
    | 1 | 123   |
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);
}
TEST_F(test_results_2, error_msg_undefined_step)
{
  const char* script = R"*(
    Feature: A Feature 
    Scenario: A Scenario
    Given This does not exist
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  const auto& scenarios = cuke::results::test_results().back().scenarios;
  ASSERT_EQ(scenarios.size(), 1);
  ASSERT_EQ(scenarios.at(0).steps.size(), 1);
  EXPECT_EQ(scenarios.at(0).steps.at(0).error_msg,
            std::string("Undefined step"));
}
