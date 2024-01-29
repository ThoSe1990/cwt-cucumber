#pragma once

#include <type_traits>

#include "scenario.hpp"
#include "scenario_outline.hpp"

namespace cwt::details::compiler
{

template <typename Parent,
          typename = std::enable_if_t<std::is_same_v<Parent, scenario> ||
                                      std::is_same_v<Parent, scenario_outline>>>
class step
{
 public:
  step(Parent* parent)
      : m_parent(parent),
        m_location_idx(
            m_parent->get_chunk().make_constant(m_parent->location())),
        m_begin(m_parent->get_parser().current())
  {
  }

  void compile()
  {
    token end = process_step();

    std::size_t name_idx =
        m_parent->get_chunk().make_constant(create_string(m_begin, end));

    uint32_t jump = m_parent->emit_jump();

    m_parent->emit_hook(hook_type::before_step);
    m_parent->emit_bytes(op_code::call_step, name_idx);
    m_parent->emit_hook(hook_type::after_step);

    m_parent->patch_jump(jump);

    m_parent->emit_bytes(op_code::constant, name_idx);
    m_parent->emit_bytes(op_code::constant, m_location_idx);
    m_parent->emit_byte(op_code::step_result);
  }

 private:
  std::string_view get_var_name(std::string_view str)
  {
    auto without_ankle_brackets = [](std::string_view str)
    { return str.substr(1, str.size() - 2); };
    return without_ankle_brackets(str);
  }

  token process_step()
  {
    while (m_parent->get_parser().is_none_of(token_type::linebreak,
                                             token_type::eof))
    {
      m_parent->get_parser().advance();
      if (m_parent->get_parser().match(token_type::variable))
      {
        std::string_view name =
            get_var_name(m_parent->get_parser().previous().value);
        m_parent->emit_bytes(
            op_code::get_var,
            m_parent->get_chunk().make_constant(create_string(name)));
      }
    }
    return m_parent->get_parser().previous();
  }

 private:
  Parent* m_parent;
  std::size_t m_location_idx;
  token m_begin;
};

}  // namespace cwt::details::compiler