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
        unsigned char v = cwt::details::get_arg(n, values, 1, __FILE__, __LINE__);
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
        unsigned short v = cwt::details::get_arg(n, values, 1, __FILE__, __LINE__);
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
        unsigned int v = cwt::details::get_arg(n, values, 1, __FILE__, __LINE__);
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
        unsigned long v = cwt::details::get_arg(n, values, 1, __FILE__, __LINE__);
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

// class vm_get_args_floating : public ::testing::Test
// {
//  public:
//   static bool called;

//  protected:
//   void SetUp() override
//   {
//     called = false;
//     test_vm = vm();
//     test_vm.push_step(step(
//         [](argc n, argv values)
//         {
//           char c = cwt::details::get_arg(n, values, 1, __FILE__, __LINE__);
//           ASSERT_EQ(c, 99);
//           unsigned int uc =
//               cwt::details::get_arg(n, values, 1, __FILE__, __LINE__);
//           ASSERT_EQ(uc, 99);

//           short s = cwt::details::get_arg(n, values, 1, __FILE__, __LINE__);
//           ASSERT_EQ(s, 99);
//           unsigned short us =
//               cwt::details::get_arg(n, values, 1, __FILE__, __LINE__);
//           ASSERT_EQ(us, 99);

//           int i = cwt::details::get_arg(n, values, 1, __FILE__, __LINE__);
//           ASSERT_EQ(i, 99);
//           unsigned int ui =
//               cwt::details::get_arg(n, values, 1, __FILE__, __LINE__);
//           ASSERT_EQ(ui, 99);

//           long l = cwt::details::get_arg(n, values, 1, __FILE__, __LINE__);
//           ASSERT_EQ(l, 99);
//           unsigned long ul =
//               cwt::details::get_arg(n, values, 1, __FILE__, __LINE__);
//           ASSERT_EQ(ul, 99);

//           std::size_t size_type =
//               cwt::details::get_arg(n, values, 1, __FILE__, __LINE__);
//           ASSERT_EQ(size_type, 99);

//           called = true;
//         },
//         "This is ninetynine: {int}"));
//   }

//   void TearDown() override { test_vm.reset(); }

//   vm test_vm;
// };
// bool vm_get_args_integral::called;

// TEST_F(vm_get_args_floating, run)
// {
//   const char* script = R"*(
//   Feature: First Feature
//   Scenario: First Scenario
//   * This is ninetynine:  99
// )*";
//   [[maybe_unused]] return_code r = test_vm.interpret(script);
//   EXPECT_TRUE(called);
// }
