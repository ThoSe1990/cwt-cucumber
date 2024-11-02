#include "catalog.hpp"

#include "registry.hpp"

#include <nlohmann/json.hpp>

namespace cuke::catalog
{
std::string as_readable_text()
{
  std::string result{"Step Definitions (catalog):\n"};
  result.append("---------------------------\n");
  for (const auto& step : cuke::registry().steps())
  {
    result.append(to_string(step.step_type()));
    result += ' ';
    result.append(step.definition());
    result += '\n';
  }
  return result;
}

std::string as_json(std::size_t indents /* = 2 */)
{
  using json = nlohmann::json;

  json steps_catalog = {{"steps_catalog", json::array()}};

  for (const auto& step : cuke::registry().steps())
  {
    json field_step = {{"definition", step.definition()}};
    json field_type = {{"type", to_string(step.step_type())}};

    json field_var_types = json::array();
    for (const auto& var_type : step.value_types())
    {
      field_var_types.push_back(to_string(var_type));
    }

    json step_entry = {{"type", field_type["type"]},
                       {"definition", field_step["definition"]},
                       {"var_types", field_var_types}};

    steps_catalog["steps_catalog"].push_back(step_entry);
  }

  return steps_catalog.dump(indents);
}

}  // namespace cuke::catalog
