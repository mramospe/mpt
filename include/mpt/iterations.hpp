#include <utility>

namespace mpt {

  template <std::size_t N, std::size_t... I>
  constexpr auto add_to_index_sequence(std::index_sequence<I...>) {
    return std::index_sequence<N + I...>{};
  }

  template <std::size_t Start, std::size_t Stop>
  constexpr auto make_range_sequence() {
    return add_to_index_sequence<Start>(
        std::make_index_sequence<Stop - Start>());
  }
} // namespace mpt
