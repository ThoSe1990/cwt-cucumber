#include "test_results.hpp"
#include "ast.hpp"
#include "log.hpp"
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

  m_parse_errors = 0;
  m_run_failed = false;
}

std::size_t test_result::parse_errors() const noexcept
{
  return m_parse_errors;
}
bool test_result::run_failed() const noexcept { return m_run_failed; }

void test_result::add_parse_error() noexcept { ++m_parse_errors; }
void test_result::add_run_failure() noexcept { m_run_failed = true; }

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
    str.append(log::color::red());
    str.append(std::to_string(results::test_results().scenarios_failed()));
    str.append(" failed");
    str.append(log::color::reset());
    add_comma = true;
  }

  if (results::test_results().scenarios_skipped() > 0)
  {
    if (add_comma)
    {
      str.append(", ");
    }
    str.append(log::color::blue());
    str.append(std::to_string(results::test_results().scenarios_skipped()));
    str.append(" skipped");
    str.append(log::color::reset());
    add_comma = true;
  }

  if (results::test_results().scenarios_passed() > 0)
  {
    if (add_comma)
    {
      str.append(", ");
    }
    str.append(log::color::green());
    str.append(std::to_string(results::test_results().scenarios_passed()));
    str.append(" passed");
    str.append(log::color::reset());
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
    str.append(log::color::red());
    str.append(std::to_string(results::test_results().steps_failed()));
    str.append(" failed");
    str.append(log::color::reset());
    add_comma = true;
  }

  if (results::test_results().steps_undefined() > 0)
  {
    if (add_comma)
    {
      str.append(", ");
    }
    str.append(log::color::yellow());
    str.append(std::to_string(results::test_results().steps_undefined()));
    str.append(" undefined");
    str.append(log::color::reset());
    add_comma = true;
  }

  if (results::test_results().steps_skipped() > 0)
  {
    if (add_comma)
    {
      str.append(", ");
    }
    str.append(log::color::blue());
    str.append(std::to_string(results::test_results().steps_skipped()));
    str.append(" skipped");
    str.append(log::color::reset());
    add_comma = true;
  }

  if (results::test_results().steps_passed() > 0)
  {
    if (add_comma)
    {
      str.append(", ");
    }
    str.append(log::color::green());
    str.append(std::to_string(results::test_results().steps_passed()));
    str.append(" passed");
    str.append(log::color::reset());
  }

  str += ')';

  return str;
}
const char* to_color(test_status status)
{
  switch (status)
  {
    case cuke::results::test_status::passed:
      return log::color::green();
    case cuke::results::test_status::failed:
      return log::color::red();
    case cuke::results::test_status::skipped:
      return log::color::blue();
    case cuke::results::test_status::undefined:
      return log::color::yellow();
    default:
      return log::color::reset();
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

test_status final_result()
{
  if (test_results().run_failed())
  {
    return test_status::failed;
  }
  if (test_results().parse_errors() > 0)
  {
    return test_status::failed;
  }
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

}  // namespace cuke::results

namespace cuke::results
{

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
scenario& new_scenario(const cuke::ast::scenario_node& current)
{
  scenario result;
  result.id = current.id();
  result.line = current.line();
  result.name = current.name();
  result.description = cuke::internal::to_string(current.description());
  result.keyword = current.keyword();
  result.tags = current.tags();
  test_results().back().scenarios.push_back(result);
  return test_results().back().scenarios.back();
}
void remove_last_scenario() { test_results().back().scenarios.pop_back(); }
step& new_step(const cuke::ast::step_node& current)
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
  return test_results().back().scenarios.back().steps.back();
}

void set_step_to(test_status status)
{
  if (has_current_step())
  {
    steps_back().status = status;
  }
}

feature& features_back() { return test_results().back(); }
scenario& scenarios_back() { return test_results().back().scenarios.back(); }
step& steps_back()
{
  return test_results().back().scenarios.back().steps.back();
}

bool has_current_step() noexcept
{
  return !test_results().data().empty() &&
         !test_results().back().scenarios.empty() &&
         !test_results().back().scenarios.back().steps.empty();
}

namespace
{
// Which hook record a failure belongs to, if any. Named by an absolute
// path (feature index, scenario index, kind, record index) rather than a
// pointer or "whatever is current": a hook_scope nested inside another can
// grow the same vector, which a pointer would not survive, and a feature or
// scenario pushed while a scope is open would make "current" mean something
// else. hook_scope saves and restores this, so the innermost open scope is
// the one that owns a failure.
hook_cursor g_hook_cursor;
}  // namespace

hook_scope::hook_scope(hook_kind kind) : m_previous(g_hook_cursor)
{
  if (test_results().data().empty() || test_results().back().scenarios.empty())
  {
    // No scenario to attach to: detach from whatever the outer scope (if
    // any) was pointing at, so a hook running here does not silently write
    // into the outer hook's record. m_previous still remembers it, so the
    // destructor restores it once this scope ends.
    g_hook_cursor = hook_cursor{};
    return;
  }
  const std::size_t feature_index = test_results().data().size() - 1;
  feature& parent_feature = test_results().back();
  const std::size_t scenario_index = parent_feature.scenarios.size() - 1;
  scenario& current = parent_feature.scenarios.back();
  std::vector<hook_result>& records =
      kind == hook_kind::before ? current.before : current.after;
  records.emplace_back();
  g_hook_cursor = hook_cursor{true, feature_index, scenario_index, kind,
                              records.size() - 1};
}

hook_scope::~hook_scope() { g_hook_cursor = m_previous; }

void fail_current(std::string_view error_msg)
{
  if (g_hook_cursor.active &&
      g_hook_cursor.feature < test_results().data().size())
  {
    feature& target_feature = test_results().data()[g_hook_cursor.feature];
    if (g_hook_cursor.scenario < target_feature.scenarios.size())
    {
      scenario& target = target_feature.scenarios[g_hook_cursor.scenario];
      std::vector<hook_result>& records =
          g_hook_cursor.kind == hook_kind::before ? target.before
                                                  : target.after;
      if (g_hook_cursor.index < records.size())
      {
        records[g_hook_cursor.index].status = test_status::failed;
        records[g_hook_cursor.index].error_msg = error_msg;
        return;
      }
    }
  }
  if (has_current_step())
  {
    step& current = steps_back();
    current.status = test_status::failed;
    current.error_msg = error_msg;
    return;
  }
  // Neither a hook record nor a step: this is a BEFORE_ALL hook, which
  // runs before any feature or scenario exists. Record it against the run
  // itself so final_result() does not silently report success.
  //
  // A failing AFTER_ALL hook never reaches this branch: it runs after the
  // whole run, when has_current_step() above is already true (the last
  // scenario's last step), so it is charged to that step instead,
  // retroactively, after that step was already counted passed. Known
  // limitation, not a design choice made here.
  test_results().add_run_failure();
}

}  // namespace cuke::results

namespace cuke
{

const results::feature& current_feature() { return results::features_back(); }
const results::scenario& current_scenario()
{
  return results::scenarios_back();
}
const results::step& current_step() { return results::steps_back(); }

}  // namespace cuke
