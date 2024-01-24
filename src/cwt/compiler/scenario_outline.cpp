#include "scenario_outline.hpp"
#include "examples.hpp"
#include "step.hpp"

namespace cwt::details::compiler
{

scenario_outline::scenario_outline(feature* enclosing)
    : m_enclosing(enclosing), compiler(*enclosing)
{
  m_parser->advance();
  auto [name_idx, location_idx] = create_name_and_location();
  print_name_and_location(name_idx, location_idx);
  m_parser->advance();
  emit_byte(op_code::init_scenario);
}

scenario_outline::~scenario_outline()
{
  // finish_chunk();
  // m_enclosing->emit_hook(hook_type::reset_context);
  // m_enclosing->emit_hook(hook_type::before);
  // m_enclosing->emit_constant(std::make_unique<chunk>(get_chunk()));
  // m_enclosing->emit_constant(op_code::define_var, get_chunk().name());
  // m_enclosing->emit_bytes(op_code::get_var,
  //                         m_enclosing->get_chunk().last_constant());
  // m_enclosing->emit_bytes(op_code::call, 0);
  // m_enclosing->emit_hook(hook_type::after);
  // m_enclosing->emit_byte(op_code::scenario_result);
}
void scenario_outline::compile()
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
      m_parser->error_at(m_parser->current(),
                         "Expect StepLine, Tags or Examples");
      return;
    }
    m_parser->skip_linebreaks();
  } while (m_parser->is_none_of(token_type::examples, token_type::tag));
  finish_chunk(); // FINISH SCENARIO OUTLINE CHUNK HERE
  // and declare it as variable in enclosing 
  m_enclosing->emit_constant(std::make_unique<chunk>(get_chunk()));
  m_enclosing->emit_constant(op_code::define_var, get_chunk().name());
  const std::size_t scenario_idx = m_enclosing->get_chunk().last_constant();
  do
  {
    if (m_parser->match(token_type::examples))
    {
      m_parser->advance();
      m_parser->skip_linebreaks();
      examples e(m_enclosing);
      e.header();
      e.body(scenario_idx);
    }
    else
    {
      m_parser->error_at(m_parser->current(), "Expect Examples table");
      return;
    }
    m_parser->skip_linebreaks();
  } while (m_parser->is_none_of(token_type::scenario,
                                token_type::scenario_outline, token_type::eof));
}

}  // namespace cwt::details::compiler