#include "examples.hpp"
#include "step.hpp"

namespace cwt::details::compiler
{

examples::examples(feature* parent) : m_parent(parent) {}
void examples::header()
{
  parser& p = m_parent->get_parser();
  p.consume(token_type::vertical, "Expect '|' after examples begin.");
  while (!p.match(token_type::linebreak))
  {
    p.consume(token_type::word, "Expect variable name.");
    // TODO push nil for op_code::define_Var war as init value
    m_parent->emit_bytes(
        op_code::define_var,
        m_parent->get_chunk().make_constant(create_string(p.previous().value)));
    p.consume(token_type::vertical, "Expect '|' after variable.");
  }
}

void examples::body(std::size_t scenario_idx)
{
  parser& p = m_parent->get_parser();
  while (p.is_none_of(token_type::scenario, token_type::scenario_outline,
                      token_type::examples, token_type::tag, token_type::eof))
  {
    while (p.is_none_of(token_type::linebreak, token_type::eof))
    {
      p.consume(token_type::vertical, "Expect '|' at table begin.,");

      if (p.match(token_type::minus))
      {
        // TODO
      }
      // TODO emit_string, emit_long, emit_double, etc. ...
      m_parent->emit_table_value();
      // m_parent->emit_constant(create_string(p.current().value));
      // TODO access variable name (or make constant before and then access the
      // corresponding index it before to access it)
      m_parent->emit_bytes(
          op_code::set_var,
          m_parent->get_chunk().make_constant(create_string("var")));

      p.advance();
      p.consume(token_type::vertical, "Expect '|' after value in table.");
    }
    m_parent->emit_hook(hook_type::before);
    m_parent->emit_bytes(op_code::get_var, scenario_idx);
    m_parent->emit_bytes(op_code::call, 0);
    m_parent->emit_hook(hook_type::after);
    m_parent->emit_byte(op_code::scenario_result);
    p.consume(token_type::linebreak, "Expect linebreak after table row.");
  }
}

}  // namespace cwt::details::compiler