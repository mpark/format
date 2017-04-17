/* TODO */

#pragma once

#include <format/chars.h>
#include <format/false.h>
#include <format/parse.h>

#define FS(format) MakeFormatStr(STR(format))

template <typename TTuple, typename TArgs>
class TFormat {
  public:

  explicit TFormat(TArgs &&args) : Args(std::move(args)) {}

  void Write(std::ostream &strm) && {
    using tuple_t = decltype(TTuple::Get());
    WriteImpl(strm,
              TTuple(),
              std::make_index_sequence<std::tuple_size<tuple_t>::value>(),
              std::move(Args));
  }

  private:

  template <typename T, typename TDummy = void>
  struct TWriteElemImpl;

  template <typename TDummy>
  struct TWriteElemImpl<std::size_t, TDummy> {

    template <typename TElem>
    auto Dispatch(std::ostream &strm,
                  TElem,
                  TArgs &&args,
                  std::nullptr_t) const
          -> decltype(strm << std::get<TElem::Get()>(std::move(args)), void()) {
      strm << std::get<TElem::Get()>(std::move(args));
    }

    template <typename TElem>
    void Dispatch(std::ostream &, TElem, TArgs &&, void *) const {
      static_assert(
          TFalse<std::tuple_element_t<TElem::Get(), TArgs>>::value,
          "Failed to find a viable operator<<.");
    }

    template <typename TElem>
    void operator()(std::ostream &strm, TElem, TArgs &&args) const {
      static_assert(TElem::Get() < std::tuple_size<TArgs>::value,
                    "Index out of range.");
      Dispatch(strm, TElem(), std::move(args), nullptr);
    }

  };  // TWriteElemImpl<std::size_t>

  template <typename TDummy>
  struct TWriteElemImpl<TRange, TDummy> {

    template <typename TElem>
    void operator()(std::ostream &strm, TElem, TArgs &&) const {
      strm << TElem::Get();
    }

  };  // TWriteElemImpl<TRange>

  template <typename TElem>
  static constexpr void WriteElem(std::ostream &strm, TElem, TArgs &&args) {
    TWriteElemImpl<decltype(TElem::Get())>()(strm, TElem(), std::move(args));
  }

  template <std::size_t... Is>
  static void WriteImpl(std::ostream &strm,
                        TTuple,
                        std::index_sequence<Is...>,
                        TArgs &&args) {
    std::initializer_list<int>(
        {(WriteElem(
              strm,
              []() {
                struct TElem {
                  static constexpr auto Get() {
                    return std::get<Is>(TTuple::Get());
                  }
                };  // TElem
                return TElem();
              }(),
              std::move(args)),
          0)...});
  }

  TArgs Args;

};  // TFormat

template <typename TTuple, typename TArgs>
std::ostream &operator<<(std::ostream &strm, TFormat<TTuple, TArgs> &&that) {
  std::move(that).Write(strm);
  return strm;
}

template <typename TTuple, typename TArgs>
std::ostream &operator<<(std::ostream &strm, const TFormat<TTuple, TArgs> &) {
  static_assert(
      TFalse<TTuple, TArgs>::value,
      "The result of format() must be used immediately within the expression.");
  return strm;
}

template <typename TChars>
class TFormatStr {
  public:

  template <typename... TArgs>
  [[clang::warn_unused_result]]
  auto Format(TArgs &&...args) const {
    return TFormat<TTuple, std::tuple<TArgs &&...>>(
        std::forward_as_tuple(std::forward<TArgs>(args)...));
  }

  private:

  struct TTuple {
    static constexpr auto Get() { return Parse(TChars()); }
  };  // TTuple

};  // TFormatStr

template <typename TChars>
auto MakeFormatStr(TChars) {
  return TFormatStr<TChars>();
}

