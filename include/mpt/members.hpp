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
        mpt::signature::function_signature<Output, Input...>>(f);
  }

  /// Make a wrapper around a non-const member function
  template <class Object, class Output, class... Input>
  constexpr auto make_function_wrapper(Output (Object::*f)(Input...)) {
    return function_wrapper<
        mpt::signature::member_function_signature<Object, Output, Input...>>(f);
  }

  /// Make a wrapper around a const member function
  template <class Object, class Output, class... Input>
  constexpr auto make_function_wrapper(Output (Object::*f)(Input...) const) {
    return function_wrapper<mpt::signature::member_function_signature<
        Object const, Output, Input...>>(f);
  }

  /// The validator is only defined is the pointer to the member function has
  /// the correct signature
  template <class Signature, mpt::signature::function_pointer_type_t<Signature>>
  struct member_validator {};

  /// Check if an object has a specific member function defined
  template <class Object, class Checker> struct has_member {
    template <class T>
    static constexpr std::true_type check(typename Checker::validator<T> *);
    template <class T> static constexpr std::false_type check(...);
    static constexpr auto value = decltype(check<Object>(0))::value;
  };

  /// Check if an object has a specific member function defined
  template <class Object, class Checker>
  static constexpr auto has_member_v = has_member<Object, Checker>::value;
} // namespace mpt::members
