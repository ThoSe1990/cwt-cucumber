#include "../src/cucumber.hpp"

struct date
{
  int day;
  std::string month;
  int year;
};

struct date_range
{
  date start;
  date end;
};

CUSTOM_PARAMETER(
    custom_date, "{date}",
    R"(from ([A-Za-z]+) (\d{1,2}), (\d{4}) to ([A-Za-z]+) (\d{1,2}), (\d{4}))",
    "a custom date pattern")
{
  date begin;
  begin.month = CUKE_PARAM_ARG(1).to_string();
  begin.day = int(CUKE_PARAM_ARG(2));
  begin.year = CUKE_PARAM_ARG(3).as<int>();

  date end;
  end.month = CUKE_PARAM_ARG(4).to_string();
  end.day = static_cast<int>(CUKE_PARAM_ARG(5));
  end.year = CUKE_PARAM_ARG(6).as<int>();

  return date_range{begin, end};
}

CUSTOM_PARAMETER(custom_event, "{event}", R"('(.*?)')", "a custom event")
{
  std::string event = CUKE_PARAM_ARG(1);
  return event;
}

WHEN(using_date, "{event} is {date}")
{
  std::string event = CUKE_ARG(1);
  date_range dr = CUKE_ARG(2);
  std::cout << "Event: " << event << std::endl;
  std::cout << "Begin: " << dr.start.month << ' ' << dr.start.day << ", "
            << dr.start.year << std::endl;
  std::cout << "End: " << dr.end.month << ' ' << dr.end.day << ", "
            << dr.end.year << std::endl;
  cuke::context<date_range>(dr);
}
THEN(checking_months,
     "The beginning month is {word} and the ending month is {word}")
{
  std::string start = CUKE_ARG(1);
  std::string end = CUKE_ARG(2);

  cuke::equal(cuke::context<date_range>().start.month, start);
  cuke::equal(cuke::context<date_range>().end.month, end);
}

CUSTOM_PARAMETER(custom, "{pair of integers}", R"(var1=(\d+), var2=(\d+))",
                 "two integers")
{
  int var1 = CUKE_PARAM_ARG(1);
  int var2 = CUKE_PARAM_ARG(2);
  return std::make_pair(var1, var2);
}

WHEN(custom_par_when, "this is {pair of integers}")
{
  std::pair<int, int> p = CUKE_ARG(1);
  std::cout << "Pair initialized with CUKE_ARG(1) and two values: " << p.first
            << ' ' << p.second << std::endl;
  cuke::context<std::pair<int, int>>(p);
}

THEN(custom_par_then, "their values are {int} and {int}")
{
  const int var1 = CUKE_ARG(1);
  const int var2 = CUKE_ARG(2);
  cuke::equal(cuke::context<std::pair<int, int>>().first, var1);
  cuke::equal(cuke::context<std::pair<int, int>>().second, var2);
}
