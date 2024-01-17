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
    function_ptr("a step", step);
    cuke_before(hook);
  }

  void TearDown() override 
  {
    close_cucumber();
  }

  static void step(int arg_count, cuke_value* args) { }
  static void hook(int arg_count, cuke_value* args)
  {
    m_called = true;
  } 
  static bool m_called;
};
bool test_hooks_before::m_called;

TEST_F(test_hooks_before, call_hook)
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




class test_hooks_after : public ::testing::Test 
{
protected:
  void SetUp() override
  {
    m_called = false;
    open_cucumber();
    function_ptr("a step", step);
    cuke_after(hook);
  }

  void TearDown() override 
  {
    close_cucumber();
  }

  static void step(int arg_count, cuke_value* args) { }
  static void hook(int arg_count, cuke_value* args)
  {
    m_called = true;
  } 
  static bool m_called;
};
bool test_hooks_after::m_called;

TEST_F(test_hooks_after, call_hook)
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




class test_hooks_before_t : public ::testing::Test 
{
protected:
  void SetUp() override
  {
    m_called = false;
    open_cucumber();
    function_ptr("a step", step);
    cuke_before_t(hook, "@tag1 or @tag2");
  }

  void TearDown() override 
  {
    close_cucumber();
  }

  static void step(int arg_count, cuke_value* args) { }
  static void hook(int arg_count, cuke_value* args)
  {
    m_called = true;
  } 
  static bool m_called;
};
bool test_hooks_before_t::m_called;

TEST_F(test_hooks_before_t, call_hook_1)
{
  const char* script = R"*(
  Feature: some feature 
  with some description 

    @tag1
    Scenario: some scenario
      Given a step
)*";
  ASSERT_EQ(CUKE_SUCCESS, run_cuke(script, ""));
  EXPECT_TRUE(m_called);
}
TEST_F(test_hooks_before_t, call_hook_2)
{
  const char* script = R"*(
  Feature: some feature 
  with some description 

    @tag2
    Scenario: some scenario
      Given a step
)*";
  ASSERT_EQ(CUKE_SUCCESS, run_cuke(script, ""));
  EXPECT_TRUE(m_called);
}
TEST_F(test_hooks_before_t, call_hook_3)
{
  const char* script = R"*(
  Feature: some feature 
  with some description 

    @tag3
    Scenario: some scenario
      Given a step
)*";
  ASSERT_EQ(CUKE_SUCCESS, run_cuke(script, ""));
  EXPECT_FALSE(m_called);
}




class test_hooks_after_t : public ::testing::Test 
{
protected:
  void SetUp() override
  {
    m_called = false;
    open_cucumber();
    function_ptr("a step", step);
    cuke_before_t(hook, "(@tag1 and @tag2)or@tag3");
  }

  void TearDown() override 
  {
    close_cucumber();
  }

  static void step(int arg_count, cuke_value* args) { }
  static void hook(int arg_count, cuke_value* args)
  {
    m_called = true;
  } 
  static bool m_called;
};
bool test_hooks_after_t::m_called;

TEST_F(test_hooks_after_t, call_hook_1)
{
  const char* script = R"*(
  Feature: some feature 
  with some description 

    @tag1 @tag2
    Scenario: some scenario
      Given a step
)*";
  ASSERT_EQ(CUKE_SUCCESS, run_cuke(script, ""));
  EXPECT_TRUE(m_called);
}
TEST_F(test_hooks_after_t, call_hook_2)
{
  const char* script = R"*(
  Feature: some feature 
  with some description 

    @tag3
    Scenario: some scenario
      Given a step
)*";
  ASSERT_EQ(CUKE_SUCCESS, run_cuke(script, ""));
  EXPECT_TRUE(m_called);
}

TEST_F(test_hooks_after_t, call_hook_3)
{
  const char* script = R"*(
  Feature: some feature 
  with some description 

    @tag4
    Scenario: some scenario
      Given a step
)*";
  ASSERT_EQ(CUKE_SUCCESS, run_cuke(script, ""));
  EXPECT_FALSE(m_called);
}




class test_hooks_steps : public ::testing::Test 
{
protected:
  void SetUp() override
  {
    m_call_count = 0;
    open_cucumber();
    function_ptr("a step", step);
    cuke_before_step(hook);
    cuke_after_step(hook);
  }

  void TearDown() override 
  {
    close_cucumber();
  }

  static void step(int arg_count, cuke_value* args) { }
  static void hook(int arg_count, cuke_value* args)
  {
    m_call_count++;
  } 
  static int m_call_count;
};
int test_hooks_steps::m_call_count;


TEST_F(test_hooks_steps, call_hook_step)
{
  const char* script = R"*(
  Feature: some feature 
  with some description 
    Scenario: some scenario
      Given a step
      Given a step
)*";
  ASSERT_EQ(CUKE_SUCCESS, run_cuke(script, ""));
  EXPECT_EQ(m_call_count, 4);
}






