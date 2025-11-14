#include "report.hpp"

#include "util.hpp"
#include "options.hpp"
#include "test_results.hpp"

#ifdef WITH_JSON
#include <nlohmann/json.hpp>
#endif  // WITH_JSON

namespace
{
// FIXME: duplicated in catalog!
void write_data(std::string_view data, const std::string& m_filepath)
{
  if (m_filepath.empty())
  {
    cuke::log::info(data, cuke::log::new_line);
  }
  else
  {
    std::ofstream file(m_filepath);
    if (file.is_open())
    {
      file << data;
      file.close();
    }
    else
    {
      cuke::log::error(std::format("Can not open file '{}'", m_filepath));
    }
  }
}
}  // namespace
namespace cuke::report
{

namespace internal
{

#ifdef WITH_JSON
void push_tags(nlohmann::json& field, const std::vector<std::string>& tags)
{
  for (const std::string& tag : tags)
  {
    field["tags"].push_back(tag);
  }
}

nlohmann::json to_json(const cuke::table& t)
{
  using json = nlohmann::json;

  json field_table{{"rows", json::array()}};

  for (std::size_t row = 0; row < t.row_count(); ++row)
  {
    json field_cells_in_row{{"cells", json::array()}};
    for (std::size_t col = 0; col < t.col_count(); ++col)
    {
      field_cells_in_row["cells"].push_back(t[row][col].to_string());
    }
    field_table["rows"].push_back(field_cells_in_row);
  }
  return field_table;
}
#endif  // WITH_JSON

}  // namespace internal

std::string as_json(std::size_t indents /* = 2 */)
{
#ifdef WITH_JSON
  using json = nlohmann::json;

  std::vector<results::feature>& features = results::test_results().data();

  json json_features = json::array();

  for (const results::feature& feature : features)
  {
    json field_feature = {{"description", feature.description},
                          {"elements", json::array()},
                          {"id", feature.id},
                          {"line", feature.line},
                          {"keyword", feature.keyword},
                          {"name", feature.name},
                          {"tags", json::array()},
                          {"uri", feature.file}};
    internal::push_tags(field_feature, feature.tags);

    for (const results::scenario& scenario : feature.scenarios)
    {
      json field_scenario = {
          {"description", scenario.description},
          {"id", scenario.id},
          {"keyword", scenario.keyword},
          {"line", scenario.line},
          {"name", scenario.name},
          {"steps", json::array()},
          {"tags", json::array()},
          {"type", scenario.name},
      };
      internal::push_tags(field_scenario, scenario.tags);

      for (const results::step& step : scenario.steps)
      {
        json field_step = {
            {"arguments", json::array()},
            {"keyword", step.keyword},
            {"line", step.line},
            {"name", step.name},
            {
                "match",
                {{"location", step.source_location}},
            },
            {
                "result",
                {{"status", results::to_string(step.status)}},
            },
        };

        if (step.status == results::test_status::failed ||
            step.status == results::test_status::undefined)
        {
          field_step["result"]["error_message"] = step.error_msg;
        }
        if (!step.table.empty())
        {
          field_step["arguments"].push_back(internal::to_json(step.table));
        }
        if (!step.doc_string.empty())
        {
          json field_doc_string = {{"content", step.doc_string}};
          field_step["arguments"].push_back(field_doc_string);
        }

        field_scenario["steps"].push_back(field_step);
      }
      field_feature["elements"].push_back(field_scenario);
    }

    json current_feature = {
        {"description", field_feature["description"]},
        {"elements", field_feature["elements"]},
        {"id", field_feature["id"]},
        {"line", field_feature["line"]},
        {"keyword", field_feature["keyword"]},
        {"name", field_feature["name"]},
        {"uri", field_feature["uri"]},
        {"tags", field_feature["tags"]},
    };

    json_features.push_back(current_feature);
  }

  return json_features.dump(indents);
#else
  log::error(
      "nlohmann-json is not added as dependency in this build. Can not "
      "export json format.",
      log::new_line);
  return "";
#endif  // WITH_JSON
}
void print_json_to_sink(std::size_t indents /* = 2 */)
{
  write_data(as_json(), get_program_option_value(options::key::report_json));
}

}  // namespace cuke::report
