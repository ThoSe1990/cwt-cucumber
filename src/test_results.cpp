#include "test_results.hpp"
#include "ast.hpp"
#include "log.hpp"
#include "util.hpp"
#include "util_regex.hpp"

namespace cuke::results
{

test_result& test_results()
{
  static test_result instance;
  return instance;
}

void test_result::clear() noexcept
{
  m_data.clear();

  m_scenarios_count = 0;

  m_scenarios_failed = 0;
  m_scenarios_skipped = 0;
  m_scenarios_passed = 0;

  m_steps_count = 0;

  m_steps_failed = 0;
  m_steps_undefined = 0;
  m_steps_skipped = 0;
  m_steps_passed = 0;
}

std::size_t test_result::scenarios_passed() const noexcept
{
  return m_scenarios_passed;
}

std::size_t test_result::scenarios_failed() const noexcept
{
  return m_scenarios_failed;
}
std::size_t test_result::scenarios_skipped() const noexcept
{
  return m_scenarios_skipped;
}
std::size_t test_result::steps_passed() const noexcept
{
  return m_steps_passed;
}
std::size_t test_result::steps_failed() const noexcept
{
  return m_steps_failed;
}
std::size_t test_result::steps_skipped() const noexcept
{
  return m_steps_skipped;
}
std::size_t test_result::steps_undefined() const noexcept
{
  return m_steps_undefined;
}
void test_result::add_scenario(test_status status) noexcept
{
  ++m_scenarios_count;
  switch (status)
  {
    case test_status::passed:
      ++m_scenarios_passed;
      break;
    case test_status::failed:
      ++m_scenarios_failed;
      break;
    case test_status::skipped:
      ++m_scenarios_skipped;
      break;
    case test_status::undefined:
      // can't happen ...
      break;
  }
}
void test_result::add_step(test_status status) noexcept
{
  ++m_steps_count;
  switch (status)
  {
    case test_status::passed:
      ++m_steps_passed;
      break;
    case test_status::failed:
      ++m_steps_failed;
      break;
    case test_status::skipped:
      ++m_steps_skipped;
      break;
    case test_status::undefined:
      ++m_steps_undefined;
      break;
  }
}

std::size_t test_result::scenarios_count() const noexcept
{
  return m_scenarios_count;
}
std::size_t test_result::steps_count() const noexcept { return m_steps_count; }

std::string scenarios_to_string()
{
  std::string str;
  str.reserve(64);

  str.append(std::to_string(results::test_results().scenarios_count()));
  str.append(results::test_results().scenarios_count() > 1 ? " Scenarios ("
                                                           : " Scenario (");

  bool add_comma = false;
  if (results::test_results().scenarios_failed() > 0)
  {
    str.append(log::red);
    str.append(std::to_string(results::test_results().scenarios_failed()));
    str.append(" failed");
    str.append(log::reset_color);
    add_comma = true;
  }

  if (results::test_results().scenarios_skipped() > 0)
  {
    if (add_comma)
    {
      str.append(", ");
    }
    str.append(log::blue);
    str.append(std::to_string(results::test_results().scenarios_skipped()));
    str.append(" skipped");
    str.append(log::reset_color);
    add_comma = true;
  }

  if (results::test_results().scenarios_passed() > 0)
  {
    if (add_comma)
    {
      str.append(", ");
    }
    str.append(log::green);
    str.append(std::to_string(results::test_results().scenarios_passed()));
    str.append(" passed");
    str.append(log::reset_color);
  }

  str += ')';

  return str;
}

std::string steps_to_string()
{
  std::string str;
  str.reserve(64);

  str.append(std::to_string(results::test_results().steps_count()));
  str.append(results::test_results().steps_count() > 1 ? " Steps ("
                                                       : " Step (");

  bool add_comma = false;
  if (results::test_results().steps_failed() > 0)
  {
    str.append(log::red);
    str.append(std::to_string(results::test_results().steps_failed()));
    str.append(" failed");
    str.append(log::reset_color);
    add_comma = true;
  }

  if (results::test_results().steps_undefined() > 0)
  {
    if (add_comma)
    {
      str.append(", ");
    }
    str.append(log::yellow);
    str.append(std::to_string(results::test_results().steps_undefined()));
    str.append(" undefined");
    str.append(log::reset_color);
    add_comma = true;
  }

  if (results::test_results().steps_skipped() > 0)
  {
    if (add_comma)
    {
      str.append(", ");
    }
    str.append(log::blue);
    str.append(std::to_string(results::test_results().steps_skipped()));
    str.append(" skipped");
    str.append(log::reset_color);
    add_comma = true;
  }

  if (results::test_results().steps_passed() > 0)
  {
    if (add_comma)
    {
      str.append(", ");
    }
    str.append(log::green);
    str.append(std::to_string(results::test_results().steps_passed()));
    str.append(" passed");
    str.append(log::reset_color);
  }

  str += ')';

  return str;
}
const char* to_color(test_status status)
{
  switch (status)
  {
    case cuke::results::test_status::passed:
      return log::green;
    case cuke::results::test_status::failed:
      return log::red;
    case cuke::results::test_status::skipped:
      return log::blue;
    case cuke::results::test_status::undefined:
      return log::yellow;
    default:
      return log::reset_color;
  }
}
std::string to_string(test_status status)
{
  switch (status)
  {
    case cuke::results::test_status::passed:
      return std::string("passed");
    case cuke::results::test_status::failed:
      return std::string("failed");
    case cuke::results::test_status::skipped:
      return std::string("skipped");
    case cuke::results::test_status::undefined:
      return std::string("undefined");
    default:
      return std::string("");
  }
}

std::string step_prefix(test_status status)
{
  switch (status)
  {
    case cuke::results::test_status::passed:
      return std::string("[   PASSED    ] ");
    case cuke::results::test_status::failed:
      return std::string("[   FAILED    ] ");
    case cuke::results::test_status::skipped:
      return std::string("[   SKIPPED   ] ");
    case cuke::results::test_status::undefined:
      return std::string("[   UNDEFINED ] ");
    default:
      return std::string("");
  }
}

void new_feature(const cuke::ast::feature_node& current)
{
  feature result;
  result.id = current.id();
  result.file = current.file();
  result.line = current.line();
  result.tags = current.tags();
  result.keyword = current.keyword();
  result.name = current.name();
  result.description = cuke::internal::to_string(current.description());
  test_results().data().push_back(result);
}
void new_scenario(const cuke::ast::scenario_node& current)
{
  scenario result;
  result.id = current.id();
  result.line = current.line();
  result.name = current.name();
  result.description = cuke::internal::to_string(current.description());
  result.keyword = current.keyword();
  result.tags = current.tags();
  test_results().back().scenarios.push_back(result);
}
void new_step(const cuke::ast::step_node& current)
{
  step result;
  result.line = current.line();
  result.id = std::format("{};{}", test_results().back().scenarios.back().id,
                          current.name());
  result.name = current.name();
  result.keyword = current.keyword();
  result.doc_string = cuke::internal::to_string(current.doc_string());
  result.table = current.data_table();

  test_results().back().scenarios.back().steps.push_back(result);
}

void set_source_location(const std::string& location)
{
  test_results().back().scenarios.back().steps.back().source_location =
      location;
}

void set_feature_to(test_status status)
{
  test_results().back().status = status;
}
void set_scenario_to(test_status status)
{
  test_results().back().scenarios.back().status = status;
}
void set_step_to(test_status status)
{
  test_results().back().scenarios.back().steps.back().status = status;
}
}  // namespace cuke::results
