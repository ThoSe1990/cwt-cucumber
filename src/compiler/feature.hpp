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
  feature(cucumber* enclosing, const cuke::value_array& tags);
  ~feature();

  void compile();
  [[nodiscard]] bool has_background() const noexcept;
  [[nodiscard]] const chunk& background_chunk() const noexcept;

 private:
  void init();
  void next();
  
  void compile_scenario(const cuke::value_array &tags);
  void compile_scenario_outline(const cuke::value_array &tags);

 private:
  cucumber* m_enclosing;
  cuke::value_array m_tags;
  std::optional<chunk> m_background;
};

}  // namespace cwt::details::compiler
