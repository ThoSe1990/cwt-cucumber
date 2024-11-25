#include "test_results.hpp"
#include "util.hpp"

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
    str.append(cuke::internal::color_codes.at(cuke::internal::color::red));
    str.append(std::to_string(results::test_results().scenarios_failed()));
    str.append(" failed");
    str.append(cuke::internal::color_codes.at(cuke::internal::color::standard));
    add_comma = true;
  }

  if (results::test_results().scenarios_skipped() > 0)
  {
    if (add_comma)
    {
      str.append(", ");
    }
    str.append(cuke::internal::color_codes.at(cuke::internal::color::blue));
    str.append(std::to_string(results::test_results().scenarios_skipped()));
    str.append(" skipped");
    str.append(cuke::internal::color_codes.at(cuke::internal::color::standard));
    add_comma = true;
  }

  if (results::test_results().scenarios_passed() > 0)
  {
    if (add_comma)
    {
      str.append(", ");
    }
    str.append(cuke::internal::color_codes.at(cuke::internal::color::green));
    str.append(std::to_string(results::test_results().scenarios_passed()));
    str.append(" passed");
    str.append(cuke::internal::color_codes.at(cuke::internal::color::standard));
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
    str.append(cuke::internal::color_codes.at(cuke::internal::color::red));
    str.append(std::to_string(results::test_results().steps_failed()));
    str.append(" failed");
    str.append(cuke::internal::color_codes.at(cuke::internal::color::standard));
    add_comma = true;
  }

  if (results::test_results().steps_undefined() > 0)
  {
    if (add_comma)
    {
      str.append(", ");
    }
    str.append(cuke::internal::color_codes.at(cuke::internal::color::yellow));
    str.append(std::to_string(results::test_results().steps_undefined()));
    str.append(" undefined");
    str.append(cuke::internal::color_codes.at(cuke::internal::color::standard));
    add_comma = true;
  }

  if (results::test_results().steps_skipped() > 0)
  {
    if (add_comma)
    {
      str.append(", ");
    }
    str.append(cuke::internal::color_codes.at(cuke::internal::color::blue));
    str.append(std::to_string(results::test_results().steps_skipped()));
    str.append(" skipped");
    str.append(cuke::internal::color_codes.at(cuke::internal::color::standard));
    add_comma = true;
  }

  if (results::test_results().steps_passed() > 0)
  {
    if (add_comma)
    {
      str.append(", ");
    }
    str.append(cuke::internal::color_codes.at(cuke::internal::color::green));
    str.append(std::to_string(results::test_results().steps_passed()));
    str.append(" passed");
    str.append(cuke::internal::color_codes.at(cuke::internal::color::standard));
  }

  str += ')';

  return str;
}

}  // namespace cuke::results
