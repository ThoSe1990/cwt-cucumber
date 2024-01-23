#include "scenario.hpp"

namespace cwt::details
{

scenario::scenario(feature* enclosing)
    : m_enclosing(enclosing), compiler(*enclosing)
{
  m_parser->advance();
  auto [name_idx, location_idx] = create_name_and_location();
  print_name_and_location(name_idx, location_idx);
  m_parser->advance();
  emit_byte(op_code::init_scenario);
}

scenario::~scenario()
{
  finish_chunk();
  m_enclosing->emit_hook(hook_type::reset_context);
  m_enclosing->emit_hook(hook_type::before);
  m_enclosing->emit_constant(std::make_unique<chunk>(get_chunk()));
  m_enclosing->emit_constant(op_code::define_var, get_chunk().name());
  m_enclosing->emit_bytes(op_code::get_var,
                          m_enclosing->get_chunk().last_constant());
  m_enclosing->emit_bytes(op_code::call, 0);
  m_enclosing->emit_hook(hook_type::after);
  m_enclosing->emit_byte(op_code::scenario_result);
}
void scenario::compile()
{
  do
  {
    if (m_parser->match(token_type::step))
    {
      auto [name_idx, location_idx] = create_name_and_location();

      uint32_t jump = emit_jump();

      emit_hook(hook_type::before_step);
      emit_bytes(op_code::call_step, name_idx);
      emit_hook(hook_type::after_step);

      patch_jump(jump);
      emit_bytes(op_code::step_result, name_idx);
    }
    else
    {
      m_parser->error_at(m_parser->current(), "Expect StepLine");
    }
    m_parser->skip_linebreaks();
  } while (!m_parser->check(token_type::scenario) &&
           !m_parser->check(token_type::eof));
}

}  // namespace cwt::details