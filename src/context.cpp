#include "context.hpp"

namespace cuke::internal
{

scenario_context& get_context()
{
  static scenario_context sc;
  return sc;
}
}  // namespace cuke::internal
