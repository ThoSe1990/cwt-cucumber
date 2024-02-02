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

  void call(argc n, argv tags) const
  {
    const auto scenario_has_no_tags = [&n]() { return n == 0; };
    const auto hook_has_tags = [this]() { return !m_tags.empty(); };
    if (hook_has_tags() && scenario_has_no_tags())
    {
      return;
    }
    else if (m_tags.evaluate(n, tags))
    {
      m_callback();
    }
  }
  void operator()() const { m_callback(); }

 private:
  hook_callback m_callback;
  compiler::tag_expression m_tags;
};

}  // namespace cwt::details
