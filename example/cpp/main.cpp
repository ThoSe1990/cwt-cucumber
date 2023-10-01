
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

int main(int argc, const char* argv[])
{
  cwtc::tests c; 

  c.run(argc, argv);
  return 0;
}