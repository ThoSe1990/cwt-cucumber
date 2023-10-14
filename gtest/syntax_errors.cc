#include <gtest/gtest.h>

extern "C" {
  #include "compiler.h"
}

TEST(syntax_errors, invalid_begin)
{
  testing::internal::CaptureStderr(); 

const char* script = R"*(
  this_is_wrong!
)*";

  EXPECT_TRUE(NULL == compile(script, "", ""));
  const std::string error_print = testing::internal::GetCapturedStderr(); 
  EXPECT_STREQ("[line 2] Error at 'this_is_wrong!': Expect FeatureLine.\n", error_print.c_str());
}

TEST(syntax_errors, invalid_step)
{
  testing::internal::CaptureStderr(); 

const char* script = R"*(
  Feature: some feature ... 
    Scenario: some scenario ...
      Given Something given
      this_is_wrong! 
)*";

  EXPECT_TRUE(NULL == compile(script, "", ""));
  const std::string error_print = testing::internal::GetCapturedStderr(); 
  EXPECT_STREQ("[line 5] Error at 'this_is_wrong!': Expect StepLine or Scenario.\n", error_print.c_str());
}