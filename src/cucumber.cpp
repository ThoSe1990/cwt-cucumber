#include "cucumber.hpp"
#include <algorithm>
#include "options.hpp"
#include "parser.hpp"
#include "test_results.hpp"
#include "test_runner.hpp"
#include "util.hpp"

namespace cuke
{

// TODO:
// - suppress prints when quiet -> use singleton targs, initialialize properly
//

void print_failed_scenarios()
{
  bool first = true;
  for (const results::feature& feature : results::test_results().data())
  {
    std::for_each(feature.scenarios.begin(), feature.scenarios.end(),
                  [&first](const results::scenario& scenario)
                  {
                    if (scenario.status == results::test_status::failed)
                    {
                      if (first)
                      {
                        internal::println("Failed Scenarios:");
                        first = false;
                      }
                      internal::print(internal::color::red, scenario.name);
                      internal::println(internal::color::black, "  ",
                                        scenario.file, ':', scenario.line);
                    }
                  });
  }
}

cuke::results::test_status entry_point(int argc, const char* argv[])
{
  if (cuke::internal::print_help(argc, argv))
  {
    return cuke::results::final_result();
  }

  internal::terminal_arguments& targs = internal::terminal_args(argc, argv);

  for (const auto& feature : targs.get_options().files)
  {
    cuke::parser p;
    p.parse_from_file(feature.path);

    cuke::test_runner runner(feature.lines_to_compile,
                             &targs.get_options().tags);
    if (targs.get_options().quiet)
    {
      runner.set_quiet();
    }
    p.for_each_scenario(runner);
  }

  print_failed_scenarios();
  internal::println();
  internal::println(results::scenarios_to_string());
  internal::println(results::steps_to_string());

  return cuke::results::final_result();
}

}  // namespace cuke
