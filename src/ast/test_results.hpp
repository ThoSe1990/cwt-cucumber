#pragma once 

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

static void new_feature(feature&& f)
{
  test_results().data().push_back(std::move(f));
}
static void new_scenario(scenario&& s)
{
  test_results().back().scenarios.push_back(std::move(s));
}
static void new_step(step&& s)
{
  test_results().back().scenarios.back().steps.push_back(std::move(s));
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

}  // namespace cuke::results


