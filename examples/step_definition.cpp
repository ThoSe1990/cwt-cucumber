#include "../src/cucumber.hpp"
#include "box.hpp"

GIVEN(box_init_empty, "An empty box")
{
  const box& my_box = cuke::context<box>();
  cuke::equal(my_box.items_count(), 0);
}
GIVEN(box_with_label, "An empty box with a label")
{
  std::string label = CUKE_DOC_STRING();
  cuke::context<box>(label);
}
WHEN(add_item, "I place {int} x {string} in it")
{
  const std::size_t count = CUKE_ARG(1);
  const std::string item = CUKE_ARG(2);

  cuke::context<box>().add_items(item, count);
}
WHEN(add_table_raw, "I add all items with the raw function:")
{
  const cuke::table& t = CUKE_TABLE();
  for (const cuke::table::row& row : t.raw())
  {
    cuke::context<box>().add_items(row[0].to_string(), row[1].as<long>());
  }
}
WHEN(add_table_hashes, "I add all items with the hashes function:")
{
  const cuke::table& t = CUKE_TABLE();
  for (const auto& row : t.hashes())
  {
    cuke::context<box>().add_items(row["ITEM"].to_string(),
                                   row["QUANTITY"].as<long>());
  }
}
WHEN(add_table_rows_hash,
     "I add the following item with the rows_hash function:")
{
  const cuke::table& t = CUKE_TABLE();
  cuke::table::pair hash_rows = t.rows_hash();

  cuke::context<box>().add_items(hash_rows["ITEM"].to_string(),
                                 hash_rows["QUANTITY"].as<long>());
}
THEN(box_is_labeled, "The box is labeled")
{
  cuke::is_false(cuke::context<box>().label().empty());
}
THEN(box_item_at_index, "The {int}. item is {string}")
{
  const std::size_t number = CUKE_ARG(1);
  const std::size_t idx_zero_based = number - 1;
  const std::string item = CUKE_ARG(2);

  cuke::equal(item, cuke::context<box>().at(idx_zero_based));
}

THEN(box_count_items, "The box contains {int} item(s)")
{
  const int items_count = CUKE_ARG(1);
  const box& my_box = cuke::context<box>();
  cuke::equal(my_box.items_count(), items_count);
}

THEN(box_find_item, "{word} is/are in the box")
{
  const std::string item = CUKE_ARG(1);
  cuke::is_true(cuke::context<box>().contains(item));
}

THEN(box_count_items_alternative, "{int} item(s) is/are {string}")
{
  const std::size_t count = CUKE_ARG(1);
  const std::string item = CUKE_ARG(2);
  cuke::equal(count, cuke::context<box>().count(item));
}
