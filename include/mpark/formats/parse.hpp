// MPark.Format
//
// Copyright Michael Park, 2014-2017
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MPARK_FORMATS_PARSE_HPP
#define MPARK_FORMATS_PARSE_HPP

#include <cstddef>
#include <tuple>
#include <type_traits>

#include <mpark/formats/lib.hpp>

namespace mpark {
  namespace formats {

    namespace detail {

      constexpr bool is_double_open(const char *text, std::size_t size) {
        return *text == '{' && size > 1 && *(text + 1) == '{';
      }

      constexpr bool is_double_close(const char *text, std::size_t size) {
        return *text == '}' && size > 1 && *(text + 1) == '}';
      }

      constexpr bool is_open(const char *text, std::size_t size) {
        return *text == '{' && !is_double_open(text, size);
      }

      constexpr bool is_close(const char *text) { return *text == '}'; }

      constexpr auto convert_impl(lib::string_view sv, std::true_type) {
        std::size_t idx = 0;
        for (std::size_t i = 1; i < sv.size() - 1; ++i) {
          idx *= 10;
          idx += sv.data()[i] - '0';
        }  // for
        return idx;
      }

      constexpr auto convert_impl(lib::string_view sv, std::false_type) {
        return sv;
      }

      template <typename StrView>
      constexpr auto convert(StrView) {
        constexpr auto sv = StrView::value();
        constexpr auto data = sv.data();
        constexpr auto size = sv.size();
        return convert_impl(sv,
                            lib::bool_constant<(is_open(data, size) &&
                                                is_close(data + size - 1))>{});
      }

      enum class State { Success, SingleOpen, SingleClose };

      template <State state, typename Defer = void>
      struct Report;

      template <typename Defer>
      struct Report<State::Success, Defer> {};

      template <typename Defer>
      struct Report<State::SingleOpen, Defer> {
        static_assert(lib::false_v<Defer>,
                      "Single '{' encountered in format string.");
      };

      template <typename Defer>
      struct Report<State::SingleClose, Defer> {
        static_assert(lib::false_v<Defer>,
                      "Single '}' encountered in format string.");
      };

      constexpr auto get_state(const char *text, std::size_t size) {
        struct Result {
          State state;
          const char *end;
          const char *next;
        };  // Result
        std::size_t i = 0;
        if (is_open(text + i, size - i)) {
          while (i < size && !is_close(text + i)) {
            ++i;
          }  // while
          if (i >= size) {
            return Result{State::SingleOpen, nullptr, nullptr};
          }  // if
          ++i;
        } else {
          while (i < size && !is_open(text + i, size - i)) {
            if (is_double_open(text + i, size - i) ||
                is_double_close(text + i, size - i)) {
              return Result{State::Success, text + i + 1, text + i + 2};
            }  // if
            if (is_close(text + i)) {
              return Result{State::SingleClose, nullptr, nullptr};
            }  // if
            ++i;
          }  // while
        }  // if
        return Result{State::Success, text + i, text + i};
      }

    }  // namespace detail

    template <typename End>
    constexpr auto parse_impl(End, lib::size_constant<0>) {
      return std::make_tuple();
    }

    template <typename Cur, std::size_t Size>
    constexpr auto parse_impl(Cur, lib::size_constant<Size>) {
      using State = CONSTANT(detail::get_state(Cur::value(), Size));
      static_cast<void>(detail::Report<State::value().state>{});
      using Token = CONSTANT(
          lib::string_view(Cur::value(), State::value().end - Cur::value()));
      using Next = CONSTANT(State::value().next);
      constexpr std::size_t size = Next::value() - Cur::value();
      return std::tuple_cat(
          std::make_tuple(detail::convert(Token{})),
          parse_impl(Next{}, lib::size_constant<Size - size>{}));
    }

    template <typename StrView>
    constexpr auto parse(StrView) {
      using Data = CONSTANT(StrView::value().data());
      return parse_impl(Data{}, lib::size_constant<StrView::value().size()>{});
    }

  }  // namespace formats
}  // namespace mpark

#endif  // MPARK_FORMATS_PARSE_HPP
