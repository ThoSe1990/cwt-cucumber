#pragma once

#include <vector>

#include "value.hpp"
namespace cwt::details
{

enum class op_code
{
  constant,
  tag,
  nil,
  pop,
  get_var,
  set_var,
  define_var,
  print_line,
  print_linebreak,
  print_double_spaces,
  step_result,
  init_scenario,
  scenario_result,
  jump_if_failed,
  loop,
  call,
  call_step,
  call_step_with_doc_string,
  hook,
  func_return
};

// TODO reserve chunks vectors by default ?
class chunk
{
 public:
  [[nodiscard]] std::size_t size() const noexcept;
  [[nodiscard]] const uint32_t& back() const noexcept;

  [[nodiscard]] std::size_t constants_count() const noexcept;
  [[nodiscard]] std::size_t last_constant() const noexcept;
  [[nodiscard]] value& constant(const std::size_t index);
  [[nodiscard]] value& constants_back() noexcept;

  void push_byte(op_code byte, const std::size_t line);
  void push_byte(uint32_t byte, const std::size_t line);

  [[nodiscard]] op_code instruction(std::size_t index) const;
  [[nodiscard]] uint32_t at(std::size_t index) const;
  [[nodiscard]] uint32_t operator[](std::size_t index) const;

  template <typename Arg>
  [[nodiscard]] std::size_t make_constant(Arg&& arg)
  {
    m_constants.emplace_back(std::forward<Arg>(arg));
    return m_constants.size() - 1;
  }
  template <typename Arg>
  [[nodiscard]] std::size_t make_constant(const Arg& arg)
  {
    m_constants.push_back(value{arg});
    return m_constants.size() - 1;
  }
  class const_iterator
  {
   public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = const uint32_t;
    using difference_type = std::ptrdiff_t;
    using pointer = const uint32_t*;
    using reference = const uint32_t&;

    explicit const_iterator(std::vector<uint32_t>::const_iterator it)
        : m_current(it)
    {
    }
    const uint32_t& operator*() const;
    uint32_t next();
    op_code next_as_instruction();
    const_iterator& operator++();
    bool operator==(const const_iterator& rhs) const;
    bool operator!=(const const_iterator& rhs) const;

   private:
    std::vector<uint32_t>::const_iterator m_current;
  };

  const_iterator cbegin() const;
  const_iterator cend() const;

 private:
  std::vector<uint32_t> m_code;
  std::vector<std::size_t> m_lines;
  value_array m_constants;
};

}  // namespace cwt::details
