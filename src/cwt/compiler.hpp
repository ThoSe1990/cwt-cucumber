#pragma once

#include <memory>
#include <stack>

#include "chunk.hpp"
#include "parser.hpp"


/*

TODO hooks args is for tags
-> in version 1.0.0 tag evaluation is at runtime (in vm)
        and this is reasonable because when compiling cucumber 
        users hooks are not there, steps/hooks are implemented later
-> change hook types from string to an enum
-> more emit functions to structure code (emit_hook(type))

*/

namespace cwt::details
{

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

  void feature();
  void scenario();
  void step();

  void name();

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

  void emit_hook(hook_type type);


 private:
  parser m_parser;
  std::string m_filename;
  std::stack<chunk> m_chunks;
};

}  // namespace cwt::details
