#include "cucumber.hpp"

int main(int argc, const char* argv[])
{
  cuke::internal::vm cuke_vm(argc, argv);

  if (cuke::internal::print_help(argc, argv))
  {
    return EXIT_SUCCESS;
  }

  try
  {
    cuke_vm.run();  
  }
  catch(const std::runtime_error& e)
  {
    cuke::internal::println(cuke::internal::color::red, e.what());
    return EXIT_FAILURE;
  }
  
  return static_cast<int>(cuke_vm.final_result());
}