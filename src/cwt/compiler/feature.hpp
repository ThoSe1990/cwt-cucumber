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
  template <typename S>
  void do_compile()
  {
    if (tags_valid())
    {
      S s(this);
      s.compile();
    }
    else
    {
      m_parser->advance();
      m_parser->advance_to(token_type::feature, token_type::tag,
                           token_type::scenario, token_type::scenario_outline,
                           token_type::eof);
    }
  }

 private:
  cucumber* m_enclosing;
};

}  // namespace cwt::details::compiler
