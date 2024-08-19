#include "cucumber.hpp"

int main(int argc, const char* argv[])
{
  auto result = cuke::entry_point(argc, argv);
  return result == cuke::results::test_status::passed ? EXIT_SUCCESS
                                                      : EXIT_FAILURE;
}
