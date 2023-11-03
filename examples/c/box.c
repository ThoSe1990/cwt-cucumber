
#include "box.h"

void box_init(int w, int h, int d)
{
  s_width = w;
  s_height = h;
  s_depth = d;
  s_weight = 0;
}

int box_front()
{
  return s_width * s_height;
}
int box_side()
{
  return s_depth * s_height;
}
int box_top()
{
  return s_depth * s_width;
}
int box_volume() 
{
  return s_width * s_height * s_depth;
}
