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

struct compile_unit
{
  function func;
  std::unique_ptr<compile_unit> enclosing;
};

class compiler
{
 public:
  compiler(std::string_view source);
  compiler(std::string_view source, std::string_view filename);
  [[nodiscard]] function compile();
  [[nodiscard]] bool error() const noexcept;
  [[nodiscard]] bool no_error() const noexcept;

 private:
  void start_function(const std::string& name);
  std::unique_ptr<compile_unit> end_function();

  void consume(token_type type, std::string_view msg);
  template <typename... Args>
  [[nodiscard]] bool check(token_type type, Args... args)
  {
    return check(type) || check(args...);
  }
  [[nodiscard]] bool check(token_type type);
  [[nodiscard]] bool match(token_type type);

  void advance();
  template <typename... Args>
  void advance_until(Args... args)
  {
    while (!check(std::forward<Args>(args)...))
    {
      advance();
    }
  }

  void skip_linebreaks();
  void name();
  void scenario();
  void parse_scenarios();

  void error_at(const token& t, std::string_view msg) noexcept;

  void emit_byte(uint32_t byte);
  void emit_byte(op_code code);
  void emit_bytes(op_code code, uint32_t byte);
  void emit_function(function&& func);

  template <typename Arg>
  void emit_constant(Arg&& arg)
  {
    emit_bytes(op_code::constant,
               m_current->func.chunk_ptr->make_constant(std::forward<Arg>(arg)));
  }

  void feature();

 private:
  scanner m_scanner;
  parser m_parser;
  std::string m_filename;

  std::unique_ptr<compile_unit> m_current;
};

}  // namespace cwt::details
