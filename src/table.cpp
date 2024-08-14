#include <algorithm>
#include <stdexcept>
#include <utility>

#include "table.hpp"

namespace cuke
{
table::table(value_array&& row)
    : m_data(std::move(row)), m_col_count(m_data.size())
{
  row.clear();
}
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

bool table::append_row(value_array&& row)
{
  if (m_col_count != row.size())
  {
    return false;
  }
  else
  {
    m_data.reserve(m_data.size() + m_col_count);
    m_data.insert(m_data.end(), std::make_move_iterator(row.begin()),
                  std::make_move_iterator(row.end()));
    row.clear();
    return true;
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
table::row table::hash_row(std::size_t row_index) const
{
  if (row_index < row_count()) [[likely]]
  {
    return row(m_data.begin() + row_index * m_col_count, m_col_count,
               m_data.begin());
  }
  throw std::runtime_error(
      std::format("table::hash: Row index '{}' does not exist", row_index));
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

std::vector<std::size_t> table::col_sizes() const noexcept
{
  std::vector<std::size_t> col_size(col_count(), 0);

  for (const auto& row : raw())
  {
    for (std::size_t i = 0; i < col_count(); ++i)
    {
      col_size[i] = std::max(col_size[i], row[i].to_string().length());
    }
  }

  return col_size;
}

std::vector<std::string> table::to_string_array() const noexcept
{
  std::vector<std::size_t> col_size = col_sizes();
  std::vector<std::string> result;
  result.reserve(row_count());
  for (const auto& row : raw())
  {
    std::string line = "|";
    for (std::size_t i = 0; i < col_count(); ++i)
    {
      line += ' ';
      line.append(row[i].to_string());
      int padding = col_size[i] - row[i].to_string().length();
      if (padding > 0)
      {
        line.append(padding, ' ');
      }
      line += ' ';
      line += '|';
    }
    result.push_back(line);
  }
  return result;
}

}  // namespace cuke
