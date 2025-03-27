#pragma once

#include <vector>
#include <string>

#include "ast.hpp"
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
  std::string keyword;
  std::string source_location;
  std::string doc_string;
  std::string error_msg;
  cuke::table table;
};
struct scenario
{
  std::string id;
  test_status status{test_status::passed};
  std::size_t line;
  std::string name;
  std::string description;
  std::string keyword;
  std::vector<step> steps{};
  std::vector<std::string> tags;
};
struct feature
{
  test_status status{test_status::passed};
  std::string id;
  std::string keyword;
  std::string name;
  std::string description;
  std::string file;
  std::size_t line;
  std::vector<std::string> tags;
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

void new_feature(const cuke::ast::feature_node& current);
void new_scenario(const cuke::ast::scenario_node& current);
void new_scenario_outline(const cuke::ast::scenario_outline_node& current,
                          std::size_t number);
void new_step(const cuke::ast::step_node& current);
void set_source_location(const std::string& location);
void set_feature_to(test_status status);
void set_scenario_to(test_status status);
void set_step_to(test_status status);

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
