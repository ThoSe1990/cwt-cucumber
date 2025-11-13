#pragma once

#include <string>
#include <vector>

class box
{
 public:
  box() = default;
  box(const std::string& label) : m_label(label) {}

  void add_item(const std::string& item) { m_items.push_back(item); }
  void add_items(const std::string& item, std::size_t count)
  {
    for ([[maybe_unused]] int i = 0; i < count; i++)
    {
      add_item(item);
    }
  }
  [[nodiscard]] std::size_t items_count() const noexcept
  {
    return m_items.size();
  }
  void add_weight(const std::size_t weight) { m_weight += weight; }
  const std::string& label() const noexcept { return m_label; }
  [[nodiscard]] std::size_t weight() const noexcept { return m_weight; }
  [[nodiscard]] const std::string& at(const std::size_t idx) const
  {
    return m_items.at(idx);
  }
  void close() noexcept { m_is_open = false; }

  std::size_t count(std::string_view item)
  {
    return std::count(m_items.begin(), m_items.end(), item);
  }
  bool contains(const std::string& item)
  {
    return std::find(m_items.begin(), m_items.end(), item) != m_items.end();
  }

 private:
  bool m_is_open{true};
  std::size_t m_weight{0};
  std::string m_label{""};
  std::vector<std::string> m_items;
};
