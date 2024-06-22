#include "scenario.hpp"
#include "step.hpp"

namespace cwt::details::compiler
{

scenario::scenario(feature* enclosing)
    : m_enclosing(enclosing), compiler(*enclosing)
{
  init();
}
scenario::scenario(feature* enclosing, const cuke::value_array& tags)
    : m_enclosing(enclosing), m_tags(tags), compiler(*enclosing)
{
  init();
}

void scenario::init()
{
  m_lexer->advance();
  auto [name_idx, location_idx] = create_name_and_location();
  print_name_and_location(name_idx, location_idx);
  m_lexer->advance_until_line_starts_with(token_type::step);
  emit_byte(op_code::init_scenario);

  if (m_enclosing->has_background())
  {
    create_call(*this, m_enclosing->background_chunk());
  }
}

scenario::~scenario()
{
  finish_chunk();
  m_enclosing->emit_byte(op_code::reset_context);
  m_enclosing->emit_tags(m_tags);
  m_enclosing->emit_bytes(op_code::hook_before, m_tags.size());

  create_call(*m_enclosing, this->get_chunk());

  m_enclosing->emit_tags(m_tags);
  m_enclosing->emit_bytes(op_code::hook_after, m_tags.size());
}
void scenario::compile()
{
  do
  {
    if (m_lexer->match(token_type::step))
    {
      step s(this);
      s.compile();
    }
    else
    {
      m_lexer->error_at(m_lexer->current(), "Expect StepLine");
      return;
    }
    m_lexer->skip_linebreaks();
  } while (m_lexer->is_none_of(token_type::scenario,
                                token_type::scenario_outline, token_type::tag,
                                token_type::eof));
}

}  // namespace cwt::details::compiler