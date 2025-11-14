#include <algorithm>
#include <filesystem>
#include <optional>

#include "options.hpp"
#include "catalog.hpp"
#include "log.hpp"
#include "util.hpp"

namespace
{
bool is_catalog_or_report(cuke::options::key key)
{
  switch (key)
  {
    case cuke::options::key::report_json:
    case cuke::options::key::steps_catalog_json:
    case cuke::options::key::steps_catalog_readable:
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

std::string get_option_value(cuke::options::key key,
                             std::span<const char*>::iterator it,
                             std::span<const char*>::iterator end)
{
  if (is_catalog_or_report(key))
  {
    return get_optional_file_path(it, end);
  }
  if (key == cuke::options::key::tags)
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
void cuke_args::initialize(int argc, const char* argv[])
{
  clear();
  m_args = std::span<const char*>(argv, argc);
  if (m_args.size() == 1)
  {
    m_options.flags[options::key::help] = true;
    return;
  }

  for (auto it = m_args.begin() + 1; it != m_args.end(); ++it)
  {
    if (!m_options.keys.contains(*it))
    {
      process_path(*it);
      continue;
    }

    auto& opt = m_options.keys.at(*it);

    switch (opt.type)
    {
      case options::type::option:
      {
        auto next = it + 1;
        auto value = get_option_value(opt.key, next, m_args.end());
        if (!value.empty()) ++it;
        m_options.options[opt.key] = {true, value};
      }
      break;
      case options::type::file_to_exclude:
      {
        ++it;
        if (it == m_args.end())
        {
          log::error("Expect file after '--exclude-file' option",
                     log::new_line);
          return;
        }
        m_options.excluded_files.push_back(*it);
      }
      break;
      case options::type::flag:
      {
        m_options.flags[opt.key] = true;
      }
      break;
    }
  }

  if (program_arg_is_set(options::key::quiet))
  {
    cuke::log::set_level(cuke::log::level::quiet);
  }
  if (program_arg_is_set(options::key::verbose))
  {
    cuke::log::set_level(cuke::log::level::verbose);
  }
  remove_excluded_files();
}

void cuke_args::clear()
{
  m_args = std::span<const char*>();
  m_options = options{};
}
const options& cuke_args::get_options() const noexcept { return m_options; }

const std::unordered_map<std::string_view, options::info> options::keys = []
{
  std::unordered_map<std::string_view, options::info> m;

  for (auto const& d : options::defs)
  {
    if (!d.short_key.empty()) m[d.short_key] = {d.key, d.type, d.description};
    if (!d.long_key.empty()) m[d.long_key] = {d.key, d.type, d.description};
  }
  return m;
}();

void cuke_args::find_feature_in_dir(const std::filesystem::path& dir)
{
  for (const auto& entry : std::filesystem::directory_iterator(dir))
  {
    if (std::filesystem::is_directory(entry))
    {
      find_feature_in_dir(entry.path());
    }
    else if (entry.path().extension() == ".feature")
    {
      m_options.files.push_back(feature_file{entry.path().string(), {}});
    }
  }
}

void cuke_args::process_path(std::string_view sv)
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
      m_options.files.push_back(feature_file{file_path, lines});
    }
  }
}

void cuke_args::remove_excluded_files()
{
  m_options.files.erase(
      std::remove_if(m_options.files.begin(), m_options.files.end(),
                     [this](const feature_file& file)
                     {
                       return std::any_of(
                           m_options.excluded_files.begin(),
                           m_options.excluded_files.end(),
                           [&file](const std::string& to_exclude)
                           { return file.path.ends_with(to_exclude); });
                       ;
                     }),
      m_options.files.end());
}

cuke_args& program_arguments(std::optional<int> argc /*= std::nullopt*/,
                             const char* argv[] /*= nullptr*/)
{
  static cuke_args instance;
  if (argc && argv)
  {
    instance.initialize(argc.value(), argv);
  }
  return instance;
}
const bool program_arg_is_set(options::key key)
{
  const auto& opt = program_arguments().get_options();
  if (opt.flags.contains(key))
  {
    return opt.flags.at(key);
  }
  if (opt.options.contains(key))
  {
    return opt.options.at(key).first;
  }
  return false;
}
const std::string& get_program_option_value(options::key key)
{
  const auto& opt = program_arguments().get_options();
  if (opt.options.contains(key) && opt.options.at(key).first)
  {
    return opt.options.at(key).second;
  }
  throw std::runtime_error(std::format("Unknown program options {}", int(key)));
}

}  // namespace cuke
