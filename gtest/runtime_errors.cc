#include <gtest/gtest.h>

    #include "cwt/cucumber.hpp"


class runtime_error_strings : public ::testing::Test 
{
protected:
  void SetUp() override
  {
    testing::internal::CaptureStderr(); 
    open_cucumber();
    cuke_step("a test step with a {string}", test_step);
  }

  void TearDown() override 
  {
    close_cucumber();
  }
  
  static void test_step(int arg_count, cuke_value* args)
  {

  }
};

TEST_F(runtime_error_strings, missing_quote)
{

const char* script = R"*(
  Feature: some feature ... 
    Scenario: some scenario ...
      Given a test step with a "string is valid"
      But this is an unknown step
)*";

  EXPECT_EQ(CUKE_FAILED, run_cuke(script, ""));
}