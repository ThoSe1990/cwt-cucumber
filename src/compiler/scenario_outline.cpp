#include "scenario_outline.hpp"
#include "examples.hpp"
#include "step.hpp"

#include "../util.hpp"

namespace cwt::details::compiler
{

scenario_outline::scenario_outline(feature* enclosing)
    : m_enclosing(enclosing), compiler(*enclosing)
{
  init();
}
scenario_outline::scenario_outline(feature* enclosing, const cuke::value_array& tags)
    : m_enclosing(enclosing), m_tags(tags), compiler(*enclosing)
{
  init();
}

void scenario_outline::init()
{
  m_parser->advance();
  auto [name_idx, location_idx] = create_name_and_location();
  print_name_and_location(name_idx, location_idx);
  m_parser->advance_until_line_starts_with(token_type::step);
  emit_byte(op_code::init_scenario);

  if (m_enclosing->has_background())
  {
    create_call(*this, m_enclosing->background_chunk());
  }
}

void scenario_outline::compile()
{
  compile_steps();
  const std::size_t scenario_idx = make_scenario();
  compile_table(scenario_idx);
}

void scenario_outline::compile_steps()
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

  finish_chunk();
}

std::size_t scenario_outline::make_scenario()
{
  m_enclosing->emit_constant(std::make_unique<chunk>(get_chunk()));
  m_enclosing->emit_constant(op_code::define_var, get_chunk().name());

  return m_enclosing->get_chunk().last_constant();
}

void scenario_outline::compile_table(std::size_t scenario_idx)
{
  do
  {
    switch (m_parser->current().type)
    {
      case token_type::tag:
      {
        read_tags();
      }
      break;
      case token_type::examples:
      {
        compile_examples(scenario_idx);
      }
      break;
      default:
      {
        m_parser->error_at(m_parser->current(), "Expect Examples table");
        return;
      }
    }
    m_parser->skip_linebreaks();
  } while (m_parser->is_none_of(token_type::scenario,
                                token_type::scenario_outline, token_type::eof));
}

void scenario_outline::compile_examples(std::size_t scenario_idx)
{
  const cuke::value_array all_tags = combine(m_tags, take_latest_tags());
  if (tags_valid(all_tags))
  {
    examples e(m_enclosing, all_tags);
    e.header();
    e.body(scenario_idx);
  }
  else
  {
    m_parser->advance();
    m_parser->advance_to(token_type::scenario, token_type::scenario_outline,
                         token_type::examples, token_type::tag,
                         token_type::feature, token_type::eof);
  }
}

}  // namespace cwt::details::compiler