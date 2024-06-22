#pragma once

#include "feature.hpp"

namespace cwt::details::compiler
{

class scenario : public compiler
{
 public:
  scenario(feature* enclosing);
  scenario(feature* enclosing, const cuke::value_array& tags);
  ~scenario();
  void compile();

 private:
  void init();

 private:
  feature* m_enclosing;
  cuke::value_array m_tags;
};

}  // namespace cwt::details::compiler
