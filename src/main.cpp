#include "cucumber.hpp"

int main(int argc, const char* argv[])
{
  cwt::details::vm cuke_vm(argc, argv);

  try
  {
    cuke_vm.run();  
  }
  catch(const std::exception& e)
  {
    cwt::details::println(cwt::details::color::red, e.what());
    return static_cast<int>(cwt::details::return_code::runtime_error);
  }
  
  return static_cast<int>(cuke_vm.final_result());
}