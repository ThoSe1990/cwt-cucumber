#include <gtest/gtest.h>

extern "C" {
  #include "cwt/cucumber.h"  
  #include "cwt/implementation/program_options.h"
  #include "cwt/implementation/compiler.h"
}

class run_single_scenario : public ::testing::Test 
{
protected:
  void SetUp() override
  {
    open_cucumber();
    cuke_step("this fails", test_step_fail);
    cuke_step("this passes", test_step_pass);
    cuke_step("{int} and {int} are equal", test_is_equal);
  }

  void TearDown() override 
  {
    reset_tags();
    reset_scenarios_by_line();
    close_cucumber();
  }
  
  static void test_is_equal(int arg_count, cuke_value* args)
  {
    int n1 = cuke_to_int(&args[0]);
    int n2 = cuke_to_int(&args[1]);
    cuke_assert(n1 == n2, "n1 is not equal n2");
  }

  static void test_step_fail(int arg_count, cuke_value* args)
  {
    cuke_assert(false, "");
  } 
  static void test_step_pass(int arg_count, cuke_value* args)
  {

  } 
};



TEST_F(run_single_scenario, with_scenario_by_line_1)
{
const char* script = R"*(
  Feature: some feature 
    
    Scenario: some scenario
      Given this fails

    Scenario: some scenario
      Given this passes
)*";
  int argc = 3;
  const char* argv[argc];
  argv[0] = "some program";
  argv[1] = "-l";
  argv[2] = "4"; 

  int single_scenarios[MAX_SCENARIOS];
  int single_scenarios_count = get_scenario_lines(argc, &argv[1], single_scenarios);
  set_scenarios_by_line(single_scenarios, single_scenarios_count);

  EXPECT_EQ(CUKE_FAILED, run_cuke(script, ""));
}


TEST_F(run_single_scenario, with_scenario_by_line_2)
{
const char* script = R"*(
  Feature: some feature 
    
    Scenario: some scenario
      Given this fails
    Scenario: some scenario
      Given this passes
    Scenario: some scenario
      Given this fails
    Scenario: some scenario
      Given this passes


)*";
  int argc = 5;
  const char* argv[argc];
  argv[0] = "some program";
  argv[1] = "-l";
  argv[2] = "6"; 
  argv[3] = "-l";
  argv[4] = "10"; 

  int single_scenarios[MAX_SCENARIOS];
  int single_scenarios_count = get_scenario_lines(argc, &argv[1], single_scenarios);
  set_scenarios_by_line(single_scenarios, single_scenarios_count);

  EXPECT_EQ(CUKE_SUCCESS, run_cuke(script, ""));
}


TEST_F(run_single_scenario, with_scenario_by_line_3)
{
const char* script = R"*(
  Feature: some feature 
  with some description 
    Scenario Outline: some scenario
      Then <var1> and <var2> are equal
          
    Examples: some name here 
    | var1 | var2 |
    | 9    | 1    |
    | 1    | 1    |
)*";
  int argc = 3;
  const char* argv[argc];
  argv[0] = "some program";
  argv[1] = "-l";
  argv[2] = "9"; 

  int single_scenarios[MAX_SCENARIOS];
  int single_scenarios_count = get_scenario_lines(argc, &argv[1], single_scenarios);
  set_scenarios_by_line(single_scenarios, single_scenarios_count);

  EXPECT_EQ(CUKE_FAILED, run_cuke(script, ""));
}

TEST_F(run_single_scenario, with_scenario_by_line_4)
{
const char* script = R"*(
  Feature: some feature 
  with some description 
    Scenario Outline: some scenario
      Then <var1> and <var2> are equal
          
    Examples: some name here 
    | var1 | var2 |
    | 9    | 1    |
    | 1    | 1    |
    | 9    | 1    |
    | 1    | 1    |
)*";
  int argc = 5;
  const char* argv[argc];
  argv[0] = "some program";
  argv[1] = "-l";
  argv[2] = "10"; 
  argv[3] = "-l";
  argv[4] = "11"; 


  int single_scenarios[MAX_SCENARIOS];
  int single_scenarios_count = get_scenario_lines(argc, &argv[1], single_scenarios);
  set_scenarios_by_line(single_scenarios, single_scenarios_count);

  EXPECT_EQ(CUKE_FAILED, run_cuke(script, ""));
}


TEST_F(run_single_scenario, with_scenario_by_line_5)
{
const char* script = R"*(
  Feature: some feature 
  with some description 
    Scenario Outline: some scenario
      Then <var1> and <var2> are equal
          
    Examples: some name here 
    | var1 | var2 |
    | 9    | 1    |
    | 1    | 1    |
          
    Examples: some name here 
    | var1 | var2 |
    | 9    | 1    |
    | 1    | 1    |
)*";
  int argc = 5;
  const char* argv[argc];
  argv[0] = "some program";
  argv[1] = "-l";
  argv[2] = "10";
  argv[3] = "-l";
  argv[4] = "14"; 

  int single_scenarios[MAX_SCENARIOS];
  int single_scenarios_count = get_scenario_lines(argc, &argv[1], single_scenarios);
  set_scenarios_by_line(single_scenarios, single_scenarios_count);

  EXPECT_EQ(CUKE_FAILED, run_cuke(script, ""));
}

// TODO with tags