
#include "box.h"

static int s_width;
static int s_height;
static int s_depth;
static double s_weight;
static bool s_is_open;

void box_init(int w, int h, int d)
{
  s_width = w;
  s_height = h;
  s_depth = d;
  s_weight = 0;
  s_is_open = false;
}


void set_box_weight(double weight)
{
  s_weight = weight;
}
double box_weight()
{
  return s_weight;
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
void box_open()
{
  s_is_open = true;
}
void box_close()
{
  s_is_open = false;
}

bool box_is_open()
{
  return s_is_open;
}