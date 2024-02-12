#include <gtest/gtest.h>

#include "../src/cucumber.hpp"

using namespace cwt::details;

class vm_asserts_is_true : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    test_vm = vm();
    test_vm.push_step(step([](argc, argv) { cuke::is_true(true); }, "Is true"));
    test_vm.push_step(step([](argc, argv) { cuke::is_true(false); }, "Is not true"));
  }

  void TearDown() override { test_vm.reset(); }

  vm test_vm;

};

TEST_F(vm_asserts_is_true, pass)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given Is true
)*";

  EXPECT_EQ(return_code::passed, test_vm.run(script));
  EXPECT_EQ(1, test_vm.scenario_results().at(return_code::passed));
  EXPECT_EQ(1, test_vm.step_results().at(return_code::passed));
}

TEST_F(vm_asserts_is_true, fail)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given Is not true
)*";

  EXPECT_EQ(return_code::failed, test_vm.run(script));
  EXPECT_EQ(1, test_vm.scenario_results().at(return_code::failed));
  EXPECT_EQ(1, test_vm.step_results().at(return_code::failed));
}


class vm_asserts_is_false : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    test_vm = vm();
    test_vm.push_step(step([](argc, argv) { cuke::is_false(false); }, "Is false"));
    test_vm.push_step(step([](argc, argv) { cuke::is_false(true); }, "Is not false"));
  }

  void TearDown() override { test_vm.reset(); }

  vm test_vm;

};

TEST_F(vm_asserts_is_false, pass)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given Is false
)*";

  EXPECT_EQ(return_code::passed, test_vm.run(script));
  EXPECT_EQ(1, test_vm.scenario_results().at(return_code::passed));
  EXPECT_EQ(1, test_vm.step_results().at(return_code::passed));
}

TEST_F(vm_asserts_is_false, fail)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given Is not false
)*";

  EXPECT_EQ(return_code::failed, test_vm.run(script));
  EXPECT_EQ(1, test_vm.scenario_results().at(return_code::failed));
  EXPECT_EQ(1, test_vm.step_results().at(return_code::failed));
}



class vm_asserts_equal : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    test_vm = vm();
    test_vm.push_step(step([](argc n, argv values) 
    { 
      int v1 = CUKE_ARG(1);
      int v2 = CUKE_ARG(2);
      cuke::equal(v1, v2); 
    }, "{int} and {int} are equal"));
  }

  void TearDown() override { test_vm.reset(); }

  vm test_vm;

};

TEST_F(vm_asserts_equal, pass)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given 12 and 12 are equal
)*";

  EXPECT_EQ(return_code::passed, test_vm.run(script));
  EXPECT_EQ(1, test_vm.scenario_results().at(return_code::passed));
  EXPECT_EQ(1, test_vm.step_results().at(return_code::passed));
}

TEST_F(vm_asserts_equal, fail)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given 12 and 15 are equal
)*";

  EXPECT_EQ(return_code::failed, test_vm.run(script));
  EXPECT_EQ(1, test_vm.scenario_results().at(return_code::failed));
  EXPECT_EQ(1, test_vm.step_results().at(return_code::failed));
}


class vm_asserts_not_equal : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    test_vm = vm();
    test_vm.push_step(step([](argc n, argv values) 
    { 
      int v1 = CUKE_ARG(1);
      int v2 = CUKE_ARG(2);
      cuke::not_equal(v1, v2); 
    }, "{int} and {int} are not equal"));
  }

  void TearDown() override { test_vm.reset(); }

  vm test_vm;

};

TEST_F(vm_asserts_not_equal, pass)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given 99 and 12 are not equal
)*";

  EXPECT_EQ(return_code::passed, test_vm.run(script));
  EXPECT_EQ(1, test_vm.scenario_results().at(return_code::passed));
  EXPECT_EQ(1, test_vm.step_results().at(return_code::passed));
}

TEST_F(vm_asserts_not_equal, fail)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given 15 and 15 are not equal
)*";

  EXPECT_EQ(return_code::failed, test_vm.run(script));
  EXPECT_EQ(1, test_vm.scenario_results().at(return_code::failed));
  EXPECT_EQ(1, test_vm.step_results().at(return_code::failed));
}



class vm_asserts_greater_than : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    test_vm = vm();
    test_vm.push_step(step([](argc n, argv values) 
    { 
      int v1 = CUKE_ARG(1);
      int v2 = CUKE_ARG(2);
      cuke::greater(v1, v2); 
    }, "{int} is greater than {int}"));
  }

  void TearDown() override { test_vm.reset(); }

  vm test_vm;

};

TEST_F(vm_asserts_greater_than, pass)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given 20 is greater than 10
)*";

  EXPECT_EQ(return_code::passed, test_vm.run(script));
  EXPECT_EQ(1, test_vm.scenario_results().at(return_code::passed));
  EXPECT_EQ(1, test_vm.step_results().at(return_code::passed));
}

TEST_F(vm_asserts_greater_than, fail)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given 10 is greater than 20
)*";

  EXPECT_EQ(return_code::failed, test_vm.run(script));
  EXPECT_EQ(1, test_vm.scenario_results().at(return_code::failed));
  EXPECT_EQ(1, test_vm.step_results().at(return_code::failed));
}


class vm_asserts_greater_or_equal_than : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    test_vm = vm();
    test_vm.push_step(step([](argc n, argv values) 
    { 
      int v1 = CUKE_ARG(1);
      int v2 = CUKE_ARG(2);
      cuke::greater_or_equal(v1, v2); 
    }, "{int} is greater or equal than {int}"));
  }

  void TearDown() override { test_vm.reset(); }

  vm test_vm;

};

TEST_F(vm_asserts_greater_or_equal_than, pass_1)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given 10 is greater or equal than 10
)*";

  EXPECT_EQ(return_code::passed, test_vm.run(script));
  EXPECT_EQ(1, test_vm.scenario_results().at(return_code::passed));
  EXPECT_EQ(1, test_vm.step_results().at(return_code::passed));
}
TEST_F(vm_asserts_greater_or_equal_than, pass_2)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given 15 is greater or equal than 10
)*";

  EXPECT_EQ(return_code::passed, test_vm.run(script));
  EXPECT_EQ(1, test_vm.scenario_results().at(return_code::passed));
  EXPECT_EQ(1, test_vm.step_results().at(return_code::passed));
}

TEST_F(vm_asserts_greater_or_equal_than, fail)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given 9 is greater or equal than 10
)*";

  EXPECT_EQ(return_code::failed, test_vm.run(script));
  EXPECT_EQ(1, test_vm.scenario_results().at(return_code::failed));
  EXPECT_EQ(1, test_vm.step_results().at(return_code::failed));
}



class vm_asserts_less_than : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    test_vm = vm();
    test_vm.push_step(step([](argc n, argv values) 
    { 
      int v1 = CUKE_ARG(1);
      int v2 = CUKE_ARG(2);
      cuke::less(v1, v2); 
    }, "{int} is less than {int}"));
  }

  void TearDown() override { test_vm.reset(); }

  vm test_vm;

};

TEST_F(vm_asserts_less_than, pass)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given 5 is less than 10
)*";

  EXPECT_EQ(return_code::passed, test_vm.run(script));
  EXPECT_EQ(1, test_vm.scenario_results().at(return_code::passed));
  EXPECT_EQ(1, test_vm.step_results().at(return_code::passed));
}

TEST_F(vm_asserts_less_than, fail)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given 30 is less than 20
)*";

  EXPECT_EQ(return_code::failed, test_vm.run(script));
  EXPECT_EQ(1, test_vm.scenario_results().at(return_code::failed));
  EXPECT_EQ(1, test_vm.step_results().at(return_code::failed));
}



class vm_asserts_less_or_equal_than : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    test_vm = vm();
    test_vm.push_step(step([](argc n, argv values) 
    { 
      int v1 = CUKE_ARG(1);
      int v2 = CUKE_ARG(2);
      cuke::less_or_equal(v1, v2); 
    }, "{int} is less or equal than {int}"));
  }

  void TearDown() override { test_vm.reset(); }

  vm test_vm;

};

TEST_F(vm_asserts_less_or_equal_than, pass_1)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given 10 is less or equal than 10
)*";

  EXPECT_EQ(return_code::passed, test_vm.run(script));
  EXPECT_EQ(1, test_vm.scenario_results().at(return_code::passed));
  EXPECT_EQ(1, test_vm.step_results().at(return_code::passed));
}
TEST_F(vm_asserts_less_or_equal_than, pass_2)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given 5 is less or equal than 10
)*";

  EXPECT_EQ(return_code::passed, test_vm.run(script));
  EXPECT_EQ(1, test_vm.scenario_results().at(return_code::passed));
  EXPECT_EQ(1, test_vm.step_results().at(return_code::passed));
}

TEST_F(vm_asserts_less_or_equal_than, fail)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given 12 is less or equal than 10
)*";

  EXPECT_EQ(return_code::failed, test_vm.run(script));
  EXPECT_EQ(1, test_vm.scenario_results().at(return_code::failed));
  EXPECT_EQ(1, test_vm.step_results().at(return_code::failed));
}