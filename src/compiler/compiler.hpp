#pragma once

#include <memory>
#include <string_view>
#include <type_traits>

#include "../token.hpp"
#include "../chunk.hpp"
#include "../lexer.hpp"
#include "../options.hpp"

#include "tags.hpp"

namespace cwt::details::compiler
{

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
  [[nodiscard]] lexer& get_lexer() noexcept { return *m_lexer.get(); }

  void set_options(const options& opts);
  const options& get_options() const;
  [[nodiscard]] bool lines_match() const noexcept;

  void read_tags();
  [[nodiscard]] cuke::value_array take_latest_tags();
  [[nodiscard]] bool tags_valid(const cuke::value_array& tags);

  void emit_byte(uint32_t byte);
  void emit_byte(op_code code);
  void emit_bytes(op_code code, uint32_t byte);
  uint32_t emit_jump();
  void patch_jump(uint32_t offset);
  void emit_tags(const cuke::value_array& tags);

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
  compiler(const file& feature_file);

  template <typename Other,
            typename = std::enable_if_t<std::is_base_of_v<compiler, Other>>>
  compiler(const Other& other)
      : m_lexer(other.m_lexer),
        m_options(other.m_options),
        m_lines(other.m_lines),
        m_filename(other.m_filename),
        m_chunk(location()),
        m_latest_tags(other.m_latest_tags)
  {
  }

 protected:
  std::shared_ptr<lexer> m_lexer;
  std::shared_ptr<options> m_options;

 private:
  std::vector<unsigned long> m_lines;
  std::string m_filename;
  chunk m_chunk;
  std::shared_ptr<cuke::value_array> m_latest_tags =
      std::make_unique<cuke::value_array>();
};

}  // namespace cwt::details::compiler
