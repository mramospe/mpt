/*!\file

  Utilities to work with signatures of (member) functions and functors.
 */
#pragma once
#include "mpt/types.hpp"
#include <functional>
#include <tuple>

namespace mpt {

#ifndef MPT_DOXYGEN_WARD
  template <class Function> struct function_signature;
#endif

  /// Represent the signature of a function
  template <class Output, class... Input>
  struct function_signature<Output(Input...)> {
    using output_t = Output;
    using input_t = std::tuple<Input...>;
  };

#ifndef MPT_DOXYGEN_WARD
  template <class MemberFunction> struct member_function_signature;
#endif

  /// Represent the signature of a member function
  template <class Object, class Output, class... Input>
  struct member_function_signature<Output(Object &, Input...)> {
    using output_t = Output;
    using object_t = Object;
    using input_t = std::tuple<Input...>;
  };

  /// Determine the input type at the given position
  template <std::size_t I, class Signature> struct signature_input_at {
    using type =
        mpt::templated_object_type_at_t<I, typename Signature::input_t>;
  };

  /// Type of the input type at the given position
  template <std::size_t I, class Signature>
  using signature_input_at_t = typename signature_input_at<I, Signature>::type;

  /// Determine the output type at the given position
  template <std::size_t I, class Signature> struct signature_output {
    using type =
        mpt::templated_object_type_at_t<I, typename Signature::output_t>;
  };

  /// Type of the output type at the given position
  template <std::size_t I, class Signature>
  using signature_output_t = typename signature_output<I, Signature>::type;

  /*!\brief Determine the signature of a callable

    By default it is assumed that the template argument is a functor with
    a single version of operator(). The other specializations cover cases
    where the argument is a (non-)const member function or a global
    function.
   */
  template <class Callable> struct callable_signature {
    using type =
        typename callable_signature<decltype(&Callable::operator())>::type;
  };

#ifndef MPT_DOXYGEN_WARD
  template <class Output, class... Input>
  struct callable_signature<Output (*)(Input...)> {
    using type = function_signature<Output(Input...)>;
  };

  template <class Output, class Object, class... Input>
  struct callable_signature<Output (Object::*)(Input...) const> {
    using type = member_function_signature<Output(Object const &, Input...)>;
  };

  template <class Output, class Object, class... Input>
  struct callable_signature<Output (Object::*)(Input...)> {
    using type = member_function_signature<Output(Object &, Input...)>;
  };
#endif

  /// Determine the signature of a callable
  template <class Callable>
  using callable_signature_t = typename callable_signature<Callable>::type;

  /// Checks if the given signature is that of a function
  template <class Signature> struct is_function_signature : std::false_type {};

#ifndef MPT_DOXYGEN_WARD
  template <class Output, class... Input>
  struct is_function_signature<function_signature<Output(Input...)>>
      : std::true_type {};
#endif

  /// Whether the given signature is that of a function
  template <class Signature>
  static constexpr auto is_function_signature_v =
      is_function_signature<Signature>::value;

  /// Checks if the given signature is that of a const member function
  template <class Signature>
  struct is_const_member_function_signature : std::false_type {};

#ifndef MPT_DOXYGEN_WARD
  template <class Object, class Output, class... Input>
  struct is_const_member_function_signature<
      member_function_signature<Output(Object &, Input...)>>
      : std::conditional_t<std::is_const_v<Object>, std::true_type,
                           std::false_type> {};
#endif

  /// Whether the given signature is that of a const member function
  template <class Signature>
  static constexpr auto is_const_member_function_signature_v =
      is_const_member_function_signature<Signature>::value;

  /// Checks if the given signature is that of a non-const member function
  template <class Signature>
  struct is_nonconst_member_function_signature : std::false_type {};

#ifndef MPT_DOXYGEN_WARD
  template <class Object, class Output, class... Input>
  struct is_nonconst_member_function_signature<
      member_function_signature<Output(Object &, Input...)>>
      : std::conditional_t<!std::is_const_v<Object>, std::true_type,
                           std::false_type> {};
#endif

  /// Whether the given signature is that of a non-const member function
  template <class Signature>
  static constexpr auto is_nonconst_member_function_signature_v =
      is_nonconst_member_function_signature<Signature>::value;

  /// Type wrapper that checks if the given callable is a function
  template <class Callable>
  struct is_function : is_function_signature<callable_signature_t<Callable>> {};

  /// Whether the given callable is a function
  template <class Callable>
  static constexpr auto is_function_v = is_function<Callable>::value;

  /// Type wrapper that checks if the given callable is a const member function
  template <class Callable>
  struct is_const_member_function
      : is_const_member_function_signature<callable_signature_t<Callable>> {};

  /// Whether the given callable is a const member function
  template <class Callable>
  static constexpr auto is_const_member_function_v =
      is_const_member_function<Callable>::value;

  /// Determine the function pointer for a given signature
  template <class Signature> struct function_pointer_type;

  /// Determine the function pointer type for a given signature
  template <class Output, class... Input>
  struct function_pointer_type<function_signature<Output(Input...)>> {
    using type = Output (*)(Input...);
  };

  /// Determine the function pointer type for a given signature
  template <class Object, class Output, class... Input>
  struct function_pointer_type<
      member_function_signature<Output(Object &, Input...)>> {
    using type = std::conditional_t<std::is_const_v<Object>,
                                    Output (Object::*)(Input...) const,
                                    Output (Object::*)(Input...)>;
  };

  /// Function pointer type for a given signature
  template <class Signature>
  using function_pointer_type_t =
      typename function_pointer_type<Signature>::type;

  /// Checks if the given callable is a non-const member function
  template <class Callable>
  struct is_nonconst_member_function
      : is_nonconst_member_function_signature<callable_signature_t<Callable>> {
  };

  /// Whether the given callable is a non-const member function
  template <class Callable>
  static constexpr auto is_nonconst_member_function_v =
      is_nonconst_member_function<Callable>::value;

  namespace {

    /// Determine the STL function wrapper for a given signature
    template <class Signature> struct stl_function_wrapper_from_signature;

    template <class Output, class... Input>
    struct stl_function_wrapper_from_signature<
        function_signature<Output(Input...)>> {
      using type = std::function<Output(Input...)>;
    };

    template <class Object, class Output, class... Input>
    struct stl_function_wrapper_from_signature<
        member_function_signature<Output(Object &, Input...)>> {
      using type = std::function<Output(Object const &, Input...)>;
    };
  } // namespace

  /// Determine the std::function wrapper associated to the given callable
  template <class Callable> struct stl_function_wrapper {
    using type = typename stl_function_wrapper_from_signature<
        callable_signature_t<Callable>>::type;
  };

  /// Determine the std::function wrapper associated to the given callable
  template <class Callable>
  using stl_function_wrapper_t = typename stl_function_wrapper<Callable>::type;
} // namespace mpt
