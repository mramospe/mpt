#include "mpt/indexed_container.hpp"
#include "test_utils.hpp"
#include <array>
#include <numeric>
#include <vector>

template <class ContainerType> auto fill_container(ContainerType &cont) {
  auto proxy = mpt::make_indexed_container_proxy(cont);
  std::iota(proxy.begin(), proxy.end(), 0ul);
}

template <class ContainerType> auto test_container(ContainerType const &cont) {

  mpt::test::errors errors;

  auto proxy = mpt::make_indexed_container_proxy(cont);

  // forward iteration
  auto test_forward = [&errors, &cont](auto begin, auto end) {
    for (auto it = begin; it != end; ++it) {
      if (*it != cont[std::distance(begin, it)]) {
        errors.push_back(
            "Mismatch of values in container when iterating forward");
        break;
      }
    }
  };

  test_forward(proxy.begin(), proxy.end());
  test_forward(proxy.cbegin(), proxy.cend());

  // backward iteration
  auto test_backward = [&errors, &cont](auto rbegin, auto rend) {
    for (auto it = rbegin; it != rend; ++it) {
      if (*it != cont[std::distance(it, rend) - 1ul]) {
        errors.push_back(
            "Mismatch of values in container when iterating backward");
        break;
      }
    }
  };

  test_backward(proxy.rbegin(), proxy.rend());
  test_backward(proxy.crbegin(), proxy.crend());

  auto itf = std::next(proxy.begin(), 4);
  if (itf != proxy.begin() + 4)
    errors.push_back("Problems shifting the iterator forward");

  auto itb = std::prev(proxy.end(), 4);
  if (itb != proxy.end() - 4)
    errors.push_back("Problems shifting the iterator backward");

  return errors;
}

auto test_array() {
  std::array<int, 10> arr;
  fill_container(arr);
  return test_container(arr);
}

auto test_vector() {
  std::vector<int> vec(10);
  fill_container(vec);
  return test_container(vec);
}

int main() {

  mpt::test::collector indexed_container("indexed_container");
  MPT_TEST_UTILS_ADD_TEST(indexed_container, test_array);
  MPT_TEST_UTILS_ADD_TEST(indexed_container, test_vector);

  return mpt::test::to_return_code(indexed_container.run());
}
