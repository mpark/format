/* TODO */

#pragma once

#include <ostream>

class TRange {
  public:

  constexpr explicit TRange(const char *str_literal, std::size_t size)
      : StrLiteral(str_literal), Size(size) {}

  constexpr const char *Get() const { return StrLiteral; }

  constexpr std::size_t GetSize() const { return Size; }

  void Write(std::ostream &strm) const {
    strm.write(StrLiteral, Size);
  }

  private:

  const char *StrLiteral;

  std::size_t Size;

};  // TRange

std::ostream &operator<<(std::ostream &strm, const TRange &that) {
  that.Write(strm);
  return strm;
}

