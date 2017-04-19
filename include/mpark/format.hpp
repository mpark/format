// MPark.Format
//
// Copyright Michael Park, 2014-2017
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MPARK_FORMAT_HPP
#define MPARK_FORMAT_HPP

#include <cstddef>
#include <ostream>
#include <sstream>
#include <tuple>
#include <utility>

#include <mpark/formats/lib.hpp>
#include <mpark/formats/parse.hpp>

#define FS(str_literal)                                          \
  [] {                                                           \
    constexpr char s[] = #str_literal;                           \
    static_assert(s[0] == '"' && s[sizeof(s) - 2] == '"',        \
                  "Argument to `FS` must be a string literal."); \
    using StrView = CONSTANT(::mpark::formats::lib::string_view( \
        str_literal, sizeof(str_literal) - 1));                  \
    return StrView{};                                            \
  }()

namespace mpark {

  namespace formats {
    namespace detail {

      template <typename Tuple, typename... Args>
      class Format {
        public:

        explicit constexpr Format(std::tuple<Args &&...> &&args)
            : args_(std::move(args)) {}

        operator std::string() && {
          std::ostringstream strm;
          strm << std::move(*this);
          return strm.str();
        }

        template <std::size_t... Is>
        void write(std::ostream &strm, std::index_sequence<Is...>) && {
          int dummy[] = {(std::move(*this).write_elem(
                              strm,
                              [] {
                                using Elem =
                                    CONSTANT(std::get<Is>(Tuple::value()));
                                return Elem{};
                              }()),
                          0)...};
          static_cast<void>(dummy);
        }

        private:

        template <typename Elem>
        void write_elem(std::ostream &strm, Elem) && {
          std::move(*this).write_elem_impl(
              strm, Elem{}, lib::identity<decltype(Elem::value())>{});
        }

        template <typename Idx>
        void write_elem_impl(std::ostream &strm,
                             Idx,
                             lib::identity<std::size_t>) && {
          static_assert(Idx::value() < sizeof...(Args), "Index out of range.");
          strm << std::get<Idx::value()>(std::move(args_));
        }

        template <typename StrView>
        void write_elem_impl(std::ostream &strm,
                             StrView,
                             lib::identity<lib::string_view>) && {
          strm << StrView::value();
        }

        std::tuple<Args &&...> args_;
      };

      template <typename Tuple, typename... Args>
      auto format(Tuple, Args &&... args) {
        return Format<Tuple, Args...>(
            std::forward_as_tuple(std::forward<Args>(args)...));
      }

      template <typename Tuple, typename... Args>
      std::ostream &operator<<(std::ostream &strm,
                               const Format<Tuple, Args...> &) {
        static_assert(lib::false_v<Tuple, Args...>,
                      "Result of `format` must be used immediately within the "
                      "expression!.");
        return strm;
      }

      template <typename Tuple, typename... Args>
      std::ostream &operator<<(std::ostream &strm,
                               Format<Tuple, Args...> &&that) {
        constexpr auto tuple = Tuple::value();
        std::move(that).write(strm,
                              std::make_index_sequence<
                                  std::tuple_size<decltype(tuple)>::value>{});
        return strm;
      }

    }  // namespace detail
  }  // namespace formats

  template <typename StrView, typename... Args>
  [[clang::warn_unused_result]]
  auto format(StrView, Args &&... args) {
    using Parse = CONSTANT(formats::parse(StrView{}));
    return formats::detail::format(Parse{}, std::forward<Args>(args)...);
  }

}  // namespace mpark

#endif  // MPARK_FORMAT_HPP
