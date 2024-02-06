#include "scenario.hpp"
#include "step.hpp"

namespace cwt::details::compiler
{

scenario::scenario(feature* enclosing)
    : m_enclosing(enclosing), compiler(*enclosing)
{
  init();
}
scenario::scenario(feature* enclosing, const value_array& tags)
    : m_enclosing(enclosing), m_tags(tags), compiler(*enclosing)
{
  init();
}

void scenario::init()
{
  m_parser->advance();
  auto [name_idx, location_idx] = create_name_and_location();
  emit_byte(op_code::print_linebreak);
  print_name_and_location(name_idx, location_idx);
  m_parser->advance();
  emit_byte(op_code::init_scenario);
}

scenario::~scenario()
{
  finish_chunk();
  m_enclosing->emit_byte(op_code::reset_context);
  m_enclosing->emit_tags(m_tags);
  m_enclosing->emit_bytes(op_code::hook_before, m_tags.size());
  m_enclosing->emit_constant(std::make_unique<chunk>(get_chunk()));
  m_enclosing->emit_constant(op_code::define_var, get_chunk().name());
  m_enclosing->emit_bytes(op_code::get_var,
                          m_enclosing->get_chunk().last_constant());
  m_enclosing->emit_bytes(op_code::call, 0);
  m_enclosing->emit_tags(m_tags);
  m_enclosing->emit_bytes(op_code::hook_after, m_tags.size());
  m_enclosing->emit_byte(op_code::scenario_result);
}
void scenario::compile()
{
  do
  {
    if (m_parser->match(token_type::step))
    {
      step s(this);
      s.compile();
    }
    else
    {
      m_parser->error_at(m_parser->current(), "Expect StepLine");
    }
    m_parser->skip_linebreaks();
  } while (m_parser->is_none_of(token_type::scenario,
                                token_type::scenario_outline, token_type::tag,
                                token_type::eof));
}

}  // namespace cwt::details::compiler