#pragma once

#include <memory>
#include <stack>

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
  std::unique_ptr<chunk> current;
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
  chunk end_function();

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
  void advance_to(Args... args)
  {
    while (!check(std::forward<Args>(args)...))
    {
      advance();
    }
  }

  void skip_linebreaks();
  void name();
  void scenario();
  void step();
  
  void error_at(const token& t, std::string_view msg) noexcept;

  void emit_byte(uint32_t byte);
  void emit_byte(op_code code);
  void emit_bytes(op_code code, uint32_t byte);
  uint32_t emit_jump();
  void patch_jump(uint32_t offset);

  template <typename Arg>
  void emit_constant(Arg&& arg)
  {
    emit_bytes(op_code::constant,
               m_chunks.top().make_constant(std::forward<Arg>(arg)));
  }

  void feature();

 private:
  scanner m_scanner;
  parser m_parser;
  std::string m_filename;

  std::stack<chunk> m_chunks;

  // compile_unit m_current;
};

}  // namespace cwt::details
