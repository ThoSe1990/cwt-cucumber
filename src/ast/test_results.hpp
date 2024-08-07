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

 private:
  std::vector<feature> m_data;
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

}  // namespace cuke::results
