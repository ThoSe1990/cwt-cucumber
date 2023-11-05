#pragma once 

#include <iostream>

namespace cuke::details
{
  static void print_error() 
  {
    std::cerr << "\n";
  }

  template <typename T, typename... Args>
  static void print_error(const T& t, const Args&... args) 
  {
      std::cerr << "\x1B[31m" << t << "\x1B[0m";
      print_error(args...);
  }

} // namespace cuke::details
