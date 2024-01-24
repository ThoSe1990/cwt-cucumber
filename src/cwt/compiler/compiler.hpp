#pragma once

#include <memory>
#include <string_view>
#include <type_traits>

#include "../token.hpp"
#include "../chunk.hpp"
#include "../parser.hpp"

namespace cwt::details::compiler
{
inline std::string create_string(std::string_view sv)
{
  return std::string(sv);
}
inline std::string create_string(std::string_view begin, std::string_view end)
{
  return std::string(begin.data(), end.data() - begin.data());
}
inline std::string create_string(const token& begin, const token& end)
{
  return create_string(begin.value, end.value);
}

class compiler
{
 public:
  [[nodiscard]] std::string location() const;
  [[nodiscard]] std::pair<std::size_t, std::size_t> create_name_and_location();
  void print_name_and_location(std::size_t name_idx, std::size_t location_idx);
  [[nodiscard]] bool error() const noexcept;
  [[nodiscard]] bool no_error() const noexcept;
  void finish_chunk() noexcept;
  [[nodiscard]] chunk& get_chunk() noexcept;
  [[nodiscard]] parser& get_parser() noexcept { return *m_parser.get(); }

  void emit_byte(uint32_t byte);
  void emit_byte(op_code code);
  void emit_bytes(op_code code, uint32_t byte);
  uint32_t emit_jump();
  void patch_jump(uint32_t offset);
  void emit_hook(hook_type type);
  void emit_table_value();

  template <typename Arg>
  void emit_constant(op_code code, Arg&& arg)
  {
    emit_bytes(code, m_chunk.make_constant(std::forward<Arg>(arg)));
  }
  template <typename Arg>
  void emit_constant(Arg&& arg)
  {
    using Decayed = typename std::decay<Arg>::type;
    emit_bytes(op_code::constant,
               m_chunk.make_constant(std::forward<Decayed>(arg)));
  }


 protected:
  compiler(std::string_view source);
  compiler(std::string_view source, std::string_view filename);

  template <typename Other,
            typename = std::enable_if_t<std::is_base_of_v<compiler, Other>>>
  compiler(const Other& other)
      : m_parser(other.m_parser),
        m_filename(other.m_filename),
        m_chunk(location())
  {
  }

 protected:
  std::shared_ptr<parser> m_parser;

 private:
  std::string m_filename;
  chunk m_chunk;
};

}  // namespace cwt::details::compiler
