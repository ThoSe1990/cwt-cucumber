#include "cucumber.hpp"
#include "box.hpp"

GIVEN(init_box, "An empty box")
{
  const box& my_box = cuke::context<box>();
  cuke::equal(my_box.items_count(), 0);
}

WHEN(add_item, "I place {int} x {string} in it")
{
  const std::size_t count = CUKE_ARG(1);
  const std::string item = CUKE_ARG(2);

  cuke::context<box>().add_items(item, count);
}

WHEN(add_table_raw, "I add all items with raw():")
{
  const cuke::table& t = CUKE_TABLE();
  for (const auto& row : t.raw())
  {
    cuke::context<box>().add_items(row[0].to_string(), row[1].copy_as<long>());
  }
}
WHEN(add_table_hashes, "I add all items with hashes():")
{
  const cuke::table& t = CUKE_TABLE();
  for (const auto& row : t.hashes())
  {
    cuke::context<box>().add_items(row["ITEM"].to_string(), row["QUANTITY"].as<long>());
  }
}
WHEN(add_table_rows_hash, "I add the following item with rows_hash():") 
{
  const cuke::table& t = CUKE_TABLE();
  cuke::table::pair hash_rows = t.rows_hash();

  cuke::context<box>().add_items(hash_rows["ITEM"].to_string(), hash_rows["QUANTITY"].as<long>());
}

THEN(check_box_size, "The box contains {int} item(s)")
{
  const int items_count = CUKE_ARG(1);
  const box& my_box = cuke::context<box>();
  cuke::equal(my_box.items_count(), items_count);
}

STEP(doc, "doc string:")
{
  std::string s = CUKE_ARG(1);
  std::cout << s << std::endl;
}