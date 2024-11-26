#pragma once

#include <memory>
#include <vector>
#include <string>

#include "util.hpp"
#include "table.hpp"

namespace cuke::results
{

enum class test_status
{
  passed = 0,
  failed,
  skipped,
  undefined
};
struct step
{
  test_status status{test_status::passed};
  std::size_t line;
  std::string id;
  std::string name;
  std::string file;
  std::string keyword;
  std::string source_location;
  std::string doc_string;
  cuke::table table;
};
struct scenario
{
  std::string id;
  test_status status{test_status::passed};
  std::size_t line;
  std::string name;
  std::string file;
  std::string keyword;
  std::vector<step> steps{};
  std::vector<std::pair<std::string, std::size_t>> tags;
};
struct feature
{
  test_status status{test_status::passed};
  std::string id;
  std::string keyword;
  std::string name;
  std::string uri;
  std::vector<std::pair<std::string, std::size_t>> tags;
  std::vector<scenario> scenarios{};
};

class test_result
{
 public:
  [[nodiscard]] std::vector<feature>& data() noexcept { return m_data; }
  [[nodiscard]] feature& back() noexcept { return m_data.back(); }
  void clear() noexcept;

  [[nodiscard]] std::size_t scenarios_passed() const noexcept;
  [[nodiscard]] std::size_t scenarios_failed() const noexcept;
  [[nodiscard]] std::size_t scenarios_skipped() const noexcept;
  [[nodiscard]] std::size_t steps_passed() const noexcept;
  [[nodiscard]] std::size_t steps_failed() const noexcept;
  [[nodiscard]] std::size_t steps_skipped() const noexcept;
  [[nodiscard]] std::size_t steps_undefined() const noexcept;

  void add_scenario(test_status status) noexcept;
  void add_step(test_status status) noexcept;

  [[nodiscard]] std::size_t scenarios_count() const noexcept;
  [[nodiscard]] std::size_t steps_count() const noexcept;

 private:
  std::vector<feature> m_data;

  std::size_t m_scenarios_count{0};
  std::size_t m_steps_count{0};

  std::size_t m_scenarios_passed{0};
  std::size_t m_scenarios_failed{0};
  std::size_t m_scenarios_skipped{0};

  std::size_t m_steps_passed{0};
  std::size_t m_steps_failed{0};
  std::size_t m_steps_skipped{0};
  std::size_t m_steps_undefined{0};
};

[[nodiscard]] test_result& test_results();

[[nodiscard]] static test_status final_result()
{
  if (test_results().data().empty())
  {
    return test_status::passed;
  }

  if (test_results().scenarios_failed() == 0)
  {
    return test_status::passed;
  }
  return test_status::failed;
}

static void new_feature() { test_results().data().emplace_back(); }
static void new_scenario() { test_results().back().scenarios.emplace_back(); }
static void new_scenario_outline()
{
  test_results().back().scenarios.emplace_back();
}
static void new_step()
{
  test_results().back().scenarios.back().steps.emplace_back();
}

static void set_feature_to(test_status status)
{
  test_results().back().status = status;
}
static void set_scenario_to(test_status status)
{
  test_results().back().scenarios.back().status = status;
}
static void set_step_to(test_status status)
{
  test_results().back().scenarios.back().steps.back().status = status;
}

[[nodiscard]] static feature& features_back() { return test_results().back(); }
[[nodiscard]] static scenario& scenarios_back()
{
  return test_results().back().scenarios.back();
}
[[nodiscard]] static step& steps_back()
{
  return test_results().back().scenarios.back().steps.back();
}

[[nodiscard]] std::string scenarios_to_string();
[[nodiscard]] std::string steps_to_string();
[[nodiscard]] internal::color to_color(test_status status);
[[nodiscard]] std::string to_string(test_status status);
[[nodiscard]] std::string step_prefix(test_status status);

}  // namespace cuke::results
