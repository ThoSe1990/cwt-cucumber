#include <iostream>
#include "../cucumber.hpp"

int main(int argc, const char* argv[])
{
  cuke::runner c; 
  return c.run(argc, argv);
}