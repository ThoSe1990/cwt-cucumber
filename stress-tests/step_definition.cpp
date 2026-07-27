#include "../src/cucumber.hpp"
#include "../examples/box.hpp"
#include "asserts.hpp"

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

THEN(test, "The {int}. item is {string}")
{
  const std::size_t number = CUKE_ARG(1);
  const std::size_t idx_zero_based = number - 1;
  const std::string item = CUKE_ARG(2);

  cuke::equal(item, cuke::context<box>().at(idx_zero_based));
}

WHEN(doc_string, "There is a doc string:")
{
  const std::string& str = CUKE_DOC_STRING();
  cuke::is_false(str.empty());
}
WHEN(doc_string_vector, "There is a doc string as vector:")
{
  const std::vector<std::string> doc_string = CUKE_DOC_STRING();
  cuke::is_false(doc_string.empty());
}

WHEN(xml_doc_string, "There is a doc string with an xml payload:")
{
  const std::string& doc_string = CUKE_DOC_STRING();
  cuke::is_true(doc_string.find("<note>") != std::string::npos);
}

WHEN(empty_table_cell, "There is an empty table cell")
{
  const auto& table = CUKE_TABLE();
  for (const auto& row : table.raw())
  {
    for (std::size_t i = 0; i < row.col_count(); ++i)
    {
      cuke::is_true(row[i].is_nil());
      cuke::is_true(row[i].as<std::string>().empty());
    }
  }
}

WHEN(empty_cells_in_examples, "Some values {word}, {} and {string} are empty")
{
  const std::string word_value = CUKE_ARG(1);
  cuke::is_true(word_value.empty());

  const std::string anonymous_value = CUKE_ARG(2);
  cuke::is_true(anonymous_value.empty());

  const std::string string_value = CUKE_ARG(3);
  cuke::is_true(string_value.empty());
}

struct point
{
  int x = 0;
  int y = 0;
};

WHEN(point_at, "There is a point at \\({int},{int}\\)")
{
  cuke::context<point>().x = CUKE_ARG(1);
  cuke::context<point>().y = CUKE_ARG(2);
}

THEN(point_check, "The point should be at coordinates {int} and {int}")
{
  const int expected_x = CUKE_ARG(1);
  const int expected_y = CUKE_ARG(2);

  cuke::equal(expected_x, cuke::context<point>().x);
  cuke::equal(expected_y, cuke::context<point>().y);
}

WHEN(items_in_stock, "I have {int} item(s) in stock")
{
  cuke::context<int>() = CUKE_ARG(1);
}

THEN(stock_count_check, "The stock count should be {int}")
{
  const int expected = CUKE_ARG(1);
  cuke::equal(expected, cuke::context<int>());
}

WHEN(raw_output, "I see \\{status\\} in the raw output")
{
  cuke::context<std::string>() = "{status}";
}

THEN(raw_output_check, "The captured text should be {string}")
{
  const std::string expected = CUKE_ARG(1);
  cuke::equal(expected, cuke::context<std::string>());
}
