
#include "cwt/cucumber.hpp"

// TODO same statements with differtent types doesn't work ... 

CWTC_STEP("this can fail with {int}")
{
  const int arg1 = CWTC_ARG(1);
  cwtc_assert(arg1 == 0);
}
CWTC_STEP("this is an {int}")
{
  int arg1 = CWTC_ARG(1);
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