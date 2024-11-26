#include "cucumber.hpp"
#include "options.hpp"
#include "catalog.hpp"
#include "test_results.hpp"
#include "test_runner.hpp"
#include "util.hpp"

#include <algorithm>

namespace cuke
{

void print_failed_scenarios()
{
  bool first = true;
  for (const results::feature& feature : results::test_results().data())
  {
    std::for_each(feature.scenarios.begin(), feature.scenarios.end(),
                  [&first](const auto& scenario)
                  {
                    if (scenario.status == results::test_status::failed)
                    {
                      if (first)
                      {
                        println("Failed Scenarios:");
                        first = false;
                      }
                      print(internal::color::red, scenario.name);
                      println(internal::color::black, "  ", scenario.file, ':',
                              scenario.line);
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
  if (cucumber.export_catalog())
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
  println();
  println(results::scenarios_to_string());
  println(results::steps_to_string());
}

const options& cwt_cucumber::get_options() const noexcept
{
  return program_arguments().get_options();
}
bool cwt_cucumber::print_help() const noexcept
{
  if (get_options().print_help)
  {
    print_help_screen();
    return true;
  }
  return false;
}
bool cwt_cucumber::export_catalog(
    std::size_t json_indents /* = 2 */) const noexcept
{
  if (get_options().catalog.readable_text)
  {
    catalog::print_readable_text_to_sink();
    return true;
  }
  if (get_options().catalog.json)
  {
    catalog::print_json_to_sink(json_indents);
    return true;
  }
  return false;
}
results::test_status cwt_cucumber::final_result() const noexcept
{
  return cuke::results::final_result();
}

}  // namespace cuke
