#pragma once

#include "feature.hpp"

namespace cwt::details::compiler
{

class examples
{


 public:
  examples(feature* f);
  void header();
  void body(std::size_t scenario_idx);

  private:
    feature* m_parent;

};

}  // namespace cwt::details::compiler
