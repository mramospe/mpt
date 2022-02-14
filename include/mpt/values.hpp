#pragma once
#include <cstdlib>

namespace mpt {

  /// Wrap a value to be used as a constant expression
  template <class T> struct value_wrapper {
    using value_type = T;
    value_type value;
  };

  /// Get the value at the given position
  template <std::size_t I, class T0, class... T>
  constexpr auto const &value_at(T0 const &v0, const T &...v) {
    if constexpr (I == 0)
      return v0;
    else
      return value_at<I - 1>(v...);
  }

  /// Get the value at the given position
  template <std::size_t I, class T0, class... T>
  constexpr auto &value_at(T0 &&v0, T &&...v) {
    if constexpr (I == 0)
      return v0;
    else
      return value_at<I - 1>(v...);
  }
} // namespace mpt
