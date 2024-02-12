#include "cucumber.hpp"
#include "box.hpp"

GIVEN(init_box, "An empty box")
{
  const box& my_box = cuke::context<box>();
  cuke::equal(my_box.items_count(), 0);
}

GIVEN(add_item, "I place {int} x {string} in it")
{
  const std::size_t count = CUKE_ARG(1);
  const std::string item = CUKE_ARG(2);

  for ([[maybe_unused]] int i = 0; i < count; i++)
  {
    cuke::context<box>().add_item(item);
  }
}

GIVEN(check_box_size, "The box contains {int} item(s)")
{
  const int items_count = CUKE_ARG(1);
  const box& my_box = cuke::context<box>();
  cuke::equal(my_box.items_count(), items_count);
}