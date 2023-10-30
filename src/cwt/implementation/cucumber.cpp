#include "../cucumber.hpp"

namespace cuke::details
{
  namespace fs = std::filesystem;

  runner::~runner() 
  {
    close_cucumber();
  }

  void runner::init()
  {
    open_cucumber();
    for (const auto& pair : steps()) 
    {
        cuke_step(pair.first, pair.second);
    }
    for (const hook& h : hooks())
    {
        cuke_hook(h.name, h.function, h.tag_expression);
    }
    cuke_hook("reset_context", reset_scenario_context, "");
  }

  int runner::run(int argc, const char* argv[])
  {
    get_feature_files(argc, argv);
    cuke_options(argc, argv);
    return internal_run();
  }

  void runner::find_feature_in_dir(const fs::path& dir) 
  {  
    for (const auto& entry : fs::directory_iterator(dir)) 
    {
      if (fs::is_directory(entry)) 
      {
        find_feature_in_dir(entry.path());
      } 
      else if (entry.path().extension() == ".feature") 
      {
        m_feature_files.push_back(entry.path());
      }
    }
  }

  void runner::get_feature_files(int argc, const char* argv[])
  {
    for (int i = 1; i < argc; ++i) 
    {
      fs::path path = std::string{argv[i]};
      if (fs::exists(path)) 
      {
        if (fs::is_directory(path)) 
        {
          find_feature_in_dir(path);
        } 
        else if (path.extension() == ".feature") 
        {
          m_feature_files.push_back(path);
        }
      }
    }
  }

  int runner::internal_run()
  {
    int result = CUKE_SUCCESS;
    for (const auto& file : m_feature_files)
    {
      // TODO -l option!
      char* source = read_file(file.c_str());
      if (run_cuke(source, file.c_str()) == CUKE_FAILED)
      {
        result = CUKE_FAILED;
      } 
      free(source);
    }
    print_final_result();
    return result;
  }
  
  std::vector<std::pair<const char*, cuke_step_t>>& steps()
  {
    static std::vector<std::pair<const char*, cuke_step_t>> s;
    return s;
  }

  std::vector<hook>& hooks()
  {
    static std::vector<hook> h;
    return h;
  }

  runner& get_runner()
  {
    static runner r;
    return r;
  }

  void init()
  {
    get_runner().init();
  }
  int run(int argc, const char* argv[])
  {
    return get_runner().run(argc, argv);
  }
} // namespace cuke::details