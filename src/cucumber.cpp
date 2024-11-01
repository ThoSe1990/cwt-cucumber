#include "cucumber.hpp"
#include <algorithm>
#include "options.hpp"
#include "parser.hpp"
#include "test_results.hpp"
#include "test_runner.hpp"
#include "util.hpp"

namespace cuke
{

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
  cwt_cucumber cucumber(argc, argv);
  if (cucumber.print_help())
  {
    return cuke::results::test_status::passed;
  }
  cucumber.run_tests();
  cucumber.print_results();
  return cucumber.final_result();
}

cwt_cucumber::cwt_cucumber(int argc, const char* argv[])
{
  program_arguments().initialize(argc, argv);
}
void cwt_cucumber::run_tests() const noexcept
{
  cuke::test_runner runner;
  runner.setup();
  runner.run();
  runner.teardown();
}
void cwt_cucumber::print_results() const noexcept
{
  print_failed_scenarios();
  internal::println();
  internal::println(results::scenarios_to_string());
  internal::println(results::steps_to_string());
}
bool cwt_cucumber::print_help() const noexcept
{
  if (m_args.get_options().print_help)
  {
    internal::print_help_screen();
    return true;
  }
  return false;
}
results::test_status cwt_cucumber::final_result() const noexcept
{
  return cuke::results::final_result();
}

}  // namespace cuke
