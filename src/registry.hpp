#pragma once

#include <algorithm>
#include <stdexcept>

#include "step.hpp"
#include "hooks.hpp"
#include "expression.hpp"

namespace cuke
{
namespace internal
{

// TODO: create a macro e.g. CUKE_PARAM_ARG(idx) for this
static const cuke::value& get_param_value(
    cuke::value_array::const_iterator begin, std::size_t values_count,
    std::size_t idx)
{
  std::size_t zero_based_idx = idx - 1;
  if (zero_based_idx < values_count)
  {
    return *(begin + zero_based_idx);
  }
  else
  {
    throw std::runtime_error(std::format("Index out of range"));
  }
}

template <typename Hook>
static void run_hook(const std::vector<Hook>& hooks)
{
  std::for_each(hooks.begin(), hooks.end(), [](const auto& h) { h.call(); });
}
template <typename Hook>
static void run_hook(const std::vector<Hook>& hooks,
                     const std::vector<std::string>& tags)
{
  std::for_each(hooks.begin(), hooks.end(),
                [&tags](const auto& h)
                {
                  if (h.valid_tag(tags))
                  {
                    h.call();
                  }
                });
}

class registry
{
 public:
  void sort_steps_by_type()
  {
    sort_steps([](const internal::step& lhs, const internal::step& rhs)
               { return lhs.step_type() < rhs.step_type(); });
  }

  void clear() noexcept
  {
    m_steps.clear();
    m_hooks.before.clear();
    m_hooks.after.clear();
    m_hooks.before_step.clear();
    m_hooks.after_step.clear();
    m_hooks.before_all.clear();
    m_hooks.after_all.clear();
  }

  [[nodiscard]] const expression& get_expression(std::string_view key) const
  {
    if (m_expressions.standard.contains(key.data()))
    {
      return m_expressions.standard.at(key.data());
    }
    throw std::runtime_error(
        std::format("Expression '{}' does not exists", key));
  }

  void push_step(const internal::step& s) noexcept { m_steps.push_back(s); }
  [[nodiscard]] const std::vector<internal::step>& steps() const noexcept
  {
    return m_steps;
  }

  void push_hook_before(const internal::hook& h) noexcept
  {
    m_hooks.before.push_back(h);
  }

  [[nodiscard]] const std::vector<internal::hook>& hooks_before() const noexcept
  {
    return m_hooks.before;
  }

  void push_hook_after(const internal::hook& h) noexcept
  {
    m_hooks.after.push_back(h);
  }
  [[nodiscard]] const std::vector<internal::hook>& hooks_after() const noexcept
  {
    return m_hooks.after;
  }
  void push_hook_after_all(const internal::hook& h) noexcept
  {
    m_hooks.after_all.push_back(h);
  }
  [[nodiscard]] const std::vector<internal::hook>& hooks_after_all()
      const noexcept
  {
    return m_hooks.after_all;
  }
  void push_hook_before_all(const internal::hook& h) noexcept
  {
    m_hooks.before_all.push_back(h);
  }
  [[nodiscard]] const std::vector<internal::hook>& hooks_before_all()
      const noexcept
  {
    return m_hooks.before_all;
  }
  void push_hook_before_step(const internal::hook& h) noexcept
  {
    m_hooks.before_step.push_back(h);
  }
  [[nodiscard]] const std::vector<internal::hook>& hooks_before_step()
      const noexcept
  {
    return m_hooks.before_step;
  }

  void push_hook_after_step(const internal::hook& h) noexcept
  {
    m_hooks.after_step.push_back(h);
  }
  [[nodiscard]] const std::vector<internal::hook>& hooks_after_step()
      const noexcept
  {
    return m_hooks.after_step;
  }

  void run_hook_before(const std::vector<std::string>& tags) const noexcept
  {
    run_hook(m_hooks.before, tags);
  }
  void run_hook_after(const std::vector<std::string>& tags) const noexcept
  {
    run_hook(m_hooks.after, tags);
  }
  void run_hook_before_step() const noexcept { run_hook(m_hooks.before_step); }
  void run_hook_after_step() const noexcept { run_hook(m_hooks.after_step); }
  void run_hook_before_all() const noexcept { run_hook(m_hooks.before_all); }
  void run_hook_after_all() const noexcept { run_hook(m_hooks.after_all); }

 private:
  template <typename Compare>
  void sort_steps(Compare compare)
  {
    std::sort(m_steps.begin(), m_steps.end(), compare);
  }

  std::vector<internal::step> m_steps;
  struct
  {
    std::vector<internal::hook> before;
    std::vector<internal::hook> after;
    std::vector<internal::hook> before_all;
    std::vector<internal::hook> after_all;
    std::vector<internal::hook> before_step;
    std::vector<internal::hook> after_step;
  } m_hooks;

  struct
  {
    const std::unordered_map<std::string, expression> standard = {
        {"{byte}",
         {"{byte}", "(-?\\d+)", "byte",
          [](cuke::value_array::const_iterator begin, std::size_t count) -> any
          { return get_param_value(begin, count, count).as<long long>(); }}},
        {"{int}",
         {"{int}", "(-?\\d+)", "int",
          [](cuke::value_array::const_iterator begin, std::size_t count) -> any
          { return get_param_value(begin, count, count).as<long long>(); }}},
        {"{short}",
         {"{short}", "(-?\\d+)", "short",
          [](cuke::value_array::const_iterator begin, std::size_t count) -> any
          { return get_param_value(begin, count, count).as<long long>(); }}},
        {"{long}",
         {"{long}", "(-?\\d+)", "long",
          [](cuke::value_array::const_iterator begin, std::size_t count) -> any
          { return get_param_value(begin, count, count).as<long long>(); }}},
        {"{float}",
         {"{float}", "(-?\\d*\\.?\\d+)", "float",
          [](cuke::value_array::const_iterator begin, std::size_t count) -> any
          { return get_param_value(begin, count, count).as<float>(); }}},
        {"{double}",
         {"{double}", "(-?\\d*\\.?\\d+)", "double",
          [](cuke::value_array::const_iterator begin, std::size_t count) -> any
          { return get_param_value(begin, count, count).as<double>(); }}},
        {"{word}",
         {"{word}", "([^\\s<]+)", "word",
          [](cuke::value_array::const_iterator begin, std::size_t count) -> any
          { return get_param_value(begin, count, count).to_string(); }}},
        {"{string}",
         {"{string}", "\"(.*?)\"", "string",
          [](cuke::value_array::const_iterator begin, std::size_t count) -> any
          { return get_param_value(begin, count, count).to_string(); }}},
        {"{}",
         {"{}", "(.+)", "anonymous",
          [](cuke::value_array::const_iterator begin, std::size_t count) -> any
          { return get_param_value(begin, count, count).to_string(); }}},
        {"{pair of integers}",
         {"{pair of integers}", R"(var1=(\d+), var2=(\d+))", "two integers",
          [](cuke::value_array::const_iterator begin, std::size_t count) -> any
          {
            int var1 = get_param_value(begin, count, 1).as<long long>();
            int var2 = get_param_value(begin, count, 2).as<long long>();
            return std::make_pair(var1, var2);
          }}}};
    const std::unordered_map<std::string, expression> custom;
  } m_expressions;
};

}  // namespace internal

[[nodiscard]] internal::registry& registry();

}  // namespace cuke
