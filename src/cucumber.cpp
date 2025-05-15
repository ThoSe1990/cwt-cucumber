#include "cucumber.hpp"

#include "log.hpp"
#include "report.hpp"
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
                  [&first, &feature](const auto& scenario)
                  {
                    if (scenario.status == results::test_status::failed)
                    {
                      if (first)
                      {
                        log::always("Failed Scenarios:");
                        first = false;
                      }
                      log::always(log::red, scenario.name);
                      log::always(log::black, "  ", feature.file, ':',
                                  scenario.line, log::reset_color,
                                  log::new_line);
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
  switch (program_arguments().get_options().report.type)
  {
    case report_type::none:
      print_failed_scenarios();
      log::info(log::new_line);
      log::info(results::scenarios_to_string(), log::new_line);
      log::info(results::steps_to_string(), log::new_line);
      break;

    case report_type::json:
      report::print_json_to_sink();
      break;

    default:
      log::error("Unknown report type");
  }
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
  if (get_options().catalog.type == catalog_type::readable_text)
  {
    catalog::print_readable_text_to_sink();
    return true;
  }
  if (get_options().catalog.type == catalog_type::json)
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
