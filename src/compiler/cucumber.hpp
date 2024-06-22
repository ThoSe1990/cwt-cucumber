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
  cucumber(const file& feature_file);

  void compile();
  function make_function() noexcept;
  [[nodiscard]] double compile_time() const noexcept;

 private:
  void init();
  void internal_compile();
  void compile_feature();

 private:
  double m_compile_time{0.0};
};

}  // namespace cwt::details::compiler
