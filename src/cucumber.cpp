#include "cucumber.hpp"
#include "ast/test_results.hpp"
#include "options.hpp"
#include "ast/test_results.hpp"

namespace cuke
{

cuke::results::test_status entry_point(int argc, const char* argv[])
{
  if (cuke::internal::print_help(argc, argv))
  {
    return cuke::results::final_result();
  }

  cuke::internal::terminal_arguments targs(argc, argv);

  return cuke::results::final_result();
}

}  // namespace cuke
