
#include "cwt/cucumber.hpp"

// TODO same statements with differtent types doesn't work ... 

CWTC_STEP("{byte} is below 99")
{
  const char arg1 = CWTC_ARG(1);
  const int arg2 = CWTC_ARG(2);

  cwtc_assert(arg1 < 0);
}
CWTC_STEP("this is a {double}")
{
  double arg1 = CWTC_ARG(1);
  cwtc_assert(arg1 == 0);
}

CWTC_STEP("this is a {string}")
{
  std::string arg1 = CWTC_ARG(1);
}

int main(int argc, const char* argv[])
{
  cwtc::tests c; 

  c.run(argc, argv);
  return 0;
}