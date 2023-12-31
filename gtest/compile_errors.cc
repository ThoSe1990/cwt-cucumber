#include <gtest/gtest.h>

extern "C" {
  #include "cwt/cucumber.h"
  #include "vm.h"
}


class compile_errors : public ::testing::Test 
{
protected:
  void SetUp() override
  {
    init_vm();
  }

  void TearDown() override 
  {
    free_vm();
  }
};

TEST_F(compile_errors, unexpected_identifier_1)
{
  testing::internal::CaptureStderr(); 

const char* script = R"*(
  this_is_wrong!
)*";

  EXPECT_EQ(CUKE_COMPILE_ERROR, interpret(script, ""));
  const std::string error_print = testing::internal::GetCapturedStderr(); 
  EXPECT_STREQ("[line 2] Error at 'this_is_wrong!': Expect FeatureLine.\n", error_print.c_str());
}

TEST_F(compile_errors, unexpected_identifier_2)
{
  testing::internal::CaptureStderr(); 

const char* script = R"*(
  Feature: some feature ... 
    Scenario: some scenario ...
      Given Something given
      this_is_wrong! 
)*";

  EXPECT_EQ(CUKE_COMPILE_ERROR, interpret(script, ""));
  const std::string error_print = testing::internal::GetCapturedStderr(); 
  EXPECT_STREQ("[line 5] Error at 'this_is_wrong!': Expect StepLine or Scenario.\n", error_print.c_str());
}

TEST_F(compile_errors, missing_quote)
{
  testing::internal::CaptureStderr(); 

const char* script = R"*(
  Feature: some feature ... 
    Scenario: some scenario ...
      Given a test step with a "hello world
)*";

  EXPECT_EQ(CUKE_COMPILE_ERROR, interpret(script, ""));
  const std::string error_print = testing::internal::GetCapturedStderr(); 
  EXPECT_STREQ("[line 4] Error: Unexpected linebreak in string value.\n", error_print.c_str());
}

TEST_F(compile_errors, missing_angle_bracket)
{
  testing::internal::CaptureStderr(); 

const char* script = R"*(
  Feature: some feature ... 
    Scenario: some scenario ...
      Given a test step with a <variable
)*";

  EXPECT_EQ(CUKE_COMPILE_ERROR, interpret(script, ""));
  const std::string error_print = testing::internal::GetCapturedStderr(); 
  EXPECT_STREQ("[line 4] Error: Expect '>' after variable.\n", error_print.c_str());
}
