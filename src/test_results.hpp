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
/**
 * @brief The outcome of one hook execution around a scenario.
 */
struct hook_result
{
  test_status status{test_status::passed};
  std::string error_msg;
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
  std::vector<hook_result> before{};
  std::vector<hook_result> after{};
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

  /**
   * @brief Two run-level failures: neither has a scenario or step to
   * attach to, so neither shows up in data(), and both must be checked
   * before it.
   * @details parse_errors() counts feature files that failed to parse (see
   * add_parse_error(), called from test_runner::run()). run_failed() is a
   * single flag for an assertion failing before any feature or scenario
   * exists, which today only happens inside a `BEFORE_ALL` hook, since
   * fail_current() then has neither a hook record nor a step to write to
   * (see add_run_failure()). final_result() consults both first, the same
   * way scenarios_failed() is consulted for an ordinary scenario failure.
   * @note An assertion inside `AFTER_ALL` does not reach add_run_failure();
   * see fail_current()'s documentation for why.
   */
  [[nodiscard]] std::size_t parse_errors() const noexcept;
  [[nodiscard]] bool run_failed() const noexcept;

  void add_parse_error() noexcept;
  void add_run_failure() noexcept;

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

  std::size_t m_parse_errors{0};
  bool m_run_failed{false};
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

/**
 * @brief True when a step is being executed and can carry a failure.
 */
[[nodiscard]] bool has_current_step() noexcept;
/**
 * @brief Records a failure against whatever is currently executing.
 * @details A hook record when one is open, otherwise the current step, and
 * a run-level failure (test_result::add_run_failure()) only when neither
 * exists - which is the case in a `BEFORE_ALL` hook, since it runs before
 * any feature or scenario exists.
 * @attention A failing assertion inside `AFTER_ALL` does NOT take the
 * run-level path above: `AFTER_ALL` runs after the whole run, when
 * has_current_step() is already true (the last scenario's last step), so
 * it is charged to that step instead, retroactively, after that step was
 * already counted passed - the terminal-vs-JSON disagreement this fix
 * otherwise removes. Known limitation of this sink, not a design choice;
 * fixing it means teaching fail_current() to tell "a scenario is
 * executing" apart from "a step record exists," which is a decision for
 * whoever owns this library, not something this fix takes on.
 */
void fail_current(std::string_view error_msg);

enum class hook_kind
{
  before,
  after
};

/**
 * @brief Which hook record a failure belongs to, if any.
 * @details Names the record by an absolute path - feature index, scenario
 * index, hook kind, and record index - rather than by a pointer or by
 * "whatever is current". Neither a vector reallocation nor a feature or
 * scenario pushed while this cursor is active can make the path resolve to
 * a different record: fail_current bounds-checks every step of the path
 * before writing through it, and falls back to the current step otherwise.
 */
struct hook_cursor
{
  bool active{false};
  std::size_t feature{0};
  std::size_t scenario{0};
  hook_kind kind{hook_kind::before};
  std::size_t index{0};
};

/**
 * @brief Opens a hook record on the current scenario for its lifetime.
 * @details While one is alive, an assertion inside the hook is recorded
 * against that hook rather than against whatever step last ran. The record
 * is named by position, not by reference: the feature, the scenario, the
 * hook kind, and the index within that kind's vector, all captured when the
 * scope opens.
 * @attention A hook must not run a nested test run while a scope is open.
 * Position stays valid as long as the containers it is measured against
 * only ever grow; results::remove_last_scenario() breaks that, since it can
 * pop a scenario and let a later one take its place, leaving an open
 * scope's index in range but naming a scenario it was never opened on. The
 * bounds checks in fail_current mean this can only misattribute or drop a
 * record - never read or write out of bounds - but it is still the wrong
 * record, so treat this as a hard rule and not a risk to weigh.
 */
class hook_scope
{
 public:
  explicit hook_scope(hook_kind kind);
  ~hook_scope();
  hook_scope(const hook_scope&) = delete;
  hook_scope& operator=(const hook_scope&) = delete;

 private:
  // fail_current resolves the active hook_cursor at call time, not a
  // pointer held across the scope's lifetime. test_runner, parser and
  // registry are all public API, so a hook could in principle construct and
  // run a nested test_runner, nesting hook_scope instances on the same
  // vector; saving the cursor's previous value here and restoring it on
  // destruction is what keeps that safe, rather than relying on no caller
  // ever nesting a scope today.
  hook_cursor m_previous;
};

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
