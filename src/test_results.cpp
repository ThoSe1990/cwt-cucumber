#include "test_results.hpp"
#include "util.hpp"

namespace cuke::results
{

test_result& test_results()
{
  static test_result instance;
  return instance;
}

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
