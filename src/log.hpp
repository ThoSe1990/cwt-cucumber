#pragma once

#include <iostream>
#include <utility>
#include <cstdlib>
#include <cstdio>
#ifdef _WIN32
#include <io.h>
#define isatty _isatty
#define fileno _fileno
#else
#include <unistd.h>
#endif

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
  report
};

class logger
{
 public:
  static logger& instance()
  {
    static logger cwt_logger;
    return cwt_logger;
  }

  void disable() { m_disabled = true; }
  void enable() { m_disabled = false; }
  void set_level(level l) { m_level = l; }
  void enable_colors(bool enable) { m_use_color = enable; }
  bool colors_enabled() const { return m_use_color; }

  template <typename... Args>
  void Log(level l, Args&&... args)
  {
    if (l < m_level) return;
    if (m_disabled) return;

    (std::cout << ... << std::forward<Args>(args));
  }

  template <typename... Args>
  void quiet(Args&&... args)
  {
    Log(level::quiet, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void report(Args&&... args)
  {
    Log(level::report, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void verbose(Args&&... args)
  {
    if (m_use_color) Log(level::verbose, log::black);
    Log(level::verbose, std::forward<Args>(args)...);
    if (m_use_color) Log(level::verbose, log::reset_color);
  }

  template <typename... Args>
  void info(Args&&... args)
  {
    Log(level::info, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void error(Args&&... args)
  {
    if (m_use_color) Log(level::error, log::red);
    Log(level::error, std::forward<Args>(args)...);
    if (m_use_color) Log(level::error, log::reset_color);
  }

 private:
  logger()
  {
    bool is_tty = static_cast<bool>(isatty(fileno(stdout)));
    const char* no_color_env = std::getenv("NO_COLOR");
    m_use_color = is_tty && (no_color_env == nullptr);
  }

 private:
  level m_level = level::info;
  bool m_disabled = false;
  bool m_use_color = true;
};

inline void disable() { logger::instance().disable(); }
inline void enable() { logger::instance().enable(); }
inline void set_level(level l) { logger::instance().set_level(l); }

template <typename... Args>
void error(Args&&... args)
{
  logger::instance().error(std::forward<Args>(args)...);
}

template <typename... Args>
void report(Args&&... args)
{
  logger::instance().report(std::forward<Args>(args)...);
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

/// `@brief` Write directly to stdout, bypassing logger settings
/// e.g. writing JSON results to stdout for piping
template <typename... Args>
void to_stdout(Args&&... args)
{
  (std::cout << ... << std::forward<Args>(args));
}

namespace color
{

inline const char* reset()
{
  return logger::instance().colors_enabled() ? log::reset_color : "";
}
inline const char* green()
{
  return logger::instance().colors_enabled() ? log::green : "";
}
inline const char* yellow()
{
  return logger::instance().colors_enabled() ? log::yellow : "";
}
inline const char* red()
{
  return logger::instance().colors_enabled() ? log::red : "";
}
inline const char* blue()
{
  return logger::instance().colors_enabled() ? log::blue : "";
}
inline const char* black()
{
  return logger::instance().colors_enabled() ? log::black : "";
}
}  // namespace color

}  // namespace cuke::log
