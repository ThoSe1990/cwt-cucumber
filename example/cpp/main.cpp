
#include "cwt/cucumber.hpp"

// TODO same statements with differtent types doesn't work ... 

CWTC_STEP("my first step passes!")
{
  std::cout << "hello world!" << std::endl;
  cwtc_assert(true);
}
CWTC_STEP("this fails!")
{
  std::cout << "FATAL FATAL!!!" << std::endl;
  cwtc_assert(false);
}

CWTC_STEP("this is {int}")
{
  int i = CWTC_ARG(1);
  std::string s = CWTC_ARG(2);
  std::cout << "int value: " << i << std::endl;
  std::cout << "string value: " << s << std::endl;
}



int main(int argc, const char* argv[])
{
  cwtc::tests c; 

  c.run(argc, argv);
  return 0;
}