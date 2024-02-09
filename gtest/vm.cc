#include <gtest/gtest.h>

#include "../src/cwt/cucumber.hpp"

using namespace cwt::details;

TEST(vm, init_obj) { vm test_vm; }
TEST(vm, run_chunk)
{
  vm test_vm;
  [[maybe_unused]] return_code result = test_vm.interpret("Feature:");
}

TEST(vm, op_code_constant)
{
  vm test_vm;
  function f = std::make_unique<chunk>();
  f->push_byte(op_code::constant, 0);
  f->push_byte(f->make_constant(123), 0);

  ASSERT_EQ(test_vm.execute_function(std::move(f)), return_code::runtime_error);
  EXPECT_EQ(test_vm.stack().size(), 2);
  EXPECT_EQ(test_vm.stack().at(0).type(), value_type::function);
  EXPECT_EQ(test_vm.stack().at(1).type(), value_type::integral);
  EXPECT_EQ(test_vm.stack().at(1).as<int>(), 123);
  EXPECT_EQ(test_vm.frames().size(), 1);
  EXPECT_EQ(test_vm.frames().at(0).chunk_ptr,
            test_vm.stack().at(0).as<function>().get());
}
TEST(vm, op_code_define_var)
{
  vm test_vm;
  const std::string name{"foo"};
  function f = std::make_unique<chunk>();
  std::size_t idx = f->make_constant(name);

  f->push_byte(op_code::constant, 0);
  f->push_byte(f->make_constant(nil_value{}), 0);
  f->push_byte(op_code::define_var, 0);
  f->push_byte(idx, 0);

  ASSERT_EQ(test_vm.execute_function(std::move(f)), return_code::runtime_error);
  EXPECT_EQ(test_vm.stack().size(), 1);
  EXPECT_EQ(test_vm.global(name).type(), value_type::nil);
}
TEST(vm, op_code_set_var)
{
  vm test_vm;
  const std::string name{"foo"};
  function f = std::make_unique<chunk>();
  std::size_t idx = f->make_constant(name);

  f->push_byte(op_code::constant, 0);
  f->push_byte(f->make_constant(nil_value{}), 0);
  f->push_byte(op_code::define_var, 0);
  f->push_byte(idx, 0);

  f->push_byte(op_code::constant, 0);
  f->push_byte(f->make_constant(99.99), 0);
  f->push_byte(op_code::set_var, 0);
  f->push_byte(idx, 0);

  ASSERT_EQ(test_vm.execute_function(std::move(f)), return_code::runtime_error);
  ASSERT_EQ(test_vm.global(name).type(), value_type::floating);
  EXPECT_EQ(test_vm.global(name).as<double>(), 99.99);
}
TEST(vm, op_code_get_var)
{
  vm test_vm;
  const std::string name{"foo"};
  function f = std::make_unique<chunk>();
  std::size_t idx = f->make_constant(name);

  f->push_byte(op_code::constant, 0);
  f->push_byte(f->make_constant(std::string("some initial value")), 0);
  f->push_byte(op_code::define_var, 0);
  f->push_byte(idx, 0);

  f->push_byte(op_code::get_var, 0);
  f->push_byte(idx, 0);

  ASSERT_EQ(test_vm.execute_function(std::move(f)), return_code::runtime_error);
  ASSERT_EQ(test_vm.stack().size(), 2);
  EXPECT_EQ(test_vm.stack().back().type(), value_type::string);
  EXPECT_EQ(test_vm.stack().back().as<std::string>(),
            std::string("some initial value"));
}
//  TEST(vm, op_code_print)
//  {
//   vm test_vm;
//  }
//  TEST(vm, op_code_println)
//  {
//   vm test_vm;
//  }
//  TEST(vm, op_code_step_result)
//  {
//   vm test_vm;
//  }
//  TEST(vm, op_code_init_scenario)
//  {
//   vm test_vm;
//  }
//  TEST(vm, op_code_scenario_result)
//  {
//   vm test_vm;
//  }
//  TEST(vm, op_code_jump_if_failed)
//  {
//   vm test_vm;
//  }
//  TEST(vm, op_code_call)
//  {
//   vm test_vm;
//  }
//  TEST(vm, op_code_call_step)
//  {
//   vm test_vm;
//  }
//  TEST(vm, op_code_call_step_with_doc_string)
//  {
//   vm test_vm;
//  }
//  TEST(vm, op_code_hook)
//  {
//   vm test_vm;
//  }
//  TEST(vm, op_code_func_return)
//  {
//   vm test_vm;
//  }
