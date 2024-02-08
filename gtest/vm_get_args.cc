#include <gtest/gtest.h>

#include "../src/cwt/cucumber.hpp"

using namespace cwt::details;

class vm_get_args_integral : public ::testing::Test
{
 public:
  static bool called;

 protected:
  void SetUp() override
  {
    called = false;
    test_vm = vm();
  }

  void TearDown() override { test_vm.reset(); }

  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  * This is ninetynine:  99
)*";

  vm test_vm;
};
bool vm_get_args_integral::called;

TEST_F(vm_get_args_integral, value_char)
{
  test_vm.push_step(step(
      [](argc n, argv values)
      {
        char v = cwt::details::get_arg(n, values, 1, __FILE__, __LINE__);
        ASSERT_EQ(v, 99);
        called = true;
      },
      "This is ninetynine: {byte}"));

  EXPECT_EQ(test_vm.interpret(script), return_code::passed);
  EXPECT_TRUE(called);
}
TEST_F(vm_get_args_integral, value_unsigned_char)
{
  test_vm.push_step(step(
      [](argc n, argv values)
      {
        unsigned char v =
            cwt::details::get_arg(n, values, 1, __FILE__, __LINE__);
        ASSERT_EQ(v, 99);
        called = true;
      },
      "This is ninetynine: {byte}"));

  EXPECT_EQ(test_vm.interpret(script), return_code::passed);
  EXPECT_TRUE(called);
}

TEST_F(vm_get_args_integral, value_short)
{
  test_vm.push_step(step(
      [](argc n, argv values)
      {
        short v = cwt::details::get_arg(n, values, 1, __FILE__, __LINE__);
        ASSERT_EQ(v, 99);
        called = true;
      },
      "This is ninetynine: {short}"));

  EXPECT_EQ(test_vm.interpret(script), return_code::passed);
  EXPECT_TRUE(called);
}
TEST_F(vm_get_args_integral, value_unsigned_short)
{
  test_vm.push_step(step(
      [](argc n, argv values)
      {
        unsigned short v =
            cwt::details::get_arg(n, values, 1, __FILE__, __LINE__);
        ASSERT_EQ(v, 99);
        called = true;
      },
      "This is ninetynine: {short}"));

  EXPECT_EQ(test_vm.interpret(script), return_code::passed);
  EXPECT_TRUE(called);
}

TEST_F(vm_get_args_integral, value_int)
{
  test_vm.push_step(step(
      [](argc n, argv values)
      {
        int v = cwt::details::get_arg(n, values, 1, __FILE__, __LINE__);
        ASSERT_EQ(v, 99);
        called = true;
      },
      "This is ninetynine: {int}"));

  EXPECT_EQ(test_vm.interpret(script), return_code::passed);
  EXPECT_TRUE(called);
}
TEST_F(vm_get_args_integral, value_unsigned_int)
{
  test_vm.push_step(step(
      [](argc n, argv values)
      {
        unsigned int v =
            cwt::details::get_arg(n, values, 1, __FILE__, __LINE__);
        ASSERT_EQ(v, 99);
        called = true;
      },
      "This is ninetynine: {int}"));

  EXPECT_EQ(test_vm.interpret(script), return_code::passed);
  EXPECT_TRUE(called);
}

TEST_F(vm_get_args_integral, value_long)
{
  test_vm.push_step(step(
      [](argc n, argv values)
      {
        long v = cwt::details::get_arg(n, values, 1, __FILE__, __LINE__);
        ASSERT_EQ(v, 99);
        called = true;
      },
      "This is ninetynine: {long}"));

  EXPECT_EQ(test_vm.interpret(script), return_code::passed);
  EXPECT_TRUE(called);
}
TEST_F(vm_get_args_integral, value_unsigned_long)
{
  test_vm.push_step(step(
      [](argc n, argv values)
      {
        unsigned long v =
            cwt::details::get_arg(n, values, 1, __FILE__, __LINE__);
        ASSERT_EQ(v, 99);
        called = true;
      },
      "This is ninetynine: {long}"));

  EXPECT_EQ(test_vm.interpret(script), return_code::passed);
  EXPECT_TRUE(called);
}

TEST_F(vm_get_args_integral, value_size_t)
{
  test_vm.push_step(step(
      [](argc n, argv values)
      {
        std::size_t v = cwt::details::get_arg(n, values, 1, __FILE__, __LINE__);
        ASSERT_EQ(v, 99);
        called = true;
      },
      "This is ninetynine: {long}"));

  EXPECT_EQ(test_vm.interpret(script), return_code::passed);
  EXPECT_TRUE(called);
}

class vm_get_args_floating : public ::testing::Test
{
 public:
  static bool called;

 protected:
  void SetUp() override
  {
    called = false;
    test_vm = vm();
  }

  void TearDown() override { test_vm.reset(); }
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  * This is ninetynine point ninetynine: 99.99
)*";
  vm test_vm;
};
bool vm_get_args_floating::called;

TEST_F(vm_get_args_floating, value_float)
{
  test_vm.push_step(step(
      [](argc n, argv values)
      {
        float v = cwt::details::get_arg(n, values, 1, __FILE__, __LINE__);
        ASSERT_EQ(v, 99.99f);
        called = true;
      },
      "This is ninetynine point ninetynine: {float}"));
  EXPECT_EQ(test_vm.interpret(script), return_code::passed);
  EXPECT_TRUE(called);
}
TEST_F(vm_get_args_floating, value_double)
{
  test_vm.push_step(step(
      [](argc n, argv values)
      {
        double v = cwt::details::get_arg(n, values, 1, __FILE__, __LINE__);
        ASSERT_EQ(v, 99.99);
        called = true;
      },
      "This is ninetynine point ninetynine: {double}"));
  EXPECT_EQ(test_vm.interpret(script), return_code::passed);
  EXPECT_TRUE(called);
}


class vm_get_args_strings : public ::testing::Test
{
 public:
  static bool called;

 protected:
  void SetUp() override
  {
    called = false;
    test_vm = vm();
  }

  void TearDown() override { test_vm.reset(); }
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  * This is "hello string"
)*";
  vm test_vm;
};
bool vm_get_args_strings::called;

TEST_F(vm_get_args_strings, value_string)
{
  test_vm.push_step(step(
      [](argc n, argv values)
      {
        std::string v = cwt::details::get_arg(n, values, 1, __FILE__, __LINE__);
        ASSERT_EQ(v, std::string("hello string"));
        called = true;
      },
      "This is {string}"));
  EXPECT_EQ(test_vm.interpret(script), return_code::passed);
  EXPECT_TRUE(called);
}
TEST_F(vm_get_args_strings, value_string_view)
{
  test_vm.push_step(step(
      [](argc n, argv values)
      {
        std::string_view v = cwt::details::get_arg(n, values, 1, __FILE__, __LINE__);
        ASSERT_STREQ(v.data(), std::string_view("hello string").data());
        called = true;
      },
      "This is {string}"));
  EXPECT_EQ(test_vm.interpret(script), return_code::passed);
  EXPECT_TRUE(called);
}
class vm_get_args_doc_strings : public ::testing::Test
{
 public:
  static bool called;

 protected:
  void SetUp() override
  {
    called = false;
    test_vm = vm();
  }

  void TearDown() override { test_vm.reset(); }
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  * Here is a doc string attached
  """
This is a doc string
attached to a step
  """
)*";
  vm test_vm;
  static constexpr const std::string_view expected{
"\n"
"This is a doc string\n"
"attached to a step\n"
"  "};
};
bool vm_get_args_doc_strings::called;

TEST_F(vm_get_args_doc_strings, value_string)
{
  test_vm.push_step(step(
      [](argc n, argv values)
      {
        std::string v = cwt::details::get_arg(n, values, 1, __FILE__, __LINE__);
        ASSERT_EQ(v, vm_get_args_doc_strings::expected);
        called = true;
      },
      "Here is a doc string attached"));
  EXPECT_EQ(test_vm.interpret(script), return_code::passed);
  EXPECT_TRUE(called);
}
TEST_F(vm_get_args_doc_strings, value_string_view)
{
  test_vm.push_step(step(
      [](argc n, argv values)
      {
        std::string_view v = cwt::details::get_arg(n, values, 1, __FILE__, __LINE__);
        ASSERT_STREQ(v.data(), vm_get_args_doc_strings::expected.data());
        called = true;
      },
      "Here is a doc string attached"));
  EXPECT_EQ(test_vm.interpret(script), return_code::passed);
  EXPECT_TRUE(called);
}