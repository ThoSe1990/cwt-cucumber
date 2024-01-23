#pragma once

#include <type_traits>

#include "scenario.hpp"

namespace cwt::details::compiler
{

template <typename Parent,
          typename = std::enable_if_t<std::is_same_v<
              Parent, scenario> /* || std::is_same_v<Parent, scenario_outline>
                                 */>>
class step
{
 public:
  step(Parent* parent) : m_parent(parent) {}

  void compile()
  {
    auto [name_idx, location_idx] = m_parent->create_name_and_location();

    uint32_t jump = m_parent->emit_jump();

    m_parent->emit_hook(hook_type::before_step);
    m_parent->emit_bytes(op_code::call_step, name_idx);
    m_parent->emit_hook(hook_type::after_step);

    m_parent->patch_jump(jump);

    m_parent->emit_bytes(op_code::constant, name_idx);
    m_parent->emit_bytes(op_code::constant, location_idx);

    m_parent->emit_byte(op_code::step_result);
  }

 private:
  Parent* m_parent;
};

}  // namespace cwt::details