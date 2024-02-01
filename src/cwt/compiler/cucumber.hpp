#pragma once

#include "compiler.hpp"

namespace cwt::details::compiler
{

class cucumber : public compiler
{
 public:
  cucumber(std::string_view source);
  cucumber(std::string_view source, std::string_view filename);
  void compile();
  function make_function() noexcept;

 private:
  void init();

 private:
  value_array m_tags;
};

}  // namespace cwt::details::compiler
