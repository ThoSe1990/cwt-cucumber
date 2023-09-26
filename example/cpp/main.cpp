
#include "cwt/cucumber.hpp"


cwtc_step("hello int world {int}")
{
  int arg1 = cwtc_arg(1);
  std::cout << "glad to see you ...  " << arg1 << std::endl;
  cwtc_assert_true(arg1 == 0);
}

cwtc_step("hello string world {string}")
{
  std::string arg1 = cwtc_arg(1);
  std::cout << "glad to see you ...  " << arg1 << std::endl;
}

int main(int argc, const char* argv[])
{
  cwtc::tests c; 
  c.run(argc, argv);
  return 0;
}
