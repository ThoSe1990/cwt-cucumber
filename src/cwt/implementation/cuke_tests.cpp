#include <iostream>
#include <vector>

#include "../cuke_tests.hpp"
#include "../cucumber.hpp"

namespace cuke
{
  namespace fs = std::filesystem;

  tests::tests() 
  {
    open_cucumber();
    for (const auto& pair : details::steps()) 
    {
        cuke_step(pair.first, pair.second);
    }
    for (const details::hook& h : details::hooks())
    {
        cuke_hook(h.name, h.function, h.tag_expression);
    }
    cuke_hook("reset_context", details::reset_scenario_context, "");
  }

  tests::~tests() 
  {
    close_cucumber();
  }

  int tests::run(int argc, const char* argv[])
  {
    get_feature_files(argc, argv);
    return internal_run();
  }

  void tests::find_feature_in_dir(const fs::path& dir) 
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

  void tests::get_feature_files(int argc, const char* argv[])
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

  int tests::internal_run()
  {
    int result = CUKE_SUCCESS;
    for (const auto& file : m_feature_files)
    {
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
} // namespace cuke