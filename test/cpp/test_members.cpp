#include "mpt/members.hpp"

int function(double) { return 0; }

struct object {
  auto const_call() const {}
  auto nonconst_call() {}
};

struct object_with_size {
  std::size_t size() const { return 0u; }
};

struct object_with_nonconst_size {
  std::size_t size() { return 0u; }
};

struct object_without_size {};

struct object_with_value {
  float value;
};

struct object_with_value_const {
  float const value;
};

struct object_with_value_wrong_type {
  int value;
};

struct object_without_value {};

// Example of an object that checks if the member function "size" is defined
struct size_checker {

  template <class T>
  using signature = mpt::member_function_signature<std::size_t(T const &)>;

  template <class T>
  using validator = mpt::member_function_validator<signature<T>, &T::size>;
};

// Example of an object that checks if the member "value" is defined
struct value_checker {
  template <class T>
  using validator = mpt::member_validator<T, float, &T::value>;
};

int main() {

  // check function wrappers
  mpt::make_function_wrapper(&function);
  mpt::make_function_wrapper(&object::nonconst_call);
  mpt::make_function_wrapper(&object::const_call);

  // check members
  static_assert(mpt::has_member_v<object_with_value, value_checker>,
                "Object has a member called \"value\" but \"has_member_v\" "
                "evaluates to false");
  static_assert(
      !mpt::has_member_v<object_with_value_const, value_checker>,
      "Object has a constant member called \"value\" but \"has_member_v\" "
      "evaluates to true");
  static_assert(!mpt::has_member_v<object_with_value_wrong_type, value_checker>,
                "Object has a member called \"value\" with the wrong type but "
                "\"has_member_v\" evaluates to true");
  static_assert(
      !mpt::has_member_v<object_without_value, value_checker>,
      "Object does not have a member called \"value\" but \"has_member_v\" "
      "evaluates to true");

  // check member functions
  static_assert(mpt::has_member_v<object_with_size, size_checker>,
                "Object has a member function called \"size\" but "
                "\"has_member_v\" evaluates to false");
  static_assert(
      !mpt::has_member_v<object_with_nonconst_size, size_checker>,
      "Object has a member function called \"size\" but is non-const and "
      "\"has_member_v\" evaluates to true");
  static_assert(!mpt::has_member_v<object_without_size, size_checker>,
                "Object does not have a member function called \"size\" but "
                "\"has_member_v\" evaluates to true");

  return 0;
}
