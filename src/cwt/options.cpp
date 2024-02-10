#include "options.hpp"
#include "util.hpp"

namespace cwt::details
{

terminal_arguments::terminal_arguments(int argc, const char* argv[])
    : m_args(argv, argc)
{
  process();
}
const options& terminal_arguments::get_options() const noexcept
{
  return m_options;
}
const feature_files& terminal_arguments::get_files() const noexcept
{
  return m_feature_files;
}

void terminal_arguments::process_option(std::span<const char*>::iterator it)
{
  std::string_view option(*it);

  if (option.starts_with("-t") || option.starts_with("--tags"))
  {
    std::string_view arg(*std::next(it));
    m_options.tags = compiler::tag_expression(arg);
  }
  else if (option.starts_with("-q") || option.starts_with("--quiet"))
  {
    m_options.quiet = true;
  }
}

void terminal_arguments::find_feature_in_dir(const std::filesystem::path& dir)
{
  for (const auto& entry : std::filesystem::directory_iterator(dir))
  {
    if (std::filesystem::is_directory(entry))
    {
      find_feature_in_dir(entry.path());
    }
    else if (entry.path().extension() == ".feature")
    {
      m_feature_files.push_back(file{entry.path().string(), {}});
    }
  }
}

void terminal_arguments::process_path(std::string_view sv)
{
  namespace fs = std::filesystem;
  auto [feature_file, lines] = filepath_and_lines(sv);
  fs::path path = sv;
  if (fs::exists(path))
  {
    if (fs::is_directory(path))
    {
      find_feature_in_dir(path);
    }
    else if (path.extension() == ".feature")
    {
      m_feature_files.push_back(file{feature_file, lines});
    }
  }
}
void terminal_arguments::process()
{
  for (auto it = m_args.begin(); it != m_args.end(); ++it)
  {
    std::string_view sv{*it};
    if (sv.starts_with('-'))
    {
      process_option(it);
    }
    else
    {
      process_path(sv);
    }
  }
}

}  // namespace cwt::details
