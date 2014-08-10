/* TODO */

#pragma once

#include <tuple>
#include <type_traits>

#include <format/false.h>
#include <format/range.h>

class TParseBase {
  protected:

  static constexpr bool IsDoubleOpen(const char *text, std::size_t size) {
    return *text == '{' && size > 1 && *(text + 1) == '{';
  }

  static constexpr bool IsDoubleClose(const char *text, std::size_t size) {
    return *text == '}' && size > 1 && *(text + 1) == '}';
  }

  static constexpr bool IsOpen(const char *text, std::size_t size) {
    return *text == '{' && !IsDoubleOpen(text, size);
  }

  static constexpr bool IsClose(const char *text) {
    return *text == '}';
  }

  static constexpr auto ConvertImpl(TRange range, std::true_type) {
    std::size_t idx = 0;
    for (std::size_t i = 1; i < range.GetSize() - 1; ++i) {
      idx *= 10;
      idx += range.Get()[i] - '0';
    }  // for
    return idx;
  }

  static constexpr auto ConvertImpl(TRange range, std::false_type) {
    return range;
  }

  template <typename TCharsView>
  static constexpr auto Convert(TCharsView chars_view) {
    return ConvertImpl(
        TRange(chars_view.Get(), chars_view.GetSize()),
        std::integral_constant<
            bool,
            IsOpen(chars_view.Get(), chars_view.GetSize()) &&
            IsClose(chars_view.Get() + chars_view.GetSize() - 1)>());
  }

  enum class TCode {
    Success,
    SingleOpen,
    SingleClose
  };  // TCode

  template <TCode Code, typename TDummy = void>
  struct TReport;

  template <typename TDummy>
  struct TReport<TCode::Success, TDummy> {};

  template <typename TDummy>
  struct TReport<TCode::SingleOpen, TDummy> {
    static_assert(TFalse<TDummy>::value,
                  "Single '{' encountered in format string.");
  };  // TReport<TCode::SingleOpen>

  template <typename TDummy>
  struct TReport<TCode::SingleClose, TDummy> {
    static_assert(TFalse<TDummy>::value,
                  "Single '}' encountered in format string.");
  };  // TReport<TCode::SingleClose>

  static constexpr auto GetState(const char *text, std::size_t size) {
    struct TResult {
      TCode Code;
      const char *End;
      const char *Next;
    };  // TResult
    std::size_t i = 0;
    if (IsOpen(text + i, size - i)) {
      while (i < size && !IsClose(text + i)) {
        ++i;
      }  // while
      if (i >= size) {
        return TResult{TCode::SingleOpen, nullptr, nullptr};
      }  // if
      ++i;
    } else {
      while (i < size && !IsOpen(text + i, size - i)) {
        if (IsDoubleOpen(text + i, size - i) ||
            IsDoubleClose(text + i, size - i)) {
          return TResult{TCode::Success, text + i + 1, text + i + 2};
        }  // if
        if (IsClose(text + i)) {
          return TResult{TCode::SingleClose, nullptr, nullptr};
        }  // if
        ++i;
      }  // while
    }  // if
    return TResult{TCode::Success, text + i, text + i};
  }

};  // TParseBase

template <std::size_t Size>
class TParse;

template <>
class TParse<0> {
  public:

  template <typename TChars>
  constexpr auto operator()(TChars) const {
    return std::make_tuple();
  }

};  // TParse<0>

template <std::size_t Size>
class TParse : TParseBase {
  public:

  template <typename TCur>
  constexpr auto operator()(TCur) const {
    struct TState {
      static constexpr auto Get() { return GetState(TCur::Get(), Size); }
    };  // TState
    TReport<TState::Get().Code>();
    struct TCharsView {
      static constexpr auto Get() { return TCur::Get(); }
      static constexpr auto GetSize() { return TState::Get().End - Get(); }
    };  // TCharsView
    struct TNext {
      static constexpr auto Get() { return TState::Get().Next; }
    };  // TNext
    constexpr std::size_t size = TNext::Get() - TCur::Get();
    return std::tuple_cat(std::make_tuple(Convert(TCharsView())),
                          TParse<Size - size>()(TNext()));
  }

};  // TParse<Size>

template <typename TChars>
constexpr auto Parse(TChars chars) {
  return TParse<chars.GetSize()>()(chars);
}

