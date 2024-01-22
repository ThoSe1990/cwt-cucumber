#pragma once

#include "cuke_compiler.hpp"

namespace cwt::details
{

class feature : public compiler
{
 public:
  feature(cuke_compiler* enclosing);
  void compile();

 private:
  cuke_compiler* m_enclosing;
};

}  // namespace cwt::details
