#include "mpt/signature.hpp"
#include "test_utils.hpp"
#include <tuple>

// definitions for checks on functions
using function_example_input_0 = int;
using function_example_input_1 = float;
using function_example_output = std::tuple<int, float>;

function_example_output function_example(function_example_input_0,
                                         function_example_input_1) {
  return {};
}

// definitions for checks on functors and classes
using functor_example_input_0 = double const &;
using functor_example_input_1 = int &;
using functor_example_output = int *;

struct functor_example {
  constexpr functor_example_output operator()(functor_example_input_0,
                                              functor_example_input_1) const {
    return nullptr;
  }
  constexpr void const_call() const {}
  constexpr void nonconst_call() {}
};

int main() {

  mpt::test::collector signature("signature");

  // check function signatures
  using function_signature =
      mpt::callable_signature_t<decltype(&function_example)>;
  static_assert(mpt::is_function_v<decltype(&function_example)>,
                "Failed to check whether a callable is a function or not");
  static_assert(std::is_same_v<typename function_signature::output_t,
                               function_example_output>,
                "Output type has been wrongly determined");
  static_assert(
      std::is_same_v<mpt::signature_input_at_t<0, function_signature>,
                     function_example_input_0> &&
          std::is_same_v<mpt::signature_input_at_t<1, function_signature>,
                         function_example_input_1>,
      "Input types have been wrongly determined");

  // check member function signatures
  static_assert(
      mpt::is_const_member_function_v<decltype(&functor_example::const_call)>,
      "Failed to check whether a callable is a const member function or not");
  static_assert(mpt::is_nonconst_member_function_v<decltype(
                    &functor_example::nonconst_call)>,
                "Failed to check whether a callable is a non-const member "
                "function or not");

  // check functor signatures
  using functor_signature = mpt::callable_signature_t<functor_example>;
  static_assert(std::is_same_v<typename functor_signature::output_t,
                               functor_example_output>,
                "Unable to determine function signatures correctly");
  static_assert(
      std::is_same_v<mpt::signature_input_at_t<0, functor_signature>,
                     functor_example_input_0> &&
          std::is_same_v<mpt::signature_input_at_t<1, functor_signature>,
                         functor_example_input_1>,
      "Input types have been wrongly determined");

  return mpt::test::to_return_code(signature.run());
}
