#pragma once

#include "feature.hpp"

namespace cwt::details::compiler
{

class scenario : public compiler
{
 public:
  scenario(feature* enclosing);
  ~scenario();
    void set_tags(const value_array& tags)
  {
    m_tags = tags;
  }
  void compile();

 private:
  feature* m_enclosing;
  value_array m_tags;
};

}  // namespace cwt::details
