#pragma once

#include <string>
#include <vector>
class box
{
 public:
  box() = default;
  box(unsigned int width, unsigned int height, unsigned int depth)
      : m_width(width), m_height(height), m_depth(depth)
  {
  }

  [[nodiscard]] unsigned int volume() const noexcept
  {
    return m_width * m_height * m_depth;
  }
  void open() { m_is_open = true; }
  void close() { m_is_open = false; }
  [[nodiscard]] bool is_open() const noexcept { return m_is_open; }
  void add_item(const std::string& item) { m_items.push_back(item); }
  [[nodiscard]] std::size_t items_count() const noexcept
  {
    return m_items.size();
  }

 private:
  unsigned int m_width{0};
  unsigned int m_height{0};
  unsigned int m_depth{0};
  bool m_is_open{false};
  std::vector<std::string> m_items;
};