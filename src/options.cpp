#include <filesystem>
#include <optional>

#include "options.hpp"
#include "catalog.hpp"
#include "util.hpp"

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

void cuke_args::initialize(int argc, const char* argv[])
{
  clear();
  m_args = std::span<const char*>(argv, argc);
  if (m_args.size() == 1)
  {
    m_options.print_help = true;
    return;
  }

  for (auto it = m_args.begin() + 1; it != m_args.end(); ++it)
  {
    std::string_view sv{*it};
    if (sv.starts_with('-'))
    {
      process_option(it, m_args.end());
    }
    else
    {
      process_path(sv);
    }
  }
}

void cuke_args::clear()
{
  m_args = std::span<const char*>();
  m_options = options{};
}
const options& cuke_args::get_options() const noexcept { return m_options; }

void cuke_args::process_option(std::span<const char*>::iterator it,
                               std::span<const char*>::iterator end)
{
  std::string_view option(*it);
  if (option.starts_with("-t") || option.starts_with("--tags"))
  {
    auto next_it = std::next(it);
    if (next_it == end)
    {
      println(internal::color::red,
              "Expect tag expression after '--tags/-t' option");
      return;
    }
    std::string_view arg(*next_it);
    m_options.tag_expression = arg;
  }
  else if (option.starts_with("-h") || option.starts_with("--help"))
  {
    m_options.print_help = true;
  }
  else if (option.starts_with("-q") || option.starts_with("--quiet"))
  {
    m_options.quiet = true;
  }
  else if (option.starts_with("-v") || option.starts_with("--verbose"))
  {
    m_options.verbose = true;
  }
  else if (option == "--steps-catalog")
  {
    m_options.catalog.out.try_to_set_file_sink(std::next(it), end);
    m_options.catalog.type = catalog_type::readable_text;
  }
  else if (option == "--steps-catalog-json")
  {
    m_options.catalog.out.try_to_set_file_sink(std::next(it), end);
    m_options.catalog.type = catalog_type::json;
  }
  else if (option == "--report-json")
  {
    m_options.report.out.try_to_set_file_sink(std::next(it), end);
    m_options.report.type = report_type::json;
  }
}

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

[[nodiscard]] cuke_args& program_arguments(
    std::optional<int> argc /*= std::nullopt*/,
    const char* argv[] /*= nullptr*/)
{
  static cuke_args instance;
  if (argc && argv)
  {
    instance.initialize(argc.value(), argv);
  }
  return instance;
}

}  // namespace cuke
