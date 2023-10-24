#pragma once

class box 
{
  public:
    box () = default;
    box (unsigned int side) : 
      m_width(side), m_length(side), m_height(side) {}
    box (unsigned int width, unsigned int height, unsigned int length) : 
      m_width(width), m_length(length), m_height(height) {}
    
    unsigned int volume() 
    {
      return m_width * m_height * m_length;
    }
    unsigned int front_area()
    {
      return m_length * m_height;
    }
    unsigned int side_area()
    {
      return m_width * m_height;
    }
    unsigned int top_area()
    {
      return m_length * m_width;
    }

  private:
    unsigned int m_width;
    unsigned int m_length;
    unsigned int m_height;
};