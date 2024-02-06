#include "examples.hpp"
#include "step.hpp"

namespace cwt::details::compiler
{

examples::examples(feature* f, const value_array& tags) : m_feature(f), m_tags(tags)
{
  m_feature->get_parser().advance();
  m_feature->get_parser().skip_linebreaks();
}
void examples::header()
{
  m_variables.clear();
  parser& p = m_feature->get_parser();
  p.consume(token_type::vertical, "Expect '|' after examples begin.");
  while (!p.match(token_type::linebreak))
  {
    if (p.check(token_type::vertical))
    {
      p.error_at(p.current(), "Expect variable name after '|'");
      return;
    }
    std::size_t var = make_variable();
    m_variables.push_back(var);
    m_feature->emit_constant(nil_value{});
    m_feature->emit_bytes(op_code::define_var, var);
  }
}

void examples::body(std::size_t scenario_idx)
{
  parser& p = m_feature->get_parser();
  while (p.is_none_of(token_type::scenario, token_type::scenario_outline,
                      token_type::examples, token_type::tag, token_type::eof))
  {
    process_table_row();
    create_call(scenario_idx);
    p.skip_linebreaks();
  }
}

std::size_t examples::make_variable()
{
  token begin = m_feature->get_parser().current();
  m_feature->get_parser().advance_to(token_type::vertical);
  token end = m_feature->get_parser().previous();
  m_feature->get_parser().consume(token_type::vertical,
                                 "Expect '|' after variable.");
  return m_feature->get_chunk().make_constant(create_string(begin, end));
}

void examples::process_table_row()
{
  parser& p = m_feature->get_parser();
  p.consume(token_type::vertical, "Expect '|' at table begin.");
  for (const std::size_t variable_index : m_variables)
  {
    m_feature->emit_table_value();
    m_feature->emit_bytes(op_code::set_var, variable_index);
    p.advance();
    p.consume(token_type::vertical, "Expect '|' after value in table.");
  }
  p.consume(token_type::linebreak, "Expect linebreak after table row.");
}
void examples::create_call(std::size_t scenario_idx)
{
  m_feature->emit_tags(m_tags);
  m_feature->emit_bytes(op_code::hook_before, m_tags.size());
  m_feature->emit_bytes(op_code::get_var, scenario_idx);
  m_feature->emit_bytes(op_code::call, 0);
  m_feature->emit_tags(m_tags);
  m_feature->emit_bytes(op_code::hook_after, m_tags.size());
  m_feature->emit_byte(op_code::scenario_result);
}

}  // namespace cwt::details::compiler