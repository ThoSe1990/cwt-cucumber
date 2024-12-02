#include <gtest/gtest.h>

#include "../src/asserts.hpp"
#include "../src/registry.hpp"
#include "../src/parser.hpp"
#include "../src/test_runner.hpp"
#include "../src/test_results.hpp"

#include "../src/report.hpp"

class report : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    cuke::registry().clear();
    cuke::results::test_results().clear();

    cuke::registry().push_step(
        cuke::internal::step([](const cuke::value_array& values, const auto&,
                                const auto&, const auto&) {},
                             "a step with {int} and {string}"));
    cuke::registry().push_step(
        cuke::internal::step([](const cuke::value_array& values, const auto&,
                                const auto&, const auto&) {},
                             "another step with {word} and {}"));
    cuke::registry().push_step(cuke::internal::step(
        [](const cuke::value_array& values, const auto&, const auto&,
           const auto&) { cuke::is_true(false); },
        "this fails"));
  }
};

TEST_F(report, json_single_scenario)
{
  const std::string expected = R"([
  {
    "description": "",
    "elements": [
      {
        "description": "",
        "id": "a feature;First Scenario",
        "keyword": "Scenario",
        "line": 3,
        "name": "First Scenario",
        "steps": [
          {
            "arguments": [],
            "keyword": "Given",
            "line": 4,
            "match": {
              "location": ":0"
            },
            "name": "a step with 123 and \"hello world\"",
            "results": {
              "status": "passed"
            }
          }
        ],
        "tags": [],
        "type": "First Scenario"
      }
    ],
    "id": "a feature",
    "keyword": "Feature",
    "line": 2,
    "name": "a feature",
    "tags": [],
    "uri": "<no file>"
  }
])";

  const char* script = R"*(
    Feature: a feature 
    Scenario: First Scenario 
    Given a step with 123 and "hello world"
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(expected, cuke::report::as_json());
}
TEST_F(report, json_single_scenario_w_tags)
{
  const std::string expected = R"([
  {
    "description": "",
    "elements": [
      {
        "description": "",
        "id": "a feature;First Scenario",
        "keyword": "Scenario",
        "line": 5,
        "name": "First Scenario",
        "steps": [
          {
            "arguments": [],
            "keyword": "Given",
            "line": 6,
            "match": {
              "location": ":0"
            },
            "name": "a step with 123 and \"hello world\"",
            "results": {
              "status": "passed"
            }
          }
        ],
        "tags": [
          "@tag1",
          "@tag2"
        ],
        "type": "First Scenario"
      }
    ],
    "id": "a feature",
    "keyword": "Feature",
    "line": 3,
    "name": "a feature",
    "tags": [
      "@tag1"
    ],
    "uri": "<no file>"
  }
])";

  const char* script = R"*(
    @tag1
    Feature: a feature 
    @tag2
    Scenario: First Scenario 
    Given a step with 123 and "hello world"
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(expected, cuke::report::as_json());
}
TEST_F(report, json_scenario_outline)
{
  const std::string expected = R"([
  {
    "description": "",
    "elements": [
      {
        "description": "",
        "id": "A Feature;Scenario Outline",
        "keyword": "Scenario Outline",
        "line": 6,
        "name": "Scenario Outline",
        "steps": [
          {
            "arguments": [],
            "keyword": "Given",
            "line": 4,
            "match": {
              "location": ":0"
            },
            "name": "a step with 123 and \"hi\"",
            "results": {
              "status": "passed"
            }
          }
        ],
        "tags": [],
        "type": "Scenario Outline"
      },
      {
        "description": "",
        "id": "A Feature;Scenario Outline",
        "keyword": "Scenario Outline",
        "line": 7,
        "name": "Scenario Outline",
        "steps": [
          {
            "arguments": [],
            "keyword": "Given",
            "line": 4,
            "match": {
              "location": ":0"
            },
            "name": "a step with 999 and \"xx\"",
            "results": {
              "status": "passed"
            }
          }
        ],
        "tags": [],
        "type": "Scenario Outline"
      }
    ],
    "id": "A Feature",
    "keyword": "Feature",
    "line": 2,
    "name": "A Feature",
    "tags": [],
    "uri": "<no file>"
  }
])";

  const char* script = R"*(
    Feature: A Feature 
    Scenario Outline: Scenario Outline
    Given a step with <var1> and <var2> 
    Examples: 
    | var1 | var2 | 
    | 123  | "hi" |
    | 999  | "xx" |
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(expected, cuke::report::as_json());
}
TEST_F(report, failing_scenario)
{
  const std::string expected = R"([
  {
    "description": "",
    "elements": [
      {
        "description": "",
        "id": "a feature;First Scenario",
        "keyword": "Scenario",
        "line": 3,
        "name": "First Scenario",
        "steps": [
          {
            "arguments": [],
            "keyword": "Given",
            "line": 4,
            "match": {
              "location": ":0"
            },
            "name": "a step with 123 and \"hello world\"",
            "results": {
              "status": "passed"
            }
          },
          {
            "arguments": [],
            "keyword": "And",
            "line": 5,
            "match": {
              "location": ":0"
            },
            "name": "this fails",
            "results": {
              "error_message": "Expected given condition true, but its false:",
              "status": "failed"
            }
          }
        ],
        "tags": [],
        "type": "First Scenario"
      }
    ],
    "id": "a feature",
    "keyword": "Feature",
    "line": 2,
    "name": "a feature",
    "tags": [],
    "uri": "<no file>"
  }
])";

  const char* script = R"*(
    Feature: a feature 
    Scenario: First Scenario 
    Given a step with 123 and "hello world"
    And this fails 
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(expected, cuke::report::as_json());
}
TEST_F(report, undefined_step)
{
  const std::string expected = R"([
  {
    "description": "",
    "elements": [
      {
        "description": "",
        "id": "a feature;First Scenario",
        "keyword": "Scenario",
        "line": 3,
        "name": "First Scenario",
        "steps": [
          {
            "arguments": [],
            "keyword": "Given",
            "line": 4,
            "match": {
              "location": ":0"
            },
            "name": "a step with 123 and \"hello world\"",
            "results": {
              "status": "passed"
            }
          },
          {
            "arguments": [],
            "keyword": "And",
            "line": 5,
            "match": {
              "location": ""
            },
            "name": "here is an undefined step",
            "results": {
              "error_message": "Undefined step",
              "status": "undefined"
            }
          }
        ],
        "tags": [],
        "type": "First Scenario"
      }
    ],
    "id": "a feature",
    "keyword": "Feature",
    "line": 2,
    "name": "a feature",
    "tags": [],
    "uri": "<no file>"
  }
])";

  const char* script = R"*(
    Feature: a feature 
    Scenario: First Scenario 
    Given a step with 123 and "hello world"
    And here is an undefined step 
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(expected, cuke::report::as_json());
}
