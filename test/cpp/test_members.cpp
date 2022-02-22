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

// Example of an object that checks if the member function "size" is defined
struct size_checker {

  template <class T>
  using signature =
      mpt::signature::member_function_signature<std::size_t(T const &)>;

  template <class T>
  using validator = mpt::members::member_validator<signature<T>, &T::size>;
};

int main() {

  mpt::members::make_function_wrapper(&function);
  mpt::members::make_function_wrapper(&object::nonconst_call);
  mpt::members::make_function_wrapper(&object::const_call);

  static_assert(mpt::members::has_member_v<object_with_size, size_checker>,
                "Object has a member function called \"size\" but "
                "\"has_member\" evaluates to false");
  static_assert(
      !mpt::members::has_member_v<object_with_nonconst_size, size_checker>,
      "Object has a member function called \"size\" but is non-const and "
      "\"has_member\" evaluates to true");
  static_assert(!mpt::members::has_member_v<object_without_size, size_checker>,
                "Object does not have a member function called \"size\" but "
                "\"has_member\" evaluates to true");

  return 0;
}
