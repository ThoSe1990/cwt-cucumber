#pragma once

#include <optional>
#include <unordered_map>

#include "value.hpp"

namespace cuke
{

class table
{
 public:
  table() = default;
  table(const table&) = default;
  table(table&&) = default;
  table& operator=(const table&) = default;
  table& operator=(table&&) = default;
  table(value_array data, std::size_t col_count);

  class row
  {
   public:
    row(const value_array& data, std::size_t col_count);
    row(value_array::const_iterator current, std::size_t col_count);
    row(value_array::const_iterator current, std::size_t col_count,
        std::optional<value_array::const_iterator> header);

    [[nodiscard]] const cuke::value& operator[](std::size_t idx) const;
    [[nodiscard]] const cuke::value& operator[](std::string_view key) const;
    [[nodiscard]] std::size_t col_count() const noexcept { return m_col_count; }

   private:
    value_array::const_iterator m_current;
    std::size_t m_col_count;
    std::optional<value_array::const_iterator> m_header{std::nullopt};
  };

  class row_iterator
  {
   public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = const row;
    using difference_type = std::ptrdiff_t;
    using pointer = const row*;
    using reference = const row&;

    explicit row_iterator(value_array::const_iterator it, std::size_t col_count)
        : m_current(it), m_col_count(col_count)
    {
    }
    explicit row_iterator(value_array::const_iterator it,
                          value_array::const_iterator begin,
                          std::size_t col_count)
        : m_current(it), m_col_count(col_count), m_header(begin)
    {
    }
    row operator*() const { return row(m_current, m_col_count, m_header); }
    row_iterator& operator++()
    {
      m_current += m_col_count;
      return *this;
    }
    bool operator!=(const row_iterator& rhs) const
    {
      return rhs.m_current != m_current;
    }

   private:
    value_array::const_iterator m_current;
    std::size_t m_col_count;
    std::optional<value_array::const_iterator> m_header{std::nullopt};
  };

  class raw_access
  {
   public:
    raw_access(cuke::value_array::const_iterator begin,
               cuke::value_array::const_iterator end, std::size_t col_count)
        : m_begin(begin), m_end(end), m_col_count(col_count)
    {
    }
    row_iterator begin() const { return row_iterator(m_begin, m_col_count); }
    row_iterator end() const { return row_iterator(m_end, m_col_count); }

   private:
    value_array::const_iterator m_begin;
    value_array::const_iterator m_end;
    std::size_t m_col_count;
  };

  class hash_access
  {
   public:
    hash_access(cuke::value_array::const_iterator begin,
                cuke::value_array::const_iterator end, std::size_t col_count)
        : m_begin(begin), m_end(end), m_col_count(col_count)
    {
    }
    row_iterator begin() const
    {
      return row_iterator(m_begin + m_col_count, m_begin, m_col_count);
    }
    row_iterator end() const { return row_iterator(m_end, m_col_count); }

   private:
    value_array::const_iterator m_begin;
    value_array::const_iterator m_end;
    std::size_t m_col_count;
  };

  [[nodiscard]] std::size_t row_count() const noexcept;
  [[nodiscard]] std::size_t col_count() const noexcept;
  [[nodiscard]] std::size_t cells_count() const noexcept;

  [[nodiscard]] row operator[](std::size_t idx) const;

  [[nodiscard]] raw_access raw() const;
  [[nodiscard]] hash_access hashes() const;
  [[nodiscard]] std::unordered_map<std::string, cuke::value> rows_hash() const;

 private:
  value_array m_data;
  std::size_t m_col_count{0};
};

}  // namespace cuke
