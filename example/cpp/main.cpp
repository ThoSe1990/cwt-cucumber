
#include "cwt/cucumber.hpp"

// TODO same statements with differtent types doesn't work ... 

CWTC_STEP("two numbers: {int} and {int}")
{
  int n1 = CWTC_ARG(1);
  int n2 = CWTC_ARG(2);
  std::cout << "n1: " << n1 << "  n2: " << n2 << std::endl;
}
CWTC_STEP("one number: {int}")
{
  int one = CWTC_ARG(1);
  std::cout << "one " << one << std::endl;
}
CWTC_STEP("one double: {double}")
{
  double d = CWTC_ARG(1);
  std::cout << "double " << d << std::endl;
}
CWTC_STEP("string: {str}")
{
  std::string str = CWTC_ARG(1);
  std::cout << "string: " << str << std::endl;
}


int main(int argc, const char* argv[])
{
  cwtc::tests c; 

  c.run(argc, argv);
  return 0;
}