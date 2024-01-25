#include "examples.hpp"
#include "step.hpp"

namespace cwt::details::compiler
{

examples::examples(feature* parent) : m_parent(parent) {}
void examples::header()
{
  m_indices.clear();
  parser& p = m_parent->get_parser();
  p.consume(token_type::vertical, "Expect '|' after examples begin.");
  while (!p.match(token_type::linebreak))
  {
    if (p.check(token_type::vertical))
    {
      p.error_at(p.current(), "Expect variable name after '|'");
      return;
    }

    std::size_t idx = make_variable();

    m_indices.push_back(idx);
    m_parent->emit_constant(nil_value{});
    m_parent->emit_bytes(op_code::define_var, idx);
  }
}

void examples::body(std::size_t scenario_idx)
{
  parser& p = m_parent->get_parser();
  while (p.is_none_of(token_type::scenario, token_type::scenario_outline,
                      token_type::examples, token_type::tag, token_type::eof))
  {
    p.consume(token_type::vertical, "Expect '|' at table begin.");

    for (const std::size_t idx : m_indices)
    {
      m_parent->emit_table_value();
      m_parent->emit_bytes(op_code::set_var, idx);
      p.advance();
      p.consume(token_type::vertical, "Expect '|' after value in table.");
    }
    p.consume(token_type::linebreak, "Expect linebreak after table row.");

    m_parent->emit_hook(hook_type::before);
    m_parent->emit_bytes(op_code::get_var, scenario_idx);
    m_parent->emit_bytes(op_code::call, 0);
    m_parent->emit_hook(hook_type::after);
    m_parent->emit_byte(op_code::scenario_result);
  }
}

std::size_t examples::make_variable()
{
  token begin = m_parent->get_parser().current();
  m_parent->get_parser().advance_to(token_type::vertical);
  m_parent->get_parser().consume(token_type::vertical,
                                 "Expect '|' after variable.");

  return m_parent->get_chunk().make_constant(
      create_string(begin, m_parent->get_parser().previous()));
}

}  // namespace cwt::details::compiler