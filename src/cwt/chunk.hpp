#pragma once

#include <vector>

#include "value.hpp"
namespace cwt::details
{

enum class op_code : uint32_t
{
  constant,
  tag,
  nil,
  pop,
  get_var,
  set_var,
  define_var,
  print,
  println,
  step_result,
  init_scenario,
  scenario_result,
  jump_if_failed,
  call,
  call_step,
  call_step_with_doc_string,
  hook,
  func_return
};

inline constexpr uint32_t to_uint(op_code code) { return static_cast<uint32_t>(code); }
inline constexpr op_code to_code(uint32_t val)
{
  if (val >= to_uint(op_code::constant) && val <= to_uint(op_code::func_return))
      [[likely]]
  {
    return static_cast<op_code>(val);
  }
  else [[unlikely]]
  {
    throw std::out_of_range(
        "inline op_code to_code(uint32_t val): value out of range");
  }
}

// TODO reserve chunks vectors by default ?
class chunk
{
 public:
  [[nodiscard]] std::size_t size() const noexcept;
  [[nodiscard]] const uint32_t& back() const noexcept;

  [[nodiscard]] std::size_t constants_count() const noexcept;
  [[nodiscard]] std::size_t last_constant() const noexcept;
  [[nodiscard]] const value& constant(std::size_t index) const;
  [[nodiscard]] const value& constants_back() const noexcept;

  [[nodiscard]] uint32_t lines(const std::size_t index) const;

  void push_byte(op_code byte, const std::size_t line);
  void push_byte(uint32_t byte, const std::size_t line);

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

  std::size_t get_index(chunk::const_iterator iter) const;
  const_iterator cbegin() const;
  const_iterator cend() const;

 private:
  std::vector<uint32_t> m_code;
  std::vector<std::size_t> m_lines;
  value_array m_constants;
};

}  // namespace cwt::details
