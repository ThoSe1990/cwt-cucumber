
#include "cwt/cucumber.h"
#include <stdio.h>


static int s_width;
static int s_height;
static int s_depth;
static int s_weight;

static void box_init(int w, int h, int d)
{
  s_width = w;
  s_height = h;
  s_depth = d;
  s_weight = 0;
}

static int box_front()
{
  return s_width * s_height;
}
static int box_side()
{
  return s_depth * s_height;
}
static int box_top()
{
  return s_depth * s_width;
}
static int box_volume() 
{
  return s_width * s_height * s_depth;
}




static void step_init(int arg_count, cuke_value* args)
{
  box_init(
    cuke_to_int(&args[0]),
    cuke_to_int(&args[1]),
    cuke_to_int(&args[2])
  );
}
static void step_front(int arg_count, cuke_value* args)
{
  const int front = box_front();
  const int expected = cuke_to_int(&args[0]);
  cuke_assert(front == expected, "box front area is not %d\n", expected);
}
static void step_side(int arg_count, cuke_value* args)
{
  const int side = box_side();
  const int expected = cuke_to_int(&args[0]);
  cuke_assert(side == expected, "box side area is not %d\n", expected);
}
static void step_top(int arg_count, cuke_value* args)
{
  const int top = box_top();
  const int expected = cuke_to_int(&args[0]);
  cuke_assert(top == expected, "box top area is not %d\n", expected);
}
static void step_volume(int arg_count, cuke_value* args)
{
  const int volume = box_volume();
  const int expected = cuke_to_int(&args[0]);
  cuke_assert(volume == expected, "box volume is not %d\n", expected);
}
static void step_water(int arg_count, cuke_value* args)
{
  // 1 liter water == 1 kg
  s_weight = cuke_to_int(&args[0]);
}
static void step_weight(int arg_count, cuke_value* args)
{
  const int expected = cuke_to_int(&args[0]);
  cuke_assert(s_weight == expected, "box weight is not %d\n", expected);
}


int main(int argc, const char* argv[])
{
  open_cucumber();

  cuke_step("A box with {int} x {int} x {int}", step_init);
  
  cuke_step("Put {int} liter water into the box", step_water);

  cuke_step("The volume is {int}", step_volume);
  cuke_step("The weight is {int} kg", step_weight);
  cuke_step("The front area is {int}", step_front);
  cuke_step("The side area is {int}", step_side);
  cuke_step("The top area is {int}", step_top);


  run_cuke_argc_argv(argc, argv);
  
  close_cucumber();

  return 0;
}