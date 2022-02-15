#include "mpt/smart_enum.hpp"
#include "test_utils.hpp"

MPT_SMART_ENUM(simple, simple_properties, int, simple_unknown, simple_A,
               simple_B);

template <class EnumType> mpt::test::errors test_properties_for_enum() {

  mpt::test::errors errors;

  using properties = mpt::smart_enum::properties_t<EnumType>;

  if (properties::size + 1 != properties::size_with_unknown)
    errors.push_back(
        "Difference of sizes with and without unknown is greater than one");

  return errors;
}

/// Test that the properties for an enumeration type are well-defined
mpt::test::errors test_properties() {
  mpt::test::errors errors;
  auto simple_errors = test_properties_for_enum<simple>();
  std::copy(std::make_move_iterator(simple_errors.begin()),
            std::make_move_iterator(simple_errors.end()),
            std::back_inserter(errors));
  return errors;
}

/// Test that argument-dependent lookup functions work correctly
mpt::test::errors test_adl() {

  mpt::test::errors errors;

  if (to_string(simple_A) != "simple_A" || to_string(simple_B) != "simple_B")
    errors.push_back("Unable to convert enumeration types to strings");

  if (mpt::smart_enum::from_string<simple>("simple_A") != simple_A ||
      mpt::smart_enum::from_string<simple>("simple_B") != simple_B)
    errors.push_back("Unable to create enumeration types from strings");

  return errors;
}

template <class EnumType> mpt::test::errors test_unknown_for_enum() {

  mpt::test::errors errors;

  using properties = mpt::smart_enum::properties_t<EnumType>;

  if (mpt::smart_enum::from_string<EnumType>("__protected__") !=
      properties::unknown_value)
    errors.push_back("String didn't evaluate to unknown");

  if (!mpt::smart_enum::is_unknown(
          mpt::smart_enum::from_string<EnumType>("__protected__")))
    errors.push_back("String didn't evaluate to unknown");

  try {
    mpt::smart_enum::from_string_throw_if_unknown<EnumType>("__protected__");
    errors.push_back("Should have raised an error when trying to convert from "
                     "an unknown string");
  } catch (...) {
  }

  return errors;
}

/// Test the handling of unknown values
mpt::test::errors test_unknown() {
  mpt::test::errors errors;
  auto simple_errors = test_unknown_for_enum<simple>();
  std::copy(std::make_move_iterator(simple_errors.begin()),
            std::make_move_iterator(simple_errors.end()),
            std::back_inserter(errors));
  return errors;
}

// Helpers for "test_switch"
template <simple E> struct functor;

template <> struct functor<simple_unknown> {
  constexpr bool operator()(bool b) const { return b; }
};

template <> struct functor<simple_A> {
  constexpr bool operator()(bool b) const { return b; }
};

template <> struct functor<simple_B> {
  constexpr bool operator()(bool b) const { return b; }
};

/// Test the switch function
mpt::test::errors test_switch() {
  mpt::test::errors errors;
  for (auto e : mpt::smart_enum::properties_t<simple>::values_with_unknown)
    if (mpt::smart_enum::apply_with_switch<simple, functor>(e, false))
      errors.push_back("Unable to use switch for \"" + to_string(e) + '"');
  return errors;
}

int main() {

  mpt::test::collector smart_enums("smart enumerations");
  MPT_TEST_UTILS_ADD_TEST(smart_enums, test_properties);
  MPT_TEST_UTILS_ADD_TEST(smart_enums, test_adl);
  MPT_TEST_UTILS_ADD_TEST(smart_enums, test_unknown);
  MPT_TEST_UTILS_ADD_TEST(smart_enums, test_switch);

  return mpt::test::to_return_code(smart_enums.run());
}
