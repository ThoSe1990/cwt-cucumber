#pragma once

#include <iostream>
#include <utility>

namespace cuke::log
{

constexpr static const char* reset_color = "\x1b[0m";
constexpr static const char* green = "\x1b[32m";
constexpr static const char* yellow = "\x1b[33m";
constexpr static const char* red = "\x1b[31m";
constexpr static const char* blue = "\x1b[38;2;100;149;237m";
constexpr static const char* black = "\x1b[90m";
constexpr static const char new_line = '\n';

enum class level
{
  verbose = 0,
  info,
  quiet,
  error,
  always
};

class logger
{
 public:
  static logger& instance()
  {
    static logger cwt_logger;
    return cwt_logger;
  }

  void set_level(level l) { m_level = l; }

  template <typename... Args>
  void Log(level l, Args&&... args)
  {
    if (l < m_level)
    {
      return;
    }

    (std::cout << ... << std::forward<Args>(args));
  }

  template <typename... Args>
  void quiet(Args&&... args)
  {
    Log(level::quiet, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void always(Args&&... args)
  {
    Log(level::always, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void verbose(Args&&... args)
  {
    Log(level::error, log::black);
    Log(level::verbose, std::forward<Args>(args)...);
    Log(level::error, log::reset_color);
  }

  template <typename... Args>
  void info(Args&&... args)
  {
    Log(level::info, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void error(Args&&... args)
  {
    Log(level::error, log::red);
    Log(level::error, std::forward<Args>(args)...);
    Log(level::error, log::reset_color);
  }

 private:
  logger() = default;
  level m_level = level::info;
};

inline void set_level(level l) { logger::instance().set_level(l); }

template <typename... Args>
void error(Args&&... args)
{
  logger::instance().error(std::forward<Args>(args)...);
}

template <typename... Args>
void always(Args&&... args)
{
  logger::instance().always(std::forward<Args>(args)...);
}

template <typename... Args>
void quiet(Args&&... args)
{
  logger::instance().quiet(std::forward<Args>(args)...);
}

template <typename... Args>
void verbose(Args&&... args)
{
  logger::instance().verbose(std::forward<Args>(args)...);
}

template <typename... Args>
void info(Args&&... args)
{
  logger::instance().info(std::forward<Args>(args)...);
}

}  // namespace cuke::log