class test_multiple_hooks_1 : public ::testing::Test 
{
protected:
  void SetUp() override
  {
    m_call_count = 0;
    open_cucumber();
    function_ptr("a step", step);
    cuke_after(hook);
    cuke_after_t(hook, "@hello and @world");
    cuke_before(hook);
  }

  void TearDown() override 
  {
    close_cucumber();
  }

  static void step(int arg_count, cuke_value* args) { }
  static void hook(int arg_count, cuke_value* args)
  {
    m_call_count++;
  } 
  static int m_call_count;
};
int test_multiple_hooks_1::m_call_count;


TEST_F(test_multiple_hooks_1, call_hook_step_1)
{
  const char* script = R"*(
  Feature: some feature 
  with some description 
    Scenario: some scenario
      Given a step
      Given a step
)*";
  ASSERT_EQ(CUKE_SUCCESS, run_cuke(script, ""));
  EXPECT_EQ(m_call_count, 2);
}
TEST_F(test_multiple_hooks_1, call_hook_step_2)
{
  const char* script = R"*(
  Feature: some feature 
  with some description 
    @hello 
    Scenario: some scenario
      Given a step
      Given a step
)*";
  ASSERT_EQ(CUKE_SUCCESS, run_cuke(script, ""));
  EXPECT_EQ(m_call_count, 2);
}
TEST_F(test_multiple_hooks_1, call_hook_step_3)
{
  const char* script = R"*(
  Feature: some feature 
  with some description 
    @hello  @world
    Scenario: some scenario
      Given a step
      Given a step
)*";
  ASSERT_EQ(CUKE_SUCCESS, run_cuke(script, ""));
  EXPECT_EQ(m_call_count, 3);
}







class test_multiple_hooks_2 : public ::testing::Test 
{
protected:
  void SetUp() override
  {
    m_call_count = 0;
    open_cucumber();
    function_ptr("a step", step);
    function_ptr("a step with {int} and {string}", step);
    cuke_after(hook);
    cuke_after_t(hook, "@hello and @world");
    cuke_before(hook);
    cuke_before_t(hook, "@this or @that");
    cuke_before_step(hook);
    cuke_after_step(hook);
  }

  void TearDown() override 
  {
    close_cucumber();
  }

  static void step(int arg_count, cuke_value* args) { }
  static void hook(int arg_count, cuke_value* args)
  {
    m_call_count++;
  } 
  static int m_call_count;
};
int test_multiple_hooks_2::m_call_count;


TEST_F(test_multiple_hooks_2, call_hook_step_1)
{
  const char* script = R"*(
  Feature: some feature 
  with some description 
    Scenario: some scenario
      Given a step
)*";
  ASSERT_EQ(CUKE_SUCCESS, run_cuke(script, ""));
  EXPECT_EQ(m_call_count, 4);
}
TEST_F(test_multiple_hooks_2, call_hook_step_2)
{
  const char* script = R"*(
  Feature: some feature 
  with some description 
    @hello @this
    Scenario: some scenario
      Given a step
)*";
  ASSERT_EQ(CUKE_SUCCESS, run_cuke(script, ""));
  EXPECT_EQ(m_call_count, 5);
}

TEST_F(test_multiple_hooks_2, call_hook_step_3)
{
  const char* script = R"*(
  Feature: some feature 
  with some description 
    @hello @that
    Scenario: some scenario
      Given a step
)*";
  ASSERT_EQ(CUKE_SUCCESS, run_cuke(script, ""));
  EXPECT_EQ(m_call_count, 5);
}
TEST_F(test_multiple_hooks_2, call_hook_step_4)
{
  const char* script = R"*(
  Feature: some feature 
  with some description 
    @hello @world @this
    Scenario: some scenario
      Given a step
)*";
  ASSERT_EQ(CUKE_SUCCESS, run_cuke(script, ""));
  EXPECT_EQ(m_call_count, 6);
}
TEST_F(test_multiple_hooks_2, call_hook_step_5)
{
  const char* script = R"*(
  Feature: some feature 
  with some description 
    @hello @world @this
    Scenario: some scenario
      Given a step
      Given a step
)*";
  ASSERT_EQ(CUKE_SUCCESS, run_cuke(script, ""));
  EXPECT_EQ(m_call_count, 8);
}

TEST_F(test_multiple_hooks_2, call_hook_step_6)
{
  const char* script = R"*(
  Feature: some feature 
  with some description 
    @hello @world @this
    Scenario: some scenario
      Given a step with 123 and "some string"
      Given a step
)*";
  ASSERT_EQ(CUKE_SUCCESS, run_cuke(script, ""));
  EXPECT_EQ(m_call_count, 8);
}


TEST_F(test_multiple_hooks_2, call_hook_step_7)
{
  const char* script = R"*(
  Feature: some feature 
  with some description 
    @hello @world @this
    Scenario: some scenario
      Given a step
      Given a step

    Scenario Outline: scenario outline
      Given a step with <number> and <string>

      Examples:
        | number | string |
        | 1      | "..."  |
)*";
  ASSERT_EQ(CUKE_SUCCESS, run_cuke(script, ""));
  EXPECT_EQ(m_call_count, 12);
}