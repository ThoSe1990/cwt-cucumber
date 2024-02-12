#pragma once

#include "value.hpp"
#include "compiler/tags.hpp"

namespace cwt::details
{

struct hook
{
 public:
  hook(hook_callback cb) : m_callback(cb) {}
  hook(hook_callback cb, std::string_view tags) : m_callback(cb), m_tags(tags)
  {
  }
  [[nodiscard]] bool valid_tags(argc n, argv tags) const
  {
    const auto scenario_has_tags = [&n]() { return n > 0; };
    if (m_tags.empty())
    {
      return true;
    }
    else
    {
      return scenario_has_tags() ? m_tags.evaluate(n, tags) : false;
    }
  }
  void call() const { m_callback(); }

 private:
  hook_callback m_callback;
  compiler::tag_expression m_tags;
};

}  // namespace cwt::details
