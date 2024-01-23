#pragma once

#include "feature.hpp"

namespace cwt::details::compiler
{

class scenario : public compiler
{
 public:
  scenario(feature* enclosing);
  ~scenario();
  void compile();

 private:
  feature* m_enclosing;
};

}  // namespace cwt::details
