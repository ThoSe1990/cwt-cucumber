#pragma once

#include <algorithm>
#include <format>
#include <stdexcept>
#include <string_view>

#include "step.hpp"
#include "hooks.hpp"
#include "expression.hpp"

namespace cuke
{
namespace internal
{

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

template <typename T>
static any make_parameter_value(cuke::value_array::const_iterator begin,
                                std::size_t count)
{
  return get_param_value(begin, count, 1).as<T>();
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
    m_expressions.custom.clear();
  }

  [[nodiscard]] std::unordered_map<std::string, expression> custom_expressions()
      const noexcept
  {
    return m_expressions.custom;
  }
  void push_expression(std::string_view key,
                       const expression& custom_expression)
  {
    if (m_expressions.custom.contains(key.data()))
    {
      throw std::runtime_error(
          std::format("Custom parameter type '{}' already exists.", key));
    }
    m_expressions.custom[key.data()] = custom_expression;
  }
  [[nodiscard]] bool has_expression(std::string_view key) const noexcept
  {
    return m_expressions.standard.contains(key.data()) ||
           m_expressions.custom.contains(key.data());
  }
  [[nodiscard]] const expression& get_expression(std::string_view key) const
  {
    if (m_expressions.standard.contains(key.data()))
    {
      return m_expressions.standard.at(key.data());
    }
    if (m_expressions.custom.contains(key.data()))
    {
      return m_expressions.custom.at(key.data());
    }
    throw std::runtime_error(
        std::format("Expression '{}' does not exists", key));
  }
  std::string create_expression_key_regex_pattern() const noexcept
  {
    std::stringstream pattern;
    pattern << "(";
    bool first = true;
    for (auto it = m_expressions.standard.begin();
         it != m_expressions.standard.end(); ++it)
    {
      if (!first)
      {
        pattern << "|";
      }
      first = false;
      pattern << "\\{" << it->first.substr(1, it->first.size() - 2) << "\\}";
    }
    for (auto it = m_expressions.custom.begin();
         it != m_expressions.custom.end(); ++it)
    {
      pattern << "|";
      pattern << "\\{" << it->first.substr(1, it->first.size() - 2) << "\\}";
    }
    pattern << ")";
    return pattern.str();
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
        {"{byte}", {"(-?\\d+)", "byte", make_parameter_value<char>}},
        {"{int}", {"(-?\\d+)", "int", make_parameter_value<int>}},
        {"{short}", {"(-?\\d+)", "short", make_parameter_value<short>}},
        {"{long}", {"(-?\\d+)", "long", make_parameter_value<long>}},
        {"{float}", {"(-?\\d*\\.?\\d+)", "float", make_parameter_value<float>}},
        {"{double}",
         {"(-?\\d*\\.?\\d+)", "double", make_parameter_value<double>}},
        {"{word}", {"([^\\s<]+)", "word", make_parameter_value<std::string>}},
        {"{string}",
         {"\"(.*?)\"", "string", make_parameter_value<std::string>}},
        {"{}", {"(.+)", "anonymous", make_parameter_value<std::string>}}};
    std::unordered_map<std::string, expression> custom;
  } m_expressions;
};

}  // namespace internal

[[nodiscard]] internal::registry& registry();

}  // namespace cuke
