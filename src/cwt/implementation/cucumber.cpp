#include "../cucumber.hpp"




namespace cuke::details
{
  std::vector<std::pair<const char*, cuke_step_t>>& steps()
  {
    static std::vector<std::pair<const char*, cuke_step_t>> s;
    return s;
  }

  std::vector<hook>& hooks()
  {
    static std::vector<hook> h;
    return h;
  }
} // namespace cuke::details
