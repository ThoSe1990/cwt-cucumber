#pragma once

#include "../value.hpp"

namespace cuke
{

class registry
{
 public:
  static void push_step(const internal::step& s) noexcept
  {
    registry::steps().push_back(s);
  }
  static void clear() noexcept
  {
    registry::steps().clear();
  }
  [[nodiscard]] static std::size_t steps_count() noexcept
  {
    return registry::steps().size();
  }

 private:
  [[nodiscard]] static std::vector<internal::step>& steps() noexcept
  {
    static std::vector<internal::step> instance; 
    return instance; 
  }
};

}  // namespace cuke
