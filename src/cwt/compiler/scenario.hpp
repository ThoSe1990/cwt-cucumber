#pragma once

#include "feature.hpp"

namespace cwt::details
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
