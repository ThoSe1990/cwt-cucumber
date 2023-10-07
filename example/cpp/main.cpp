
#include "cwt/cucumber.hpp"

// TODO same statements with differtent types doesn't work ... 

STEP_(test, "hello world")
{
  
}

STEP("two numbers: {int} and {int}")
{
  int n1 = CUKE_ARG(1);
  int n2 = CUKE_ARG(2);
  std::cout << "n1: " << n1 << "  n2: " << n2 << std::endl;
}

STEP("fail")
{
  cuke_assert(false);
}

STEP("one number: {int}")
{
  int one = CUKE_ARG(1);
  std::cout << "one " << one << std::endl;
}
STEP("one double: {double}")
{
  double d = CUKE_ARG(1);
  std::cout << "double " << d << std::endl;
}
STEP("string: {str}")
{
  std::string str = CUKE_ARG(1);
  std::cout << "string: " << str << std::endl;
}


int main(int argc, const char* argv[])
{
  cuke::tests c; 

  c.run(argc, argv);
  return 0;
}