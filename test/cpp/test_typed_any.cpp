#include "mpt/typed_any.hpp"
#include "test_utils.hpp"
#include <type_traits>

// Test the construction of typed "any" objects
mpt::test::errors test_simple() {

  mpt::test::errors errors;

  mpt::typed_any<int, float, double> a = 1.f;

  if (a.type_index() != 1)
    errors.push_back("Wrong determination of the type index");

  a = 1;

  if (a.type_index() != 0)
    errors.push_back(
        "Wrong determination of the type index after re-assigning the object");

  return errors;
}

// Test the evaluation of functions taking typed "any" objects as a input
mpt::test::errors test_visit() {

  mpt::test::errors errors;

  mpt::typed_any<int, float, double> a = 1.f;

  mpt::visit_typed_any(
      [&errors](auto v) {
        if constexpr (!std::is_same_v<decltype(v), float>)
          errors.push_back("Wrong determination of the type during the visit");
      },
      a);

  // set a new value
  int i = 1;

  a = i;

  mpt::visit_typed_any(
      [&errors](auto v) {
        if constexpr (!std::is_same_v<decltype(v), int>)
          errors.push_back("Wrong determination of the type during the visit "
                           "after re-assigning the object");
      },
      a);

  mpt::visit_typed_any(
      [&errors, &i](auto v) {
        if (v != i) {
          errors.push_back("Failed to change the value type");
        }
      },
      a);

  // if all types are convertible to the same type this must work
  int j = mpt::visit_typed_any([](auto v) -> int { return v; }, a);

  if (j != i)
    errors.push_back("Wrong value returned by mpt::visit_typed_any");

  // check the accessor for each qualification of "a"
  mpt::visit_typed_any([](auto const &) {}, a);
  mpt::visit_typed_any([](auto const &) {}, std::as_const(a));
  mpt::visit_typed_any([](auto &&) {}, std::move(a));
  // "a" becomes invalid after this point

  return errors;
}

int main() {

  mpt::test::collector typed_any("typed_any");
  MPT_TEST_UTILS_ADD_TEST(typed_any, test_simple);
  MPT_TEST_UTILS_ADD_TEST(typed_any, test_visit);

  return mpt::test::to_return_code(typed_any.run());
}
