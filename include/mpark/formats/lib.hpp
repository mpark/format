// MPark.Format
//
// Copyright Michael Park, 2014-2017
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MPARK_FORMATS_LIB_HPP
#define MPARK_FORMATS_LIB_HPP

#include <cstddef>
#include <ostream>

namespace mpark {
  namespace formats {
    namespace lib {

      template <typename T>
      struct identity { using type = T; };

#define CONSTANT(...)                                     \
  struct {                                                \
    static constexpr auto value() { return __VA_ARGS__; } \
  };

      template <std::size_t I>
      using size_constant = std::integral_constant<std::size_t, I>;

      template <typename...>
      constexpr bool false_v = false;

      inline namespace cpp17 {

        template <bool B>
        using bool_constant = std::integral_constant<bool, B>;

        class string_view {
          public:
          constexpr string_view(const char *str, std::size_t size)
              : str_(str), size_(size) {}

          constexpr const char *begin() const noexcept { return str_; }
          constexpr const char *end() const noexcept { return str_ + size_; }

          constexpr const char *data() const noexcept { return str_; }
          constexpr std::size_t size() const noexcept { return size_; }

          private:
          const char *str_;
          std::size_t size_;

          friend std::ostream &operator<<(std::ostream &strm,
                                          const string_view &that) {
            return strm.write(that.str_, that.size_);
          }
        };

      }  // namespace cpp17

    }  // namespace lib
  }  // namespace formats
}  // namespace mpark

#endif  // MPARK_FORMATS_LIB_HPP
