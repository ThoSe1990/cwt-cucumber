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

class chunk
{
 public:
  [[nodiscard]] std::size_t size() const noexcept;
  [[nodiscard]] std::size_t constants_count() const noexcept;
  void push_byte(op_code byte, std::size_t line);
  void push_byte(uint32_t byte, std::size_t line);
  void add_constant(value&& v);

  class iterator
  {
   public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = const uint32_t;
    using difference_type = std::ptrdiff_t;
    using pointer = const uint32_t*;
    using reference = const uint32_t&;

    explicit iterator(std::vector<uint32_t>::const_iterator it) : m_current(it) {}
    const uint32_t& operator*() const;
    iterator& operator++();
    bool operator==(const iterator& rhs) const;
    bool operator!=(const iterator& rhs) const;

   private:
    std::vector<uint32_t>::const_iterator m_current;
  };

  iterator begin() const;
  iterator end() const;

 private:
 private:
  std::size_t m_size;
  std::vector<uint32_t> m_code;
  std::vector<std::size_t> m_lines;
  value_array m_constants;
};

}  // namespace cwt::details
