#pragma once

#include <algorithm>
#include <vector>

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
};
struct scenario
{
  std::vector<step> steps{};
  test_status status{test_status::passed};
};
struct feature
{
  std::vector<scenario> scenarios{};
  test_status status{test_status::passed};
};

class test_result
{
 public:
  [[nodiscard]] std::vector<feature>& data() noexcept { return m_data; }
  [[nodiscard]] feature& back() noexcept { return m_data.back(); }
  void clear() noexcept { m_data.clear(); }

  [[nodiscard]] std::size_t scenarios_passed() const noexcept
  {
    return m_scenarios_passed;
  }
  [[nodiscard]] std::size_t scenarios_failed() const noexcept
  {
    return m_scenarios_failed;
  }
  [[nodiscard]] std::size_t scenarios_skipped() const noexcept
  {
    return m_scenarios_skipped;
  }

  [[nodiscard]] std::size_t steps_passed() const noexcept
  {
    return m_steps_passed;
  }
  [[nodiscard]] std::size_t steps_failed() const noexcept
  {
    return m_steps_failed;
  }
  [[nodiscard]] std::size_t steps_skipped() const noexcept
  {
    return m_steps_skipped;
  }
  [[nodiscard]] std::size_t steps_undefined() const noexcept
  {
    return m_steps_undefined;
  }

  void add_scenario(test_status status) noexcept
  {
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

  void add_step(test_status status) noexcept
  {
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

 private:
  std::vector<feature> m_data;

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

  bool all_passed =
      std::all_of(test_results().data().begin(), test_results().data().end(),
                  [](const feature& f)
                  { return f.status == results::test_status::passed; });

  return all_passed ? test_status::passed : test_status::failed;
}

static void new_feature() { test_results().data().emplace_back(); }
static void new_scenario() { test_results().back().scenarios.emplace_back(); }
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

[[nodiscard]] static std::string scenarios_to_string() { return ""; }
[[nodiscard]] static std::string steps_to_string() { return ""; }

}  // namespace cuke::results
