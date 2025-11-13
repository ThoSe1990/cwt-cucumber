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
