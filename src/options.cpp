#include <span>
#include <algorithm>
#include <filesystem>
#include <optional>

#include "options.hpp"
#include "catalog.hpp"
#include "log.hpp"
#include "util.hpp"

namespace
{
bool is_catalog_or_report(cuke::program_args::key_t key)
{
  switch (key)
  {
    case cuke::program_args::key_t::report_json:
    case cuke::program_args::key_t::steps_catalog_json:
    case cuke::program_args::key_t::steps_catalog_readable:
      return true;
    default:
      return false;
  }
}

std::string get_optional_file_path(std::span<const char*>::iterator it,
                                   std::span<const char*>::iterator end)
{
  if (it == end)
  {
    return "";
  }

  namespace fs = std::filesystem;
  fs::path path = *it;
  if (fs::is_directory(path))
  {
    return "";
  }
  if (path.string().find(".feature") != std::string::npos)
  {
    return "";
  }
  return path.string();
}

std::string get_option_value(cuke::program_args::key_t key,
                             std::span<const char*>::iterator it,
                             std::span<const char*>::iterator end)
{
  if (is_catalog_or_report(key))
  {
    return get_optional_file_path(it, end);
  }
  if (key == cuke::program_args::key_t::tags)
  {
    return it != end ? std::string(*it) : "";
  }
  return "";
}

}  // namespace

namespace cuke
{

namespace internal
{

runtime_options& get_runtime_options()
{
  static runtime_options opt;
  return opt;
}

}  // namespace internal

void skip_scenario() { internal::get_runtime_options().skip_scenario(true); }
void ignore_scenario()
{
  internal::get_runtime_options().ignore_scenario(true);
}
void fail_scenario(const std::string_view msg /* = "" */)
{
  internal::get_runtime_options().fail_scenario(true, msg);
}
void fail_step(const std::string_view msg /* = "" */)
{
  internal::get_runtime_options().fail_step(true, msg);
}
void program_args::initialize(int argc, const char* argv[])
{
  clear();
  std::span<const char*> args = std::span<const char*>(argv, argc);
  if (args.size() == 1)
  {
    m_flags[program_args::key_t::help] = true;
    return;
  }

  for (auto it = args.begin() + 1; it != args.end(); ++it)
  {
    if (!keys.contains(*it))
    {
      process_path(*it);
      continue;
    }

    auto& opt = keys.at(*it);

    switch (opt.type)
    {
      case program_args::type_t::option:
      {
        auto next = it + 1;
        auto value = get_option_value(opt.key, next, args.end());
        if (!value.empty()) ++it;
        m_options[opt.key] = {true, value};
      }
      break;
      case program_args::type_t::file_to_exclude:
      {
        ++it;
        if (it == args.end())
        {
          log::error("Expect file after '--exclude-file' option",
                     log::new_line);
          return;
        }
        m_excluded_files.push_back(*it);
      }
      break;
      case program_args::type_t::flag:
      {
        m_flags[opt.key] = true;
      }
      break;
    }
  }

  if (is_set(program_args::key_t::quiet))
  {
    cuke::log::set_level(cuke::log::level::quiet);
  }
  if (is_set(program_args::key_t::verbose))
  {
    cuke::log::set_level(cuke::log::level::verbose);
  }
  remove_excluded_files();
}

void program_args::clear()
{
  m_options.clear();
  m_flags.clear();
  m_files.clear();
  m_excluded_files.clear();
}

const std::unordered_map<std::string_view, program_args::info>
    program_args::keys = []
{
  std::unordered_map<std::string_view, program_args::info> m;

  for (auto const& d : program_args::defs)
  {
    if (!d.short_key.empty()) m[d.short_key] = {d.key, d.type, d.description};
    if (!d.long_key.empty()) m[d.long_key] = {d.key, d.type, d.description};
  }
  return m;
}();

void program_args::find_feature_in_dir(const std::filesystem::path& dir)
{
  for (const auto& entry : std::filesystem::directory_iterator(dir))
  {
    if (std::filesystem::is_directory(entry))
    {
      find_feature_in_dir(entry.path());
    }
    else if (entry.path().extension() == ".feature")
    {
      m_files.push_back(feature_file{entry.path().string(), {}});
    }
  }
}

void program_args::process_path(std::string_view sv)
{
  namespace fs = std::filesystem;
  auto [file_path, lines] = internal::filepath_and_lines(sv);
  fs::path path = file_path;
  if (fs::exists(path))
  {
    if (fs::is_directory(path))
    {
      find_feature_in_dir(path);
    }
    else if (path.extension() == ".feature")
    {
      m_files.push_back(feature_file{file_path, lines});
    }
  }
}

void program_args::remove_excluded_files()
{
  m_files.erase(
      std::remove_if(m_files.begin(), m_files.end(),
                     [this](const feature_file& file)
                     {
                       return std::any_of(
                           m_excluded_files.begin(), m_excluded_files.end(),
                           [&file](const std::string& to_exclude)
                           { return file.path.ends_with(to_exclude); });
                       ;
                     }),
      m_files.end());
}

bool program_args::is_set(program_args::key_t key) const
{
  if (m_flags.contains(key))
  {
    return m_flags.at(key);
  }
  if (m_options.contains(key))
  {
    return m_options.at(key).first;
  }
  return false;
}
const std::vector<feature_file>& program_args::get_feature_files() const
{
  return m_files;
}
const std::vector<std::string>& program_args::get_excluded_files() const
{
  return m_excluded_files;
}
const std::string& program_args::get_value(program_args::key_t key) const
{
  if (m_options.contains(key) && m_options.at(key).first)
  {
    return m_options.at(key).second;
  }
  // TODO: to_string
  throw std::runtime_error(
      std::format("Unknown program argument type {}", int(key)));
}

program_args& get_program_args(std::optional<int> argc /*= std::nullopt*/,
                               const char* argv[] /*= nullptr*/)
{
  static program_args instance;
  if (argc && argv)
  {
    instance.initialize(argc.value(), argv);
  }
  return instance;
}

}  // namespace cuke
