
#include "step_definition.h"
#include "box.h"

void step_init(int arg_count, cuke_value* args)
{
  box_init(
    cuke_to_int(&args[0]),
    cuke_to_int(&args[1]),
    cuke_to_int(&args[2])
  );
}
void step_front(int arg_count, cuke_value* args)
{
  const int front = box_front();
  const int expected = cuke_to_int(&args[0]);
  cuke_assert(front == expected, "box front area is not %d", expected);
}
void step_side(int arg_count, cuke_value* args)
{
  const int side = box_side();
  const int expected = cuke_to_int(&args[0]);
  cuke_assert(side == expected, "box side area is not %d", expected);
}
void step_top(int arg_count, cuke_value* args)
{
  const int top = box_top();
  const int expected = cuke_to_int(&args[0]);
  cuke_assert(top == expected, "box top area is not %d", expected);
}
void step_volume(int arg_count, cuke_value* args)
{
  const int volume = box_volume();
  const int expected = cuke_to_int(&args[0]);
  cuke_assert(volume == expected, "box volume is not %d", expected);
}
void step_water(int arg_count, cuke_value* args)
{
  // 1 liter water == 1 kg
  s_weight = cuke_to_double(&args[0]);
}
void step_weight(int arg_count, cuke_value* args)
{
  const double expected = cuke_to_double(&args[0]);
  cuke_assert(s_weight == expected, "box weight is not %d", expected);
}
