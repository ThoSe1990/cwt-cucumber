#include <gtest/gtest.h>

#include "../src/cucumber.hpp"

using namespace cwt::details;

TEST(vm, init_obj) { vm test_vm; }
TEST(vm, run_chunk)
{
  vm test_vm;
  [[maybe_unused]] return_code result = test_vm.run("Feature:");
}

TEST(vm, op_code_constant)
{
  vm test_vm;
  function f = std::make_unique<chunk>();
  f->push_byte(op_code::constant, 0);
  f->push_byte(f->make_constant(123), 0);

  ASSERT_EQ(test_vm.execute_function(std::move(f)), return_code::runtime_error);
  EXPECT_EQ(test_vm.stack().size(), 2);
  EXPECT_EQ(test_vm.stack().at(0).type(), cuke::value_type::function);
  EXPECT_EQ(test_vm.stack().at(1).type(), cuke::value_type::integral);
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
  EXPECT_EQ(test_vm.global(name).type(), cuke::value_type::nil);
}
TEST(vm, op_code_set_var_double)
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
  ASSERT_EQ(test_vm.global(name).type(), cuke::value_type::_double);
  EXPECT_EQ(test_vm.global(name).as<double>(), 99.99);
}
TEST(vm, op_code_set_var_float)
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
  f->push_byte(f->make_constant(99.99f), 0);
  f->push_byte(op_code::set_var, 0);
  f->push_byte(idx, 0);

  ASSERT_EQ(test_vm.execute_function(std::move(f)), return_code::runtime_error);
  ASSERT_EQ(test_vm.global(name).type(), cuke::value_type::floating);
  EXPECT_EQ(test_vm.global(name).as<float>(), 99.99f);
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
  EXPECT_EQ(test_vm.stack().back().type(), cuke::value_type::string);
  EXPECT_EQ(test_vm.stack().back().as<std::string>(),
            std::string("some initial value"));
}
