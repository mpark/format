# MPark.Format

> Compile-time Checked, Type-Safe Formatting in __C++14__.

[![stability][badge.stability]][stability]
[![license][badge.license]][license]
[![wandbox][badge.wandbox]][wandbox]

[badge.stability]: https://img.shields.io/badge/stability-experimental-orange.svg
[badge.license]: http://img.shields.io/badge/license-boost-blue.svg
[badge.wandbox]: https://img.shields.io/badge/try%20it-on%20wandbox-green.svg

[stability]: http://github.com/badges/stability-badges
[license]: https://github.com/mpark/patterns/blob/master/LICENSE_1_0.txt
[wandbox]: https://wandbox.org/permlink/NfSCfnToS2QCJy36

## Introduction

__MPark.Format__ is an experimental compile-time checked, type-safe formatting
library for __C++14__.

Currently, it supports Python-like format strings with positional parameters.

```cpp
#include <iostream>
#include <ostream>

#include <mpark/format.hpp>

struct Date {
  int year, month, day;
};

std::ostream &operator<<(std::ostream &strm, const Date &that) {
  return strm << mpark::format(
             FS("{0}-{1}-{2}"), that.year, that.month, that.day);
}

int main() {
  std::cout << mpark::format(FS("{0}{1}{0}\n"), "abra", "cad");
  // prints: "abracadabra"

  // std::cout << mpark::format(FS("{0}, {1}\n"), 'x');
  // error: Index out of range.

  std::cout << mpark::format(FS("The date is {0}.\n"), Date{2016, 4, 19});
  // prints: "The date is 2016-4-19."
}
```
