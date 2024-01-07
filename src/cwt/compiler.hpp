#pragma once

#include <memory>

#include "scanner.hpp"
#include "token.hpp"
#include "value.hpp"
#include "chunk.hpp"
namespace cwt::details
{

struct parser
{
  token current;
  token previous;
  bool error{false};
};

class compiler
{
 public:
  compiler(std::string_view source);
  [[nodiscard]] function compile();
  [[nodiscard]] bool no_error() const noexcept;

 private:
  [[nodiscard]] function start_function(const std::string_view name);
  void end_function();
  void consume(token_type type, std::string_view msg);
  void advance();

  void error_at(const token& t, std::string_view msg) noexcept;

  void emit_byte(uint32_t byte);
  void emit_byte(op_code code);
  void emit_bytes(op_code code, uint32_t byte);
  void emit_function(function&& func);

  template <typename Arg>
  void emit_constant(Arg&& arg)
  {
    emit_bytes(op_code::constant,
               m_current->make_constant(std::forward<Arg>(arg)));
  }

  void feature();

 private:
  scanner m_scanner;
  parser m_parser;
  chunk* m_current;
  chunk* m_enclosing;
};

}  // namespace cwt::details
