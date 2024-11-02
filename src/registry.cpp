
#include "registry.hpp"

namespace cuke
{

internal::registry& registry()
{
  static internal::registry instance;
  return instance;
}

void sort_steps_by_type()
{
  registry().sort_steps([](const internal::step& lhs, const internal::step& rhs)
                        { return lhs.step_type() < rhs.step_type(); });
}
}  // namespace cuke
