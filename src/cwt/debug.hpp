#pragma once

#ifdef PRINT_STACK

#include <string>
#include <iomanip>
#include <iostream>

#include "chunk.hpp"

namespace cwt::details
{

static void print_value(const value& v)
{
  switch (v.type())
  {
    case value_type::integral:
      std::cout << v.as<long>();
      break;
    case value_type::floating:
      std::cout << v.as<double>();
      break;
    case value_type::boolean:
      std::cout << v.as<bool>();
      break;
    case value_type::string:
      std::cout << v.as<std::string>();
      break;
    case value_type::function:
      std::cout << '<' << v.as<function>()->name() << '>';
      break;
    case value_type::step:
      std::cout << "step function";
      break;
    case value_type::nil:
      std::cout << "nil";
  }
}

static std::size_t constant_instruction(std::string_view name, const chunk& c,
                                        std::size_t offset)
{
  uint32_t idx = c[offset + 1];
  std::cout << ' ' << name << '\t' << idx << " '";
  print_value(c.constant(idx));
  std::cout << "'\n";
  return offset + 2;
}
static std::size_t byte_instruction(std::string_view name, const chunk& c,
                                    std::size_t offset)
{
  uint32_t idx = c[offset + 1];
  std::cout << ' ' << name << '\t' << idx << '\n';
  return offset + 2;
}

static std::size_t simple_instruction(std::string_view name, std::size_t offset)
{
  std::cout << ' ' << name << '\n';
  return offset + 1;
}
static std::size_t jump_instruction(std::string_view name, int sign,
                                    const chunk& c, std::size_t offset)
{
  return offset + 1;
}
static std::size_t disassemble_instruction(const chunk& c, std::size_t offset)
{
  std::cout << std::right << std::setw(4) << std::setfill('0') << offset;
  if (offset > 0 && c.lines(offset) == c.lines(offset - 1))
  {
    std::cout << "\t| ";
  }
  else
  {
    std::cout << '\t' << c.lines(offset) << ' ';
  }

  switch (static_cast<op_code>(c[offset]))
  {
    case op_code::constant:
      return constant_instruction("op_code::constant", c, offset);
    case op_code::tag:
      return constant_instruction("op_code::tag", c, offset);
    case op_code::pop:
      return simple_instruction("op_code::pop", offset);
    case op_code::get_var:
      return constant_instruction("op_code::get_var", c, offset);
    case op_code::set_var:
      return constant_instruction("op_code::set_var", c, offset);
    case op_code::define_var:
      return constant_instruction("op_code::define_var", c, offset);
    case op_code::print_linebreak:
      return simple_instruction("op_code::print_linebreak", offset);
    case op_code::print_indent:
      return simple_instruction("op_code::print_indent", offset);
    case op_code::print:
      return byte_instruction("op_code::print", c, offset);
    case op_code::println:
      return byte_instruction("op_code::println", c, offset);
    case op_code::step_result:
      return simple_instruction("op_code::step_result", offset);
    case op_code::init_scenario:
      return simple_instruction("op_code::init_scenario", offset);
    case op_code::scenario_result:
      return simple_instruction("op_code::scenario_result", offset);
    case op_code::jump_if_failed:
      return byte_instruction("op_code::jump_if_failed", c, offset);
    case op_code::call:
      return byte_instruction("op_code::call", c, offset);
    case op_code::call_step:
      return constant_instruction("op_code::call_step", c, offset);
    case op_code::call_step_with_doc_string:
      return constant_instruction("op_code::call_step_with_doc_string", c,
                                  offset);
    case op_code::hook_before:
      return byte_instruction("op_code::hook_before", c, offset);
    case op_code::hook_after:
      return byte_instruction("op_code::hook_after", c, offset);
    case op_code::hook_before_step:
      return simple_instruction("op_code::hook_before_step", offset);
    case op_code::hook_after_step:
      return simple_instruction("op_code::hook_after_step", offset);
    case op_code::reset_context:
      return simple_instruction("op_code::reset_context", offset);
    case op_code::func_return:
      return simple_instruction("op_code::func_return", offset);
    default:
      std::cout << "Unknown op_code: " << c[offset] << '\n';
      return offset + 1;
  }
}
static void disassemble_chunk(const chunk& c, const std::string& name)
{
  std::cout << "== " << name << " ==\n";
  for (std::size_t offset = 0; offset < c.size();)
  {
    offset = disassemble_instruction(c, offset);
  }
}

}  // namespace cwt::details

#endif