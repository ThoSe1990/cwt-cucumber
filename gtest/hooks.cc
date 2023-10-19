#include <gtest/gtest.h>

extern "C" {
    #include "cwt/cucumber.h"   
}

class test_hooks_before : public ::testing::Test 
{
protected:
  void SetUp() override
  {
    m_called = false;
    open_cucumber();
    cuke_step("a step", step);
    cuke_before(before);
  }

  void TearDown() override 
  {
    close_cucumber();
  }

  static void step(int arg_count, cuke_value* args) { }
  static void before(int arg_count, cuke_value* args)
  {
    m_called = true;
  } 

  static bool m_called;
};
bool test_hooks_before::m_called;

TEST_F(test_hooks_before, call_before_scenario)
{
  const char* script = R"*(
  Feature: some feature 
  with some description 
    Scenario: some scenario
      Given a step
)*";
  ASSERT_EQ(CUKE_SUCCESS, run_cuke(script, ""));
  EXPECT_TRUE(m_called);
}