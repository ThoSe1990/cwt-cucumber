#include <gtest/gtest.h>

extern "C" {
    #include "cwt/cucumber.h"   
}

class run_scenarios : public ::testing::Test 
{
protected:
  void SetUp() override
  {
    m_background_called = false;
    open_cucumber();
    cuke_step("background step", background);
    cuke_step("{int} and {int} are equal", test_is_equal);
    cuke_step("{int} is greater than {int}", test_is_greater);
  }

  void TearDown() override 
  {
    close_cucumber();
  }
  static void test_is_equal(int arg_count, cuke_value* args)
  {
    int n1 = cuke_to_int(&args[0]);
    int n2 = cuke_to_int(&args[1]);
    cuke_assert(n1 == n2);
  }
  static void test_is_greater(int arg_count, cuke_value* args)
  {
    int n1 = cuke_to_int(&args[0]);
    int n2 = cuke_to_int(&args[1]);
    cuke_assert(n1 > n2);
  }
  static void background(int arg_count, cuke_value* args)
  {
    m_background_called = true;
  }
  static bool m_background_called;
};
bool run_scenarios::m_background_called;

TEST_F(run_scenarios, simple_scenario_1)
{
const char* script = R"*(
  Feature: some feature 
  with some description 
    Scenario: some scenario
      Then 5 and 5 are equal
)*";

  EXPECT_EQ(CUKE_SUCCESS, run_cuke(script, ""));
}
TEST_F(run_scenarios, simple_scenario_without_whitespaces_1)
{
const char* script = R"*(
Feature: some feature 
with some description 
  Scenario: some scenario
    Then 5 and 5 are equal
)*";

  EXPECT_EQ(CUKE_SUCCESS, run_cuke(script, ""));
}
TEST_F(run_scenarios, simple_scenario_without_whitespaces_2)
{
const char* script = R"*(
Feature: some feature 
with some description 
Scenario: some scenario
Then 5 and 5 are equal
)*";

  EXPECT_EQ(CUKE_SUCCESS, run_cuke(script, ""));
}
TEST_F(run_scenarios, simple_scenario_2)
{
const char* script = R"*(
  Feature: some feature 
  with some description 
    Scenario: some scenario
      Then 5 is greater than 2
)*";

  EXPECT_EQ(CUKE_SUCCESS, run_cuke(script, ""));
}
TEST_F(run_scenarios, simple_scenario_outline_1)
{
const char* script = R"*(
  Feature: some feature 
  with some description 
    Scenario Outline: some scenario
      Then <var1> is greater than <var2>
          
    Examples: some name here 
    | var1 | var2 |
    | 9    | 1    |
)*";

  EXPECT_EQ(CUKE_SUCCESS, run_cuke(script, ""));
}
TEST_F(run_scenarios, simple_scenario_3)
{
const char* script = R"*(
  Feature: some feature 
  with some description 
    Scenario: some scenario
      * 5 and 5 are equal
      And 10 and 10 are equal
)*";

  EXPECT_EQ(CUKE_SUCCESS, run_cuke(script, ""));
}

TEST_F(run_scenarios, simple_scenario_outline)
{
const char* script = R"*(
  Feature: some feature 
  with some description 
    Scenario Outline: some scenario
      * <var1> and <var2> are equal
    
    Examples: some name here 
    and a very nice 
    multiline description
    before the table
    | var1 | var2 |
    | 1    |  1   |

    | 2    |   2 |
)*";

  EXPECT_EQ(CUKE_SUCCESS, run_cuke(script, ""));
}


TEST_F(run_scenarios, simple_scenario_outline_2)
{
const char* script = R"*(
  Feature: some feature 
  with some description 
    Scenario Outline: some scenario
      * <var1> and <var2> are equal
    
    Examples: some name here 
    and a very nice 
    multiline description
    before the table
    | var1 | var2 | var3 |
    | 1    |  1   | 3    |
    | 2    |   2  | 3    |
)*";

  EXPECT_EQ(CUKE_SUCCESS, run_cuke(script, ""));
}


TEST_F(run_scenarios, background_called)
{
const char* script = R"*(
  Feature: some feature 
  
  Background:
    * background step

    Scenario: some scenario
      * 5 and 5 are equal
      And 10 and 10 are equal
)*";

  EXPECT_EQ(CUKE_SUCCESS, run_cuke(script, ""));
  EXPECT_TRUE(m_background_called);
}
TEST_F(run_scenarios, background_not_called)
{
const char* script = R"*(
  Feature: some feature 

    Scenario: some scenario
      * 5 and 5 are equal
      And 10 and 10 are equal
)*";

  EXPECT_EQ(CUKE_SUCCESS, run_cuke(script, ""));
  EXPECT_FALSE(m_background_called);
}
