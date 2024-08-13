#include "cucumber.hpp"
#include "ast/test_results.hpp"
#include "options.hpp"

#include "ast/parser.hpp"
#include "ast/test_runner.hpp"
#include "util.hpp"

namespace cuke
{

cuke::results::test_status entry_point(int argc, const char* argv[])
{
  if (cuke::internal::print_help(argc, argv))
  {
    return cuke::results::final_result();
  }

  cuke::internal::terminal_arguments targs(argc, argv);

  for (const auto& feature : targs.get_options().files)
  {
    cuke::parser p;
    p.parse_from_file(feature.path);

    cuke::test_runner runner(&feature);
    if (targs.get_options().quiet)
    {
      runner.set_quiet();
    }
    p.for_each_scenario(runner);
  }

  internal::println();
  internal::println(results::scenarios_to_string());
  internal::println(results::steps_to_string());

  return cuke::results::final_result();
}

}  // namespace cuke
