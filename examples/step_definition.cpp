#include "../src/cucumber.hpp"
#include "asserts.hpp"
#include "box.hpp"
#include "context.hpp"
#include "defines.hpp"
#include "get_args.hpp"


struct foo 
{
  std::string word;
  std::string anonymous; 
};

WHEN(word_anonymous_given, "A {word} and {}")
{
  std::string word = CUKE_ARG(1);
  cuke::context<foo>().word = word;
 
  std::string anonymous = CUKE_ARG(2);
  cuke::context<foo>().anonymous = anonymous;
}

THEN(word_anonymous_then, "They will match {string} and {string}")
{
  std::string expected_word = CUKE_ARG(1);
  std::string expected_anonymous = CUKE_ARG(2);
  
  cuke::equal(expected_word, cuke::context<foo>().word);
  cuke::equal(expected_anonymous, cuke::context<foo>().anonymous);
}

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
WHEN(doc_string, "There is a doc string:")
{
  const std::string& str = CUKE_DOC_STRING();
  std::cout << "-------- Print from step definition: -----" << '\n';
  std::cout << str << '\n';
  std::cout << "------------------------------------------" << '\n';
}
WHEN(doc_string_vector, "There is a doc string as vector:")
{
  const std::vector<std::string> doc_string = CUKE_DOC_STRING();
  std::cout << "-------- Print from step definition: -----" << '\n';
  for (const std::string& line : doc_string)
  {
    std::cout << line << '\n';
  }
  std::cout << "------------------------------------------" << '\n';
}
WHEN(add_table_raw, "I add all items with raw():")
{
  const cuke::table& t = CUKE_TABLE();
  for (const cuke::table::row& row : t.raw())
  {
    cuke::context<box>().add_items(row[0].to_string(), row[1].copy_as<long>());
  }
}
WHEN(add_table_hashes, "I add all items with hashes():")
{
  const cuke::table& t = CUKE_TABLE();
  for (const auto& row : t.hashes())
  {
    cuke::context<box>().add_items(row["ITEM"].to_string(),
                                   row["QUANTITY"].as<long>());
  }
}
WHEN(add_table_rows_hash, "I add the following item with rows_hash():")
{
  const cuke::table& t = CUKE_TABLE();
  cuke::table::pair hash_rows = t.rows_hash();

  cuke::context<box>().add_items(hash_rows["ITEM"].to_string(),
                                 hash_rows["QUANTITY"].as<long>());
}

THEN(test, "The {int}. item is {string}")
{
  const std::size_t number = CUKE_ARG(1);
  const std::size_t idx_zero_based = number - 1;
  const std::string item = CUKE_ARG(2);

  cuke::equal(item, cuke::context<box>().at(idx_zero_based));
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
