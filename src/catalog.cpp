
#include "catalog.hpp"

#include "registry.hpp"

namespace cuke::catalog
{
std::string as_readable_text()
{
  std::string result{"Step Definitions (catalog):\n"};
  result.append("---------------------------\n");
  for (const auto step : cuke::registry().steps())
  {
    result.append(to_string(step.step_type()));
    result += ' ';
    result.append(step.definition());
    result += '\n';
  }
  return result;
}

}  // namespace cuke::catalog
