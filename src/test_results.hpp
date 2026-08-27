#pragma once

#include <vector>
#include <string>

#include "table.hpp"

namespace cuke::ast
{
class feature_node;
class scenario_node;
class step_node;
}  // namespace cuke::ast

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
  std::size_t line{0};
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
  std::size_t line{0};
  std::string name;
  std::string description;
  std::string keyword;
  std::vector<step> steps{};
  std::vector<std::string> tags;
};
struct feature
{
  std::string id;
  std::string keyword;
  std::string name;
  std::string description;
  std::string file;
  std::size_t line{0};
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
[[nodiscard]] test_status final_result();
[[nodiscard]] std::string scenarios_to_string();
[[nodiscard]] std::string steps_to_string();
[[nodiscard]] const char* to_color(test_status status);
[[nodiscard]] std::string to_string(test_status status);
[[nodiscard]] std::string step_prefix(test_status status);

void new_feature(const cuke::ast::feature_node& current);
[[nodiscard]] scenario& new_scenario(const cuke::ast::scenario_node& current);
[[nodiscard]] step& new_step(const cuke::ast::step_node& current);
void remove_last_scenario();
void set_step_to(test_status status);

[[nodiscard]] feature& features_back();
[[nodiscard]] scenario& scenarios_back();
[[nodiscard]] step& steps_back();

}  // namespace cuke::results

namespace cuke
{

/**
 * @brief Returns the feature currently being executed.
 * @details Valid from the moment a feature starts running (before any of
 * its scenarios) until the next feature begins. Intended for use in hooks
 * (`BEFORE`, `AFTER`, `BEFORE_STEP`, `AFTER_STEP`) or step definitions that
 * need to know which feature is currently running, e.g. for logging.
 * @attention Only call this while a feature is running; calling it before
 * any feature has started is undefined behavior.
 */
[[nodiscard]] const results::feature& current_feature();

/**
 * @brief Returns the scenario currently being executed.
 * @details Valid from the moment a scenario is selected to run - i.e.
 * already usable in a `BEFORE` hook - through its `AFTER` hook. `status`
 * reflects the scenario's outcome so far and is only final once the
 * scenario has finished.
 * @attention Only call this while a scenario is running; calling it
 * outside of a scenario's lifetime is undefined behavior.
 */
[[nodiscard]] const results::scenario& current_scenario();

/**
 * @brief Returns the step currently being executed.
 * @details Valid from `BEFORE_STEP` through `AFTER_STEP`, including inside
 * the step definition itself. `status` reflects the step's outcome so far
 * and is only final once the step has finished.
 * @attention Only call this while a step is running; calling it outside of
 * a step's lifetime is undefined behavior.
 */
[[nodiscard]] const results::step& current_step();

}  // namespace cuke
