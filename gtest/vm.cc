#include <gtest/gtest.h>

// #include "../src/cwt/vm.hpp"
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

  ASSERT_THROW([[maybe_unused]] auto result = test_vm.run(std::move(f)),
               std::out_of_range);
  EXPECT_EQ(test_vm.stack().size(), 2);
  EXPECT_EQ(test_vm.stack().at(0).type(), value_type::function);
  EXPECT_EQ(test_vm.stack().at(1).type(), value_type::integral);
  EXPECT_EQ(test_vm.stack().at(1).as<int>(), 123);
  EXPECT_EQ(test_vm.frames().size(), 1);
  EXPECT_EQ(test_vm.frames().at(0).chunk_ptr,
            test_vm.stack().at(0).as<function>().get());
}
//  TEST(vm, op_code_tag)
//  {
//   vm test_vm;
//  }
//  TEST(vm, op_code_nil)
//  {
//   vm test_vm;
//  }
//  TEST(vm, op_code_pop)
//  {
//   vm test_vm;
//  }
//  TEST(vm, op_code_get_var)
//  {
//   vm test_vm;
//  }
//  TEST(vm, op_code_set_var)
//  {
//   vm test_vm;
//  }
//  TEST(vm, op_code_define_var)
//  {
//   vm test_vm;
//  }
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
