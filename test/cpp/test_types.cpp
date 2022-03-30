#include "mpt/types.hpp"
#include "test_utils.hpp"
#include <tuple>
#include <variant>

// This is all evaluated at compile-time
int main() {

  mpt::test::collector types("types");

  static_assert(mpt::has_type_v<float, double, float, int>);
  static_assert(!mpt::has_type_v<long double, double, float, int>);

  static_assert(
      mpt::has_repeated_template_arguments_v<float, double, float, int>);
  static_assert(
      !mpt::has_repeated_template_arguments_v<long double, double, float, int>);

  static_assert(mpt::type_index_v<float, double, float, int> == 1);
  static_assert(mpt::type_index_v<int, double, float, int> == 2);

  static_assert(std::is_same_v<mpt::type_at_t<0, double, float, int>, double>);
  static_assert(std::is_same_v<mpt::type_at_t<2, double, float, int>, int>);

  static_assert(
      std::variant_size_v<
          mpt::specialize_template_t<std::variant, float, double, int>> == 3);

  using variant_type = mpt::specialize_template_avoid_repetitions_t<
      std::variant, float, double, int, float, long double, int>;
  static_assert(std::variant_size_v<variant_type> == 4);

  static_assert(
      std::tuple_size_v<
          mpt::specialize_template_t<std::tuple, float, double, double, int>> ==
      4);

  using tuple_type = mpt::specialize_template_avoid_repetitions_t<
      std::tuple, float, double, int, float, long double, int>;
  static_assert(std::tuple_size_v<tuple_type> == 4);

  static_assert(
      std::tuple_size_v<mpt::rename_template_t<variant_type, std::tuple>> == 4);

  return mpt::test::to_return_code(types.run());
}
