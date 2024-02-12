#include "cucumber.hpp"

int main(int argc, const char* argv[])
{
  cwt::details::vm cuke_vm(argc, argv);
  cuke_vm.run();
  return static_cast<int>(cuke_vm.final_result());
}