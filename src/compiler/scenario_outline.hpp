#pragma once

#include "feature.hpp"

namespace cwt::details::compiler
{

class scenario_outline : public compiler
{
 public:
  scenario_outline(feature* enclosing);
  scenario_outline(feature* enclosing, const value_array& tags);

  void compile();

 private:
  void init();
  void compile_steps();
  void compile_table(std::size_t scenario_idx);
  void compile_examples(std::size_t scenario_idx);
  std::size_t make_scenario();

 private:
  feature* m_enclosing;
  value_array m_tags;
};

}  // namespace cwt::details::compiler
