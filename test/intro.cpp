// MPark.Format
//
// Copyright Michael Park, 2014-2017
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <mpark/format.hpp>

#include <ostream>
#include <sstream>
#include <string>

#include <gtest/gtest.h>

/* Missing features:

     1. Formatting of arguments.
     2. Pass arguments by name.
     3. Allow FormatString construction from dynamic strings.  */

void expect_eq(const std::string &expected, const std::string &actual) {
  EXPECT_EQ(expected, actual);
}

struct Date { int year, month, day; };

std::ostream &operator<<(std::ostream &strm, const Date &that) {
  return strm << mpark::format(
             FS("{0}-{1}-{2}"), that.year, that.month, that.day);
}

TEST(Format, Intro) {
  // error: Index out of range.
  // std::cout << mpark::format(FS("{0}"));

  // error: Single '{' encountered in format string.
  // std::cout << mpark::format(FS("{"));

  // error: Single '}' encountered in format string.
  // std::cout << mpark::format(FS("}"));

  // error: Result of `format` must be used immediately within the expression!
  // auto save = mpark::format(FS("{0}"), 42);
  // std::cout << save;

  expect_eq(mpark::format(FS("so much depends upon {0}"), "a red wheel barrow"),
            "so much depends upon a red wheel barrow");

  expect_eq(mpark::format(FS("glazed with {0} water beside the {1} chickens\n"),
                          "rain",
                          "white"),
            "glazed with rain water beside the white chickens\n");

  expect_eq(mpark::format(FS("{0} is better than {1}.\n"), "vim", "emacs"),
            "vim is better than emacs.\n");

  expect_eq(mpark::format(FS("{1} is better than {0}.\n"), "vim", "emacs"),
            "emacs is better than vim.\n");

  expect_eq(
      mpark::format(FS("I {0} the {1} off the {2}"), "took", "cheese", "table"),
      "I took the cheese off the table");

  expect_eq(mpark::format(FS("Oh {0}, {0}! wherefore art thou {0}?"), "Romeo"),
            "Oh Romeo, Romeo! wherefore art thou Romeo?");

  // Escaped braces.
  expect_eq(
      mpark::format(FS("The {0} set is often represented as {{0}}\n"), "empty"),
      "The empty set is often represented as {0}\n");

  // Nested formatting.
  expect_eq(mpark::format(FS("{0} {1}\n"),
                          mpark::format(FS("Hello, {0}!"), "there"),
                          mpark::format(FS("My name is {0}."), "mpark")),
            "Hello, there! My name is mpark.\n");

  // Reusing a template.
  auto bordered = FS("{0}{1}{0}");
  expect_eq(mpark::format(bordered, "abra", "cad"), "abracadabra");
  expect_eq(mpark::format(bordered, "ph", "otogra"), "photograph");

  // Custom type.
  expect_eq(mpark::format(FS("The date is {0}"), Date{2012, 12, 9}),
            "The date is 2012-12-9");
}
