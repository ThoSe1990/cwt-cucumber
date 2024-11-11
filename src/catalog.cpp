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

  if (cuke::registry().custom_expressions().size() > 0)
  {
    result += '\n';
    result.append(std::string("Custom Parameter Types:\n"));
    result.append(std::string("-----------------------\n"));
    for (auto& [key, value] : cuke::registry().custom_expressions())
    {
      result.append(key);
      result.append("\n  ");
      result.append(std::string("Comment: "));
      result.append(value.type_info);
      result.append("\n  ");
      result.append(std::string("Pattern: "));
      result.append(value.pattern);
      result += '\n';
    }
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
    json field_func_name = {{"function", step.function_name()}};
    json field_step = {{"definition", step.definition()}};
    json field_type = {{"type", to_string(step.step_type())}};

    json field_var_types = json::array();
    for (const auto& info : step.type_info())
    {
      field_var_types.push_back(info.description);
    }

    json step_entry = {{"type", field_type["type"]},
                       {"function", field_func_name["function"]},
                       {"definition", field_step["definition"]},
                       {"var_types", field_var_types}};

    steps_catalog["steps_catalog"].push_back(step_entry);
  }
  for (auto& [key, value] : cuke::registry().custom_expressions())
  {
    json field_type = {{"type", key}};
    json field_pattern = {{"pattern", value.pattern}};
    json field_comment = {{"comment", value.type_info}};

    json type_entry = {
        {"type", field_type["type"]},
        {"pattern", field_pattern["pattern"]},
        {"comment", field_comment["comment"]},
    };
    steps_catalog["types"].push_back(type_entry);
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
