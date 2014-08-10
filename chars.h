/* TODO */

#pragma once

#include <utility>

#define MAKE_CHARS(str_literal)                                     \
  []() {                                                            \
    struct TStrLiteral {                                            \
      static constexpr decltype(auto) Get() {                       \
        static_cast<const char *>(str_literal);                     \
        return str_literal;                                         \
      }                                                             \
      static constexpr auto GetSize() { return sizeof(Get()) - 1; } \
    };                                                              \
    return MakeChars(TStrLiteral());                                \
  }()

template <char... Cs>
class TChars {
  public:

  static constexpr const char *Get() { return StrLiteral; }

  static constexpr auto GetSize() { return sizeof...(Cs); }

  private:

  static constexpr const char StrLiteral[] = {Cs..., '\0'};

};  // TChars

template <char... Cs>
constexpr const char TChars<Cs...>::StrLiteral[];

template <typename TStrLiteral, std::size_t... Is>
constexpr auto MakeCharsImpl(TStrLiteral str_literal,
                             std::index_sequence<Is...>) {
  return TChars<str_literal.Get()[Is]...>();
}

template <typename TStrLiteral>
constexpr auto MakeChars(TStrLiteral str_literal) {
  return MakeCharsImpl(str_literal,
                       std::make_index_sequence<str_literal.GetSize()>());
}

