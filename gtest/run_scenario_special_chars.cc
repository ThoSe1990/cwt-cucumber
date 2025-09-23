#include <gtest/gtest.h>

#include "../src/parser.hpp"
#include "../src/test_runner.hpp"
#include "../src/test_results.hpp"

using namespace cuke::internal;

class run_scenario_special_chars : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    cuke::registry().push_step(step_definition(
        [](const cuke::value_array& values, const auto&, const auto&,
           const auto&)
        {
          run_scenario_special_chars::given_word = values[0].as<std::string>();
          run_scenario_special_chars::given_anonymous =
              values[1].as<std::string>();
        },
        "A {word} and {}"));
    cuke::registry().push_step(step_definition(
        [](const cuke::value_array& values, const auto&, const auto&,
           const auto&)
        {
          EXPECT_EQ(run_scenario_special_chars::given_word,
                    values[0].as<std::string>());
          EXPECT_EQ(run_scenario_special_chars::given_anonymous,
                    values[1].as<std::string>());
        },
        "They will match \"{word}\" and \"{}\""));
  }

  void TearDown() override
  {
    cuke::registry().clear();
    cuke::results::test_results().clear();
  }

  static std::string given_word;
  static std::string given_anonymous;
};

std::string run_scenario_special_chars::given_word = "";
std::string run_scenario_special_chars::given_anonymous = "";

TEST_F(run_scenario_special_chars, outline_w_special_chars)
{
  const char* script = R"*(
    Feature: a feature 
      Scenario Outline: check for some special chars
        When A <word> and <anonymous>
        Then They will match "<expected word>" and "<expected anonymous>"
        
        Examples:
          | word      | anonymous     | expected word | expected anonymous |
          | ?         | ?             | ?             | ?                  | 
          | *         | *             | *             | *                  | 
          | .*        | .*            | .*            | .*                 | 
          | [         | [             | [             | [                  | 
          | []        | []            | []            | []                 | 
          | (         | (             | (             | (                  | 
          | ()        | ()            | ()            | ()                 | 
          | <         | <             | <             | <                  | 
          | <>        | <>            | <>            | <>                 | 
  )*";


  cuke::parser p;
  p.parse_script(script);
  ASSERT_FALSE(p.error());

  cuke::test_runner runner;
  p.for_each_scenario(runner);
}
