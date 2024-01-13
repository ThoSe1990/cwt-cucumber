#pragma once

#include <memory>
#include <stack>

#include "chunk.hpp"
#include "parser.hpp"
namespace cwt::details
{

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

  void name();
  void scenario();
  void step();

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
  parser m_parser;
  std::string m_filename;
  std::stack<chunk> m_chunks;
};

}  // namespace cwt::details
