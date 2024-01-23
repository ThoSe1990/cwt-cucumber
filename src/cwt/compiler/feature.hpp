#pragma once

#include "cucumber.hpp"

namespace cwt::details::compiler
{

class feature : public compiler
{
 public:
  feature(cucumber* enclosing);
  ~feature();
  void compile();

 private:
  cucumber* m_enclosing;
};

}  // namespace cwt::details
