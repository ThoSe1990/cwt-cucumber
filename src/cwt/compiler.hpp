#pragma once

#include <memory>
#include <stack>

#include "chunk.hpp"
#include "parser.hpp"
#include "token.hpp"

namespace cwt::details
{

static constexpr std::string create_string(std::string_view sv);
static constexpr std::string create_string(std::string_view begin,
                                           std::string_view end);
static constexpr std::string create_string(const token& begin,
                                           const token& end);

class compiler
{
 public:
  compiler(std::string_view source);
  compiler(std::string_view source, std::string_view filename);
  [[nodiscard]] function compile();
  [[nodiscard]] bool error() const noexcept;
  [[nodiscard]] bool no_error() const noexcept;

 private:
  [[nodiscard]] chunk& current_chunk();
  [[nodiscard]] chunk pop_chunk();
  void push_chunk(const std::string& name);

  [[nodiscard]] std::string location() const;
  [[nodiscard]] std::size_t create_name(const std::string& location);

  void emit_byte(uint32_t byte);
  void emit_byte(op_code code);
  void emit_bytes(op_code code, uint32_t byte);
  uint32_t emit_jump();
  void patch_jump(uint32_t offset);
  template <typename Arg>
  void emit_constant(op_code code, Arg&& arg)
  {
    emit_bytes(code, current_chunk().make_constant(std::forward<Arg>(arg)));
  }
  template <typename Arg>
  void emit_constant(Arg&& arg)
  {
    emit_bytes(op_code::constant,
               current_chunk().make_constant(std::forward<Arg>(arg)));
  }

  void emit_hook(hook_type type);

 private:
  template <typename C>
  class parent_compiler
  {
   public:
    parent_compiler(compiler* parent) : m_parent(parent)
    {
      m_parent->push_chunk("script");
    }

    parent_compiler(compiler* parent, const std::string& name)
        : m_parent(parent)
    {
      m_parent->push_chunk(name);
    }

    ~parent_compiler() = default;

    void compile() { static_cast<C*>(this)->compile_impl(); }
    compiler* parent() { return m_parent; }

   private:
    compiler* m_parent;
  };

  class feature : public parent_compiler<feature>
  {
   public:
    feature(compiler* p);
    ~feature();
    void compile_impl();
  };
  class scenario : public parent_compiler<scenario>
  {
   public:
    scenario(compiler* parent);
    ~scenario();
    void compile_impl();
  };

 private:
  parser m_parser;
  std::string m_filename;
  std::stack<chunk> m_chunks;
};

}  // namespace cwt::details
