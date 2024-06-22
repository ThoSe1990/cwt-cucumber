#pragma once

#include "compiler.hpp"
#include "feature.hpp"

namespace cwt::details::compiler
{

class background : public compiler
{
 public:
  background(feature* enclosing);
  void compile();
};

}  // namespace cwt::details::compiler
