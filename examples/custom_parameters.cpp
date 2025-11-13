#include "../src/cucumber.hpp"
#include "asserts.hpp"
#include "box.hpp"
#include "get_args.hpp"

struct date
{
  int year{0};
  int month{0};
  int day{0};
  auto operator<=>(const date& other) const = default;
  std::string to_string() { return std::format("{}-{}-{}", year, month, day); }
};

CUSTOM_PARAMETER(custom_parameter_date, "{date}", R"((\d{4})-(\d{2})-(\d{2}))",
                 "shipping date")
{
  date shipping_date;
  shipping_date.year = CUKE_PARAM_ARG(1).as<int>();
  shipping_date.month = CUKE_PARAM_ARG(2).as<int>();
  shipping_date.day = CUKE_PARAM_ARG(3).as<int>();

  return shipping_date;
}

WHEN(ship_the_box, "The box gets shipped at {date}")
{
  date shipping_date = CUKE_ARG(1);
  cuke::context<date>(shipping_date);
}

THEN(box_label, "The box is labeled with: {string}")
{
  std::string shipping_date = cuke::context<date>().to_string();
  std::string box_label = CUKE_ARG(1);
  cuke::is_true(box_label.find(shipping_date) != std::string::npos);
}

CUSTOM_PARAMETER(item_with_weight, "{item with weight}",
                 R"((\d+) kilograms? of (\w+))", "")
{
  int weight = CUKE_PARAM_ARG(1);
  std::string item = CUKE_PARAM_ARG(2);
  return std::make_pair(weight, item);
}

WHEN(add_item_with_weight, "I put {item with weight} in it")
{
  std::pair<int, std::string> item_with_weigth = CUKE_ARG(1);
  box& b = cuke::context<box>();
  b.add_item(item_with_weigth.second);
  b.add_weight(item_with_weigth.first);
}

THEN(check_weight, "The box weights {int} kilogram(s)")
{
  const int weight = CUKE_ARG(1);
  cuke::equal(weight, cuke::context<box>().weight());
}
