#include <fstream>

#include "cucumber.hpp"

int main(int argc, const char* argv[])
{
  {
    namespace cuke = cwt::details;

    cuke::create_options(argc, argv);
  }

  cwt::details::vm cuke_vm;

std::ifstream in(cwt::details::terminal_options.files[0].path);
std::string contents((std::istreambuf_iterator<char>(in)), 
    std::istreambuf_iterator<char>());

  cwt::details::return_code result = cuke_vm.interpret(contents);

  return result == cwt::details::return_code::passed ? 0 : 1;
}