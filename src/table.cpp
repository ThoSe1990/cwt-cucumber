#include "table.hpp"

namespace cuke
{

table::table(value_array data, std::size_t col_count)
    : m_data(std::move(data)), m_col_count(col_count)
{
  if ((m_data.size() % m_col_count) != 0)
  {
    throw std::invalid_argument(
        std::format("cuke::table: Inconsisten table size with cells count: {}, "
                    "and columns count: {}",
                    m_data.size(), m_col_count));
  }
}

std::size_t table::row_count() const noexcept
{
  return m_data.size() / m_col_count;
}
std::size_t table::col_count() const noexcept { return m_col_count; }
std::size_t table::cells_count() const noexcept { return m_data.size(); }

table::row table::operator[](std::size_t idx) const
{
  if (idx < row_count())
  {
    return row(m_data.begin() + idx * m_col_count, m_col_count);
  }
  else
  {
    throw std::invalid_argument(
        std::format("table::operator[]: Can not access row {}, row count = {}",
                    idx, row_count()));
  }
}

table::row::row(const value_array& data, std::size_t row_idx,
                std::size_t col_count)
    : m_current(data.begin() + row_idx),
      // m_begin(data.begin()),
      // m_end(data.end()),
      m_col_count(col_count)
{
}
table::row::row(value_array::const_iterator current, std::size_t col_count)
    : m_current(current), m_col_count(col_count)
{
}
const cuke::value& table::row::operator[](std::size_t idx) const
{
  if (idx < m_col_count)
  {
    return *(m_current + idx);
  }
  else
  {
    throw std::invalid_argument(
        std::format("cuke::table::row: Can not access value at column {}, "
                    "column size is: {}",
                    idx, m_col_count));
  }
}

}  // namespace cuke
