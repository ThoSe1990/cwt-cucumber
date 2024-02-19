#pragma once

#include <type_traits>

#include "background.hpp"
#include "scenario.hpp"
#include "scenario_outline.hpp"

namespace cwt::details::compiler
{

template <typename Parent,
          typename = std::enable_if_t<std::is_same_v<Parent, scenario> ||
                                      std::is_same_v<Parent, background> ||
                                      std::is_same_v<Parent, scenario_outline>>>
class step
{
 public:
  step(Parent* parent)
      : m_parent(parent),
        m_location_idx(
            m_parent->get_chunk().make_constant(m_parent->location())),
        m_begin(m_parent->get_parser().current())
  {
  }

  void compile() const
  {
    advance_to_linebreak();
    token end = m_parent->get_parser().previous();

    m_parent->get_parser().skip_linebreaks();

    if (m_parent->get_parser().match(token_type::doc_string))
    {
      end = m_parent->get_parser().previous();
    }
    else if (m_parent->get_parser().check(token_type::vertical))
    {
      datatable();
      end = m_parent->get_parser().previous();
    }

    std::size_t name_idx =
        m_parent->get_chunk().make_constant(create_string(m_begin, end));

    uint32_t jump = m_parent->emit_jump();

    m_parent->emit_byte(op_code::hook_before_step);
    m_parent->emit_bytes(op_code::call_step, name_idx);
    m_parent->emit_byte(op_code::hook_after_step);

    m_parent->patch_jump(jump);

    if (!m_parent->get_options().quiet)
    {
      m_parent->emit_bytes(op_code::constant, name_idx);
      m_parent->emit_bytes(op_code::constant, m_location_idx);
      m_parent->emit_byte(op_code::print_step_result);
    }
  }

 private:
  std::string_view get_var_name(std::string_view str) const
  {
    auto without_ankle_brackets = [](std::string_view str)
    { return str.substr(1, str.size() - 2); };
    return without_ankle_brackets(str);
  }

  void advance_to_linebreak() const
  {
    while (not_at_linebreak())
    {
      m_parent->get_parser().advance();
      if (m_parent->get_parser().match(token_type::variable))
      {
        emit_variable();
      }
    }
  }

  void emit_variable() const
  {
    std::string_view name =
        get_var_name(m_parent->get_parser().previous().value);
    m_parent->emit_bytes(op_code::get_var, m_parent->get_chunk().make_constant(
                                               create_string(name)));
  }

  bool not_at_linebreak() const
  {
    return m_parent->get_parser().is_none_of(token_type::linebreak,
                                             token_type::eof);
  }

  void datatable() const
  {
    const std::size_t elements_in_row = datatable_row();
    parser& p = m_parent->get_parser();
    p.skip_linebreaks();

    while (p.is_none_of(token_type::step, token_type::scenario,
                        token_type::scenario_outline, token_type::examples,
                        token_type::tag, token_type::eof))
    {
      if (datatable_row() != elements_in_row)
      {
        p.error_at(p.current(), "Inconsistent cell count within data table");
        return ;
      }
      p.skip_linebreaks();
    }
  }

  std::size_t datatable_row() const
  {
    std::size_t element_count = 0;
    parser& p = m_parent->get_parser();
    p.consume(token_type::vertical, "Expect '|' at table row begin.");
    while (!p.match(token_type::linebreak))
    {
      const std::vector<token> tokens =
          p.collect_tokens_to(token_type::vertical);
      p.advance_to(token_type::vertical);
      ++element_count;
      p.consume(token_type::vertical, "Expect '|' after value in data table.");
    }
    return element_count;
  }

 private:
  Parent* m_parent;
  std::size_t m_location_idx;
  token m_begin;
};

}  // namespace cwt::details::compiler