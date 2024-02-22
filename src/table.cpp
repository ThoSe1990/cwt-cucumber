#include "table.hpp"

namespace cuke
{

table::table(value_array data, std::size_t col_count)
    : m_data(std::move(data)), m_col_count(col_count)
{
  if ((m_data.size() % m_col_count) != 0)
  {
    throw std::runtime_error(
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
    throw std::runtime_error(
        std::format("table::operator[]: Can not access row {}, row count = {}",
                    idx, row_count()));
  }
}

table::row::row(const value_array& data, std::size_t col_count)
    : m_current(data.begin()), m_col_count(col_count)
{
}
table::row::row(value_array::const_iterator current, std::size_t col_count)
    : m_current(current), m_col_count(col_count)
{
}
table::row::row(value_array::const_iterator current, std::size_t col_count,
                std::optional<value_array::const_iterator> header)
    : m_current(current), m_col_count(col_count), m_header(header)
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
    throw std::runtime_error(
        std::format("cuke::table::row: Can not access value at column {}, "
                    "column size is: {}",
                    idx, m_col_count));
  }
}
const cuke::value& table::row::operator[](std::string_view key) const
{
  if (!m_header.has_value()) [[unlikely]]
  {
    throw std::runtime_error("table::row::operator[]: No header given.");
  }

  auto value = m_current;
  for (auto it = m_header.value(); it != (m_current + m_col_count);
       ++it, ++value)
  {
    if (it->to_string() == key)
    {
      return *value;
    }
  }
  throw std::runtime_error(std::format(
      "table::row::operator[]: Given key '{}' not found in table", key));
}

table::raw_access table::raw() const
{
  return raw_access(m_data.begin(), m_data.end(), m_col_count);
}
table::hash_access table::hashes() const
{
  return hash_access(m_data.begin(), m_data.end(), m_col_count);
}

cuke::table::pair table::rows_hash() const
{
  if (m_col_count == 2)
  {
    std::unordered_map<std::string, cuke::value> result;
    std::transform(m_data.begin(), m_data.end() - 1, m_data.begin() + 1,
                   std::inserter(result, result.begin()),
                   [](const cuke::value& k, const cuke::value& v)
                   { return std::make_pair(k.to_string(), v); });
    return result;
  }
  else
  {
    throw std::runtime_error(std::format(
        "table::rows_hash: Can only create hash rows with two columns, "
        "this table has {} columns",
        m_col_count));
  }
}

}  // namespace cuke
