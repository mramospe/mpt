#include <utility>

namespace mpt {

  /// Add the given number to the values of a sequence of indices
  template <std::size_t N, std::size_t... I>
  constexpr auto add_to_index_sequence(std::index_sequence<I...>) {
    return std::index_sequence<N + I...>{};
  }

  /// Make a std::index_sequence object given a start and a stop value
  template <std::size_t Start, std::size_t Stop>
  constexpr auto make_index_sequence_with_bounds() {
    static_assert(Stop >= Start,
                  "Stop value must be greater or equal to the start");
    return add_to_index_sequence<Start>(
        std::make_index_sequence<Stop - Start>());
  }
} // namespace mpt
