#pragma once

#include "tags.hpp"

namespace cuke::internal
{
using hook_callback = void (*)();
struct hook
{
 public:
  hook(hook_callback cb) : m_callback(cb) {}
  hook(hook_callback cb, std::string_view tags) : m_callback(cb), m_tags(tags)
  {
  }
  [[nodiscard]] bool valid_tag(const std::vector<std::string>& tags) const
  {
    if (m_tags.empty())
    {
      return true;
    }
    else
    {
      return tags.empty() ? false : m_tags.evaluate(tags);
    }
  }
  void call() const { m_callback(); }

 private:
  hook_callback m_callback;
  cuke::internal::tag_expression m_tags;
};

}  // namespace cuke::internal
