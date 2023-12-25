#include "../cucumber.hpp"

int main(int argc, const char* argv[])
{
  cuke::details::init(); 
  return cuke::details::run(argc, argv);
}