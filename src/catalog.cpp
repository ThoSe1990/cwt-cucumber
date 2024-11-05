#include "catalog.hpp"

#include "options.hpp"
#include "registry.hpp"
#include "util.hpp"

#ifdef WITH_JSON
#include <nlohmann/json.hpp>
#endif  // WITH_JSON

namespace cuke::catalog
{
std::string as_readable_text()
{
  cuke::registry().sort_steps_by_type();

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
#ifdef WITH_JSON
  using json = nlohmann::json;

  cuke::registry().sort_steps_by_type();

  json steps_catalog = {{"steps_catalog", json::array()}};

  for (const auto& step : cuke::registry().steps())
  {
    json field_step = {{"definition", step.definition()}};
    json field_type = {{"type", to_string(step.step_type())}};

    json field_var_types = json::array();
    for (const auto& var_type : step.type_info())
    {
      field_var_types.push_back(var_type);
    }

    json step_entry = {{"type", field_type["type"]},
                       {"definition", field_step["definition"]},
                       {"var_types", field_var_types}};

    steps_catalog["steps_catalog"].push_back(step_entry);
  }

  return steps_catalog.dump(indents);
#endif  // WITH_JSON

  println(internal::color::red,
          "nlohmann-json is not added as dependency in this build. Can not "
          "export json format.");
  return "";
}

void print_readable_text_to_sink()
{
  program_arguments().get_options().catalog.out.write(as_readable_text());
}
void print_json_to_sink(std::size_t indents /* = 2 */)
{
  program_arguments().get_options().catalog.out.write(as_json());
}

}  // namespace cuke::catalog
