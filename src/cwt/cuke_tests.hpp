#pragma once 

#include <filesystem>

namespace cuke
{
  namespace fs = std::filesystem;

  class tests 
  {
    public:
      tests();
      ~tests();
      int run(int argc, const char* argv[]);

    private:
      void find_feature_in_dir(const fs::path& dir);
      void get_feature_files(int argc, const char* argv[]);
      int internal_run();

    private:
      std::vector<fs::path> m_feature_files;      
  };

} // namespace cuke
