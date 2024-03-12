#include "cucumber.hpp"

int main(int argc, const char* argv[])
{
  cwt::details::vm cuke_vm(argc, argv);

  if (cwt::details::print_help(argc, argv))
  {
    return EXIT_SUCCESS;
  }

  try
  {
    cuke_vm.run();  
  }
  catch(const std::runtime_error& e)
  {
    cwt::details::println(cwt::details::color::red, e.what());
    return EXIT_FAILURE;
  }
  
  return static_cast<int>(cuke_vm.final_result());
}