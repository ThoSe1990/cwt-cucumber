#pragma once

#include <optional>
#include <unordered_map>

#include "value.hpp"

namespace cuke
{

/**
 * @class table
 * @brief The table which holds the data for tables / datatables
 *
 * @details CWT Cucumber creates a table and stores it. In a step you can create
 * / access a table if available with CUKE_TABLE()
 *
 */
class table
{
 public:
  using pair = std::unordered_map<std::string, cuke::value>;

  table() = default;
  table(const table&) = default;
  table(table&&) = default;
  table& operator=(const table&) = default;
  table& operator=(table&&) = default;
  table(value_array&& data);
  // TODO data should be rvalue ref? value_array&& data
  table(value_array data, std::size_t col_count);

  /**
   * @class row
   * @brief A row object to represent a row in a table
   *
   * @details This holds all values of the dedicated row. Access to the values
   * (cuke::value) with the operator[] by index. If it is created as hash row /
   * pair an access by string literals is possible too
   */
  class row
  {
   public:
    row(const value_array& data, std::size_t col_count);
    row(value_array::const_iterator current, std::size_t col_count);
    row(value_array::const_iterator current, std::size_t col_count,
        std::optional<value_array::const_iterator> header);

    /**
     * @brief Access to a cell
     * @param idx Columns index to access
     * @return Returns a const reference to the cuke::value in this cell
     */
    [[nodiscard]] const cuke::value& operator[](std::size_t idx) const;
    /**
     * @brief Access to a cell, when the header is available with the according
     * keys
     * @param key Columns key to access
     * @return Returns a const reference to the cuke::value in this cell
     */
    [[nodiscard]] const cuke::value& operator[](std::string_view key) const;
    /**
     * @brief Total count of columns in this table / row
    */
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


  /**
   * @brief Appends a row to the datatable
   *
   * Appending a row only works if the given row is the same
   * length as the rows in the existing table. 
   * 
   * @param data Data to be appended
   * @return Returns true if data is appended succesfully
   */
  [[nodiscard]] bool append_row(value_array&& data);

  /**
   * @brief Returns the number of rows
   */
  [[nodiscard]] std::size_t row_count() const noexcept;
  /**
   * @brief Returns the number of cols
   */
  [[nodiscard]] std::size_t col_count() const noexcept;
  /**
   * @brief Returns the number of cells
   */
  [[nodiscard]] std::size_t cells_count() const noexcept;

  /**
   * @brief Access a row by index.
   * @param idx Rows index to access
   * @return Returns a row object.
   */
  [[nodiscard]] row operator[](std::size_t idx) const;

  /**
   * @brief Returns a wrapper to the row iterator for raw access
   *
   * @details Use raw() for range based loops, e.g.: for (const auto& row :
   * t.raw()) Now you can iteratoe over each row. Access the elements by their
   * index. Note: Each element is a cuke::value this means you have to cast it,
   * like row[0].as<int>(), row[0].as<std::string>(), ...
   */
  [[nodiscard]] raw_access raw() const;

  /**
   * @brief Returns a wrapper to the row iterator for hashes
   *
   * @details Use hashes() for range based loops, e.g.: for (const auto& row :
   * t.hashes()) Now you can iteratoe over each row. Access the elements by
   * their identifier. Note: Each element is a cuke::value this means you have
   * to cast it, like row["CELL A"].as<int>(), row["CELL B"].as<std::string>(),
   * ...
   */
  [[nodiscard]] hash_access hashes() const;
  /**
   * @brief Returns a wrapper to the row iterator for hashes
   *
   * @details This works only for a total column count of 2. If the column count
   * is not equal to 2 this function throws a std::runtime_error.
   * The first column is then the key, the second is the value.
   * cuke::table::pair is an alias for std::unordered_map<std::string,
   * cuke::value>
   */
  [[nodiscard]] cuke::table::pair rows_hash() const;

 private:
  value_array m_data;
  std::size_t m_col_count{0};
};

}  // namespace cuke
