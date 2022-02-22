#pragma once
#include "mpt/signature.hpp"

namespace mpt::members {

  /// Wrapper around either a function or a member function
  template <class Signature> struct function_wrapper {
    using signature_type = Signature;
    mpt::signature::function_pointer_type_t<Signature> function_pointer;
  };

  /// Make a wrapper around a function
  template <class Output, class... Input>
  constexpr auto make_function_wrapper(Output (*f)(Input...)) {
    return function_wrapper<
        mpt::signature::function_signature<Output(Input...)>>(f);
  }

  /// Make a wrapper around a non-const member function
  template <class Object, class Output, class... Input>
  constexpr auto make_function_wrapper(Output (Object::*f)(Input...)) {
    return function_wrapper<
        mpt::signature::member_function_signature<Output(Object &, Input...)>>(
        f);
  }

  /// Make a wrapper around a const member function
  template <class Object, class Output, class... Input>
  constexpr auto make_function_wrapper(Output (Object::*f)(Input...) const) {
    return function_wrapper<mpt::signature::member_function_signature<Output(
        Object const &, Input...)>>(f);
  }

  /// Validator to check that a member function has the given signature
  template <class Signature, mpt::signature::function_pointer_type_t<Signature>>
  struct member_function_validator {};

  /// Validator for class members
  template <class Object, class Type, Type Object::*>
  struct member_validator {};

  /// Check if an object has a specific member defined
  template <class Object, class Checker> struct has_member {
    template <class T>
    static constexpr std::true_type __check(typename Checker::validator<T> *);
    template <class T> static constexpr std::false_type __check(...);
    static constexpr auto value = decltype(__check<Object>(0))::value;
  };

  /// Check if an object has a specific member function defined
  template <class Object, class Checker>
  static constexpr auto has_member_v = has_member<Object, Checker>::value;
} // namespace mpt::members
