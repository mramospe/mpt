#include "mpt/multi_index.hpp"
#include "test_utils.hpp"
#include <array>
#include <numeric>
#include <optional>
#include <sstream>
#include <string_view>
#include <type_traits>

using size_type = std::size_t;

static constexpr std::array<size_type, 3ul> reference[] = {
    {0, 0, 0}, {0, 0, 1}, {0, 1, 0}, {0, 1, 1}, {0, 2, 0}, {0, 2, 1},
    {1, 0, 0}, {1, 0, 1}, {1, 1, 0}, {1, 1, 1}, {1, 2, 0}, {1, 2, 1},
};

template <class SizeContainerType> auto make_multi_index_for_reference() {
  return mpt::multi_index<SizeContainerType>(2ul, 3ul, 2ul);
}

template <class TargetContainer, class ReferenceContainer>
std::optional<std::string>
compare_with_reference(size_type index, TargetContainer const &tgt,
                       ReferenceContainer const &ref,
                       std::string_view context = "") {

  std::string msg_prefix =
      context.empty() ? std::string{} : "(" + std::string(context) + ") ";

  if (tgt.size() != ref.size())
    return std::make_optional<std::string>(
        msg_prefix + "Wrong size of indices " + std::to_string(tgt.size()) +
        "; should be " + std::to_string(ref.size()));

  for (auto i = 0ul; i < tgt.size(); ++i)

    if (tgt[i] != ref[i]) {

      std::stringstream ss;
      ss << msg_prefix
         << "Wrong indices detected for index " + std::to_string(index) + ": ["
         << tgt[0];

      for (auto it = std::next(tgt.begin()); it != tgt.end(); ++it)
        ss << ", " << *it;

      ss << "] (reference=[" << ref[0];

      for (auto it = std::next(ref.begin()); it != ref.end(); ++it)
        ss << ", " << *it;

      ss << "])";

      return std::make_optional<std::string>(ss.str());
    }

  return std::nullopt;
}

template <class SizeContainerType> mpt::test::errors test_container() {

  mpt::test::errors errors;

  auto mi = make_multi_index_for_reference<SizeContainerType>();

  if (mi.size() != std::size(reference)) {
    errors.push_back("Wrong size of multi-index");
    return errors;
  }

  for (auto i = 0ul; i < mi.size(); ++i) {

    auto tgt = mi[i];
    auto const &ref = reference[i];

    // plain comparison by groups of indices
    auto error_opt = compare_with_reference(i, tgt, ref);
    if (error_opt)
      errors.emplace_back(std::move(error_opt.value()));

    // test the unraveling of the indices
    for (auto j = 0ul; j < mi.dim(); ++j)
      if (mi.unravel(j, i) != ref[j]) {
        errors.emplace_back("Problem unraveling values in index " +
                            std::to_string(i));
        break;
      }
  }

  return errors;
}

auto test_array() {
  static_assert(std::is_same_v<decltype(mpt::make_multi_index_array(2ul, 3ul)),
                               mpt::multi_index<std::array<size_type, 2>>>,
                "Wrong multi-index type");
  return test_container<std::array<size_type, 3ul>>();
}

auto test_vector() {
  static_assert(std::is_same_v<decltype(mpt::make_multi_index_vector(2ul, 3ul)),
                               mpt::multi_index<std::vector<size_type>>>,
                "Wrong multi-index type");
  return test_container<std::vector<size_type>>();
}

auto test_iteration() {

  mpt::test::errors errors;

  auto mi = make_multi_index_for_reference<std::vector<size_type>>();

  auto f = std::accumulate(mi.begin(), mi.end(), 0ul,
                           [](auto sum, auto const &) { return ++sum; });
  if (f != mi.size())
    errors.push_back(
        "Forward iteration size is different from the number of indices");

  size_type forward_counter = 0ul;
  for (auto indices : mi) {
    auto const &ref = reference[forward_counter];
    auto error_opt = compare_with_reference(forward_counter, indices, ref,
                                            "forward iteration");
    if (error_opt) {
      errors.emplace_back(std::move(error_opt.value()));
      break;
    } else
      ++forward_counter;
  }

  auto b = std::accumulate(mi.begin(), mi.end(), 0ul,
                           [](auto sum, auto const &) { return ++sum; });
  if (b != mi.size())
    errors.push_back(
        "Backward iteration size is different from the number of indices");

  size_type backward_counter = mi.size() - size_type(1);
  for (auto it = mi.rbegin(); it != mi.rend(); ++it) {
    auto const &ref = reference[backward_counter];
    auto error_opt = compare_with_reference(backward_counter, *it, ref,
                                            "backward iteration");
    if (error_opt) {
      errors.emplace_back(std::move(error_opt.value()));
      break;
    } else
      --backward_counter;
  }

  return errors;
}

int main() {

  mpt::test::collector multi_index("multi_index");
  MPT_TEST_UTILS_ADD_TEST(multi_index, test_array);
  MPT_TEST_UTILS_ADD_TEST(multi_index, test_vector);
  MPT_TEST_UTILS_ADD_TEST(multi_index, test_iteration);

  return mpt::test::to_return_code(multi_index.run());
}
