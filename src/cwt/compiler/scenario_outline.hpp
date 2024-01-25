#pragma once

#include "feature.hpp"

namespace cwt::details::compiler
{

class scenario_outline : public compiler
{
 public:
  scenario_outline(feature* enclosing);
  void compile();

 private:
  void compile_steps();
  void compile_examples(std::size_t scenario_idx);
  std::size_t make_scenario();

 private:
  feature* m_enclosing;
};

}  // namespace cwt::details::compiler
