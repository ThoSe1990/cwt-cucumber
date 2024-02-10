#pragma once

#include "compiler.hpp"

namespace cwt::details::compiler
{

template <typename C,
          typename = std::enable_if_t<std::is_base_of_v<compiler, C>>>
inline void create_call(C& caller, const chunk& callee_chunk)
{
  caller.emit_constant(std::make_unique<chunk>(callee_chunk));
  caller.emit_constant(op_code::define_var, callee_chunk.name());
  caller.emit_bytes(op_code::get_var, caller.get_chunk().last_constant());
  caller.emit_bytes(op_code::call, 0);
}

class cucumber : public compiler
{
 public:

  cucumber(std::string_view source);
  cucumber(std::string_view source, const std::vector<unsigned long>& lines);
  cucumber(std::string_view source, std::string_view filename);
  cucumber(std::string_view source, std::string_view filename, const std::vector<unsigned long>& lines);
  
  void compile();
  function make_function() noexcept;

 private:
  void init();
  void compile_feature();
};

}  // namespace cwt::details::compiler
