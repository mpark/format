/* Missing features:

     1. Formatting of arguments.
     2. Pass arguments by name.
     3. Allow FormatString construction from dynamic strings.  */

#include <format/format.h>

#include <cassert>
#include <sstream>

template <typename TLhs, typename TRhs>
void ExpectEq(const TLhs &lhs, const TRhs &rhs) {
  assert(lhs == rhs);
}

template <typename TVal>
std::string ToString(TVal &&val) {
  std::ostringstream strm;
  strm << std::forward<TVal>(val);
  return strm.str();
}

struct TDate { int Year, Month, Day; };

std::ostream &operator<<(std::ostream &strm, const TDate &that) {
  return strm << FS("{0}-{1}-{2}").Format(that.Year, that.Month, that.Day);
}

int main() {
  // Typical use cases.
  ExpectEq(
      ToString(FS("so much depends upon {0}").Format("a red wheel barrow")),
      "so much depends upon a red wheel barrow");
  ExpectEq(ToString(FS("glazed with {0} water beside the {1} chickens\n")
                         .Format("rain", "white")),
            "glazed with rain water beside the white chickens\n");
  ExpectEq(ToString(FS("{0} is better than {1}.\n").Format("vim", "emacs")),
            "vim is better than emacs.\n");
  ExpectEq(ToString(FS("{1} is better than {0}.\n").Format("vim", "emacs")),
            "emacs is better than vim.\n");
  ExpectEq(ToString(FS("I {0} the {1} off the {2}")
                         .Format("took", "cheese", "table")),
            "I took the cheese off the table");
  ExpectEq(
      ToString(FS("Oh {0}, {0}! wherefore art thou {0}?").Format("Romeo")),
      "Oh Romeo, Romeo! wherefore art thou Romeo?");
  // Escaped braces.
  ExpectEq(ToString(FS("The {0} set is often represented as {{0}}\n")
                         .Format("empty")),
            "The empty set is often represented as {0}\n");
  // Nested formatting.
  ExpectEq(
      ToString(FS("{0} {1}\n").Format(FS("Hello, {0}!").Format("there"),
                                      FS("My name is {0}.").Format("mpark"))),
      "Hello, there! My name is mpark.\n");
  // Reusing template.
  auto bordered = FS("{0}{1}{0}");
  ExpectEq(ToString(bordered.Format("abra", "cad")), "abracadabra");
  ExpectEq(ToString(bordered.Format("ph", "otogra")), "photograph");
  // Custom type.
  ExpectEq(ToString(FS("The date is {0}").Format(TDate{2012, 12, 9})),
            "The date is 2012-12-9");
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
}

