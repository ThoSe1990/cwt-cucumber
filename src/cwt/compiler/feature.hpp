#pragma once

#include <optional>

#include "cucumber.hpp"
#include "../value.hpp"

namespace cwt::details::compiler
{

class feature : public compiler
{
 public:
  feature(cucumber* enclosing);
  feature(cucumber* enclosing, const value_array& tags);
  ~feature();

  void compile();
  bool has_background() const noexcept;
  const chunk& background_chunk() const noexcept;

 private:
  void init();
  template <typename S>
  void do_compile()
  {
    value_array all_tags = combine(m_tags, take_latest_tags());
    if (tags_valid(all_tags))
    {
      S s(this, all_tags);
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
  value_array m_tags;
  std::optional<chunk> m_background;
};

}  // namespace cwt::details::compiler
