/*
   Features to work on 

   Arguments by name.

*/

#include <format/format.h>

#include <gtest/gtest.h>

template <typename TVal>
std::string ToString(TVal &&val) {
  std::ostringstream strm;
  strm << std::forward<TVal>(val);
  return strm.str();
}

struct TDate {

  void Write(std::ostream &strm) const {
    // Use Format here too!
    strm << FS("{0}-{1}-{2}").Format(Year, Month, Day);
  }

  int Year, Month, Day;

};  // TDate

std::ostream &operator<<(std::ostream &strm, const TDate &that) {
  that.Write(strm);
  return strm;
}

TEST(Format, Typical) {
  /* Error cases.
  std::cout << FS("{0}").Format();  // Index out of range.
  std::cout << FS("{").Format();  // Single '{' encountered in format string.
  std::cout << FS("}").Format();  // Single '}' encountered in format string.
  // Failed to find a viable operator<<.
  struct foo_t {};
  std::cout << FS("{0}").Format(foo_t());
  // The result of format() must be used immediately within the expression.
  auto save = FS("{0}").Format(42);
  std::cout << save;
  */
  auto expect_eq = [](const auto &lhs, const auto &rhs) { EXPECT_EQ(lhs, rhs); };
  // Typical use cases.
  expect_eq(
      ToString(FS("so much depends upon {0}").Format("a red wheel barrow")),
      "so much depends upon a red wheel barrow");
  expect_eq(ToString(FS("glazed with {0} water beside the {1} chickens\n")
                         .Format("rain", "white")),
            "glazed with rain water beside the white chickens\n");
  expect_eq(ToString(FS("{0} is better than {1}.\n").Format("vim", "emacs")),
            "vim is better than emacs.\n");
  expect_eq(ToString(FS("{1} is better than {0}.\n").Format("vim", "emacs")),
            "emacs is better than vim.\n");
  expect_eq(ToString(FS("I {0} the {1} off the {2}")
                         .Format("took", "cheese", "table")),
            "I took the cheese off the table");
  expect_eq(
      ToString(FS("Oh {0}, {0}! wherefore art thou {0}?").Format("Romeo")),
      "Oh Romeo, Romeo! wherefore art thou Romeo?");
  // Escaped braces.
  expect_eq(ToString(FS("The {0} set is often represented as {{0}}\n")
                         .Format("empty")),
            "The empty set is often represented as {0}\n");
  // Nested formatting.
  expect_eq(
      ToString(FS("{0} {1}\n").Format(FS("Hello, {0}!").Format("there"),
                                      FS("My name is {0}.").Format("mpark"))),
      "Hello, there! My name is mpark.\n");
  // Reusing template.
  auto bordered = FS("{0}{1}{0}");
  expect_eq(ToString(bordered.Format("abra", "cad")), "abracadabra");
  expect_eq(ToString(bordered.Format("ph", "otogra")), "photograph");
  // Custom type.
  expect_eq(ToString(FS("The date is {0}").Format(TDate{2012, 12, 9})),
            "The date is 2012-12-9");
}

