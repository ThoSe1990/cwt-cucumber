#include <gtest/gtest.h>

extern "C" {
  #include "cwt/cucumber.h"  
}

class runtime_tags : public ::testing::Test 
{
protected:
  void SetUp() override
  {
    open_cucumber();
    cuke_step("this fails", test_step_fail);
    cuke_step("this passes", test_step_pass);
    cuke_step("step with {int} and {int} are equal", test_is_equal);
  }

  void TearDown() override 
  {
    reset_tags();
    close_cucumber();
  }
  
  static void test_step_fail(int arg_count, cuke_value* args)
  {
    cuke_assert(false);
  } 
  static void test_step_pass(int arg_count, cuke_value* args)
  {

  } 
  static void test_is_equal(int arg_count, cuke_value* args)
  {
    int n1 = cuke_to_int(&args[0]);
    int n2 = cuke_to_int(&args[1]);
    cuke_assert(n1 == n2);
  }
};

TEST_F(runtime_tags, will_pass)
{
const char* script = R"*(
  Feature: some feature 
    
    @fail
    Scenario: some scenario
      Given this fails
    @pass
    Scenario: some scenario
      Given this passes
)*";
  int argc = 3;
  const char* argv[argc];
  argv[0] = "some program";
  argv[1] = "-t";
  argv[2] = "@pass";
  set_program_options(argc, argv);
  EXPECT_EQ(CUKE_SUCCESS, run_cuke(script, ""));
}

TEST_F(runtime_tags, will_fail)
{
const char* script = R"*(
  Feature: some feature 
    
    @fail
    Scenario: some scenario
      Given this fails
    @pass
    Scenario: some scenario
      Given this passes
)*";

  int argc = 3;
  const char* argv[argc];
  argv[0] = "some program";
  argv[1] = "-t";
  argv[2] = "@fail";
  set_program_options(argc, argv);

  EXPECT_EQ(CUKE_FAILED, run_cuke(script, ""));
}

TEST_F(runtime_tags, scenario_outline_fails)
{
const char* script = R"*(
  Feature: some feature 
    
    Scenario Outline: some scenario
      Given step with <n1> and <n2> are equal
    
    @pass
    Examples:
    | n1 | n2 | 
    | 1  | 1  | 
    | 2  | 2  |
    
    @fail
    Examples:
    | n1 | n2 | 
    | 1  | 2  | 
    | 3  | 4  |
)*";


  int argc = 3;
  const char* argv[argc];
  argv[0] = "some program";
  argv[1] = "-t";
  argv[2] = "@fail";
  set_program_options(argc, argv);

  EXPECT_EQ(CUKE_FAILED, run_cuke(script, ""));
}

TEST_F(runtime_tags, scenario_outline_passes)
{
const char* script = R"*(
  Feature: some feature 
    
    Scenario Outline: some scenario
      Given step with <n1> and <n2> are equal
    
    @pass
    Examples:
    | n1 | n2 | 
    | 1  | 1  | 
    | 2  | 2  |
    
    @fails
    Examples:
    | n1 | n2 | 
    | 1  | 2  | 
    | 3  | 4  |
)*";


  int argc = 3;
  const char* argv[argc];
  argv[0] = "some program";
  argv[1] = "-t";
  argv[2] = "@pass";
  set_program_options(argc, argv);

  EXPECT_EQ(CUKE_SUCCESS, run_cuke(script, ""));
}
