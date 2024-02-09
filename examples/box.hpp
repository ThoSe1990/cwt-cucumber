#pragma once

#include <string> 

class box 
{
  public:
    box () = default;
    box (unsigned int width, unsigned int height, unsigned int depth) : 
      m_width(width), m_height(height), m_depth(depth), m_weight(0), m_is_open(false),
      m_name(""), m_description("") {}
    
    unsigned int volume() const
    {
      return m_width * m_height * m_depth;
    }
    unsigned int front() const
    {
      return m_width * m_height;
    }
    unsigned int side() const 
    {
      return m_depth * m_height;
    }
    unsigned int top() const
    {
      return m_depth * m_width;
    }
    void set_weight(double weight)
    {
      m_weight = weight;
    }
    void set_name(const std::string& name)
    {
      m_name = name;
    }
    const std::string& name() const
    {
      return m_name;
    }
    void set_description(const std::string& description)
    {
      m_description = description;
    }
    const std::string& description() const
    {
      return m_description;
    }
    double weight()
    {
      return m_weight;
    }
    void open() 
    {
      m_is_open = true;
    }
    void close() 
    {
      m_is_open = false;
    }
    bool is_open()
    {
      return m_is_open;
    }

  private:
    unsigned int m_width;
    unsigned int m_height;
    unsigned int m_depth;
    double m_weight;
    bool m_is_open;
    std::string m_name;
    std::string m_description;
};