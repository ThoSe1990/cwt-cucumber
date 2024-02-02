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

  hook(const hook&) = default;
  hook(hook&&) = default;
  hook& operator=(const hook&) = default;
  hook& operator=(hook&&) = default;

  void call(argc n, argv tags) const 
  {
    // if (m_tags.evaluate(n, tags))
    {
      m_callback(); 
    }
  }
  void operator()() const { m_callback(); }

 private:
  hook_callback m_callback;
  // compiler::tag_expression m_tags;
  std::string m_tags;
};

} // namespace cwt::details
