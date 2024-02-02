#pragma once

#include <memory>
#include <string_view>
#include <type_traits>

#include "../token.hpp"
#include "../chunk.hpp"
#include "../parser.hpp"

#include "tags.hpp"

namespace cwt::details::compiler
{
inline std::string create_string(std::string_view sv)
{
  return std::string(sv);
}
inline std::string create_string(std::string_view begin, std::string_view end)
{
  return std::string(begin.data(), end.data() + end.size());
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
  
  void set_tag_expression(std::string_view expression);
  void read_tags();
  void clear_tags();
  [[nodiscard]] std::size_t tags_count();
  [[nodiscard]] bool tags_valid();


  void emit_byte(uint32_t byte);
  void emit_byte(op_code code);
  void emit_bytes(op_code code, uint32_t byte);
  uint32_t emit_jump();
  void patch_jump(uint32_t offset);
  void emit_table_value();
  void emit_tags();

  template <typename Arg>
  void emit_constant(op_code code, Arg&& arg)
  {
    using RemovedRef = std::remove_reference_t<Arg>;
    emit_bytes(code, m_chunk.make_constant(std::forward<RemovedRef>(arg)));
  }
  template <typename Arg>
  void emit_constant(Arg&& arg)
  {
    using RemovedRef = std::remove_reference_t<Arg>;
    emit_bytes(op_code::constant,
               m_chunk.make_constant(std::forward<RemovedRef>(arg)));
  }

 protected:
  compiler(std::string_view source);
  compiler(std::string_view source, std::string_view filename);

  template <typename Other,
            typename = std::enable_if_t<std::is_base_of_v<compiler, Other>>>
  compiler(const Other& other)
      : m_parser(other.m_parser),
        m_tag_expression(other.m_tag_expression),
        m_tags(other.m_tags),
        m_filename(other.m_filename),
        m_chunk(location())
  {
  }

 protected:
  std::shared_ptr<parser> m_parser;
  std::shared_ptr<tag_expression> m_tag_expression;
  std::shared_ptr<value_array> m_tags = std::make_shared<value_array>();

 private:
  std::string m_filename;
  // value_array m_tags;
  chunk m_chunk;
};

}  // namespace cwt::details::compiler
