#pragma once

#include "feature.hpp"

namespace cwt::details::compiler
{

class scenario_outline : public compiler
{
 public:
  scenario_outline(feature* enclosing);
  ~scenario_outline();
  void compile();

 private:
  feature* m_enclosing;
  std::size_t m_name_idx;
};

}  // namespace cwt::details::compiler
