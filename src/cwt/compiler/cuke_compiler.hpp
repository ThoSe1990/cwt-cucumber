#pragma once

#include "compiler.hpp"

namespace cwt::details
{

class cuke_compiler : public compiler
{
 public:
  cuke_compiler(std::string_view source);
  cuke_compiler(std::string_view source, std::string_view filename);
  void compile();
};

}  // namespace cwt::details
