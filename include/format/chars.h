/* TODO */

#pragma once

#include <utility>

#define STR(str_literal)                                            \
  []() {                                                            \
    struct {                                                        \
      static constexpr decltype(auto) Get() {                       \
        static_cast<const char *>(str_literal);                     \
        return str_literal;                                         \
      }                                                             \
      static constexpr auto GetSize() { return sizeof(Get()) - 1; } \
    } str;                                                          \
    return str;                                                     \
  }()
