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
    function_ptr("this fails", test_step_fail);
    function_ptr("this passes", test_step_pass);
    function_ptr("{int} and {int} are equal", test_is_equal);
  }

  void TearDown() override 
  {
    reset_tags();
    compile_all();
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

  std::vector<const char*> args = {"some program", "-l", "4"};
  int lines[MAX_LINES];
  int count = 0;
  option_lines(args.size(), args.data(), 0, lines, &count);
  only_compile_lines(lines, count);

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

  std::vector<const char*> args = {"some program", "-l", "6", "-l", "10"};
  int lines[MAX_LINES];
  int count = 0;
  option_lines(args.size(), args.data(), 0, lines, &count);
  only_compile_lines(lines, count);

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
  std::vector<const char*> args = {"some program", "-l", "9"};
  int lines[MAX_LINES];
  int count = 0;
  option_lines(args.size(), args.data(), 0, lines, &count);
  only_compile_lines(lines, count);

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

  std::vector<const char*> args = {"some program", "-l", "10", "-l", "11"};
  int lines[MAX_LINES];
  int count = 0;
  option_lines(args.size(), args.data(), 0, lines, &count);
  only_compile_lines(lines, count);

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
  std::vector<const char*> args = {"some program", "-l", "10", "-l", "14"};
  int lines[MAX_LINES];
  int count = 0;
  option_lines(args.size(), args.data(), 0, lines, &count);
  only_compile_lines(lines, count);

  EXPECT_EQ(CUKE_FAILED, run_cuke(script, ""));
}

// TODO with tags