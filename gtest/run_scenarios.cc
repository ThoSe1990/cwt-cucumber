#include <gtest/gtest.h>

extern "C" {
    #include "cwt/cucumber.h"   
}

class run_scenarios : public ::testing::Test 
{
protected:
  void SetUp() override
  {
    open_cucumber();
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
};

TEST_F(run_scenarios, simple_scenario_1)
{
const char* script = R"*(
  Feature: some feature 
  with some description 
    Scenario: some scenario
      Then 5 and 5 are equal
)*";

  EXPECT_EQ(CUKE_SUCCESS, run_cuke(script, "", ""));
}
TEST_F(run_scenarios, simple_scenario_2)
{
const char* script = R"*(
  Feature: some feature 
  with some description 
    Scenario: some scenario
      Then 5 is greater than 2
)*";

  EXPECT_EQ(CUKE_SUCCESS, run_cuke(script, "", ""));
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

  EXPECT_EQ(CUKE_SUCCESS, run_cuke(script, "", ""));
}