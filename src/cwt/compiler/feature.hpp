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
  void next();
  
  void compile_scenario(const value_array &tags);
  void compile_scenario_outline(const value_array &tags);

  template <typename S>
  void do_compile()
  {
    value_array all_tags = combine(m_tags, take_latest_tags());
    if (tags_valid(all_tags) && lines_match())
    {
      S s(this, all_tags);
      s.compile();
    }
    else
    {
      next();
    }
  }

 private:
  cucumber* m_enclosing;
  value_array m_tags;
  std::optional<chunk> m_background;
};

}  // namespace cwt::details::compiler
