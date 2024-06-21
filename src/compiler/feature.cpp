#include "feature.hpp"
#include "background.hpp"
#include "scenario.hpp"
#include "scenario_outline.hpp"

namespace cwt::details::compiler
{

feature::feature(cucumber* enclosing)
    : m_enclosing(enclosing), compiler(*enclosing)
{
  init();
}

feature::feature(cucumber* enclosing, const cuke::value_array& tags)
    : m_enclosing(enclosing), m_tags(tags), compiler(*enclosing)
{
  init();
}
feature::~feature()
{
  finish_chunk();
  create_call(*m_enclosing, get_chunk());
}

void feature::next()
{
  m_lexer->advance();
  m_lexer->advance_to(token_type::feature, token_type::tag,
                       token_type::scenario, token_type::scenario_outline,
                       token_type::eof);
}

void feature::init()
{
  m_lexer->advance();
  auto [name_idx, location_idx] = create_name_and_location();
  print_name_and_location(name_idx, location_idx);
  m_lexer->advance_until_line_starts_with(
      token_type::scenario, token_type::scenario_outline, token_type::tag,
      token_type::background, token_type::eof);
}

void feature::compile_scenario(const cuke::value_array& tags)
{
  if (tags_valid(tags) && lines_match())
  {
    scenario s(this, tags);
    s.compile();
  }
  else
  {
    next();
  }
}
void feature::compile_scenario_outline(const cuke::value_array& tags)
{
  if (tags_valid(tags))
  {
    scenario_outline s(this, tags);
    s.compile();
  }
  else
  {
    next();
  }
}

void feature::compile()
{
  while (!m_lexer->check(token_type::eof))
  {
    switch (m_lexer->current().type)
    {
      case token_type::tag:
      {
        read_tags();
      }
      break;
      case token_type::scenario:
      {
        compile_scenario(combine(m_tags, take_latest_tags()));
      }
      break;
      case token_type::scenario_outline:
      {
        compile_scenario_outline(combine(m_tags, take_latest_tags()));
      }
      break;
      case token_type::background:
      {
        if (m_background.has_value())
        {
          m_lexer->error_at(m_lexer->current(),
                             "Feature has already a background");
          return;
        }
        else
        {
          background b(this);
          b.compile();
          m_background = b.get_chunk();
        }
      }
      break;
      default:
      {
        return;
      }
    }
    m_lexer->skip_linebreaks();
  } 
}

bool feature::has_background() const noexcept
{
  return m_background.has_value();
}
const chunk& feature::background_chunk() const noexcept
{
  return m_background.value();
}

}  // namespace cwt::details::compiler