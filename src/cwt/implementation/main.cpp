#include <iostream>
#include "../cucumber.hpp"

int main(int argc, const char* argv[])
{
      std::cout << "------------------ " << cuke::details::steps().size() << std::endl;

  cuke::tests c; 
  return c.run(argc, argv);
}