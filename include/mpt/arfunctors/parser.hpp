/*!\file
  Utilities to parse arithmetic and relational functors from strings
*/
#pragma once
#include "mpt/arfunctors/core/operators.hpp"
#include "mpt/arfunctors/core/runtime_arfunctor.hpp"
#include <cstring>
#include <string>
#include <type_traits>
#include <variant>

namespace mpt {

  /*!\brief Define the floating-point and integral types to parse strings

    This class simply accepts two template arguments, corresponding to the
    floating-point and integral types meant to be used when parsing strings
    and creating runtime arithmetic and relational functors.
   */
  template <class FloatType = double, class IntegralType = long int>
  struct arfunctor_arithmetic_types {

    static_assert(std::is_floating_point_v<FloatType>);
    static_assert(std::is_integral_v<IntegralType>);

    using float_type = FloatType;
    using integral_type = IntegralType;
  };

  namespace {

    /// Kind of token
    enum class token_kind : int {
      none,
      number,
      function,
      operation,
      left_parenthesis,
      right_parenthesis
    };

    /// A token, which is defined by its kind and the value
    template <class ValueType> struct token {

      using value_type = ValueType;

      token_kind kind;
      value_type value;

      /// Determine if the value is valid or not
      operator bool() const { return kind != token_kind::none; }
    };

#ifndef MPT_DOXYGEN_WARD
    template <class Signature, class ArithmeticTypes, class Functors,
              class Operators>
    class token_state;
#endif

    /// Store the state for reading tokens in a string
    template <class Signature, class ArithmeticTypes, class... Functor,
              class... Operator>
    class token_state<Signature, ArithmeticTypes, types<Functor...>,
                      types<Operator...>> {

    public:
      using token_type =
          token<std::variant<typename ArithmeticTypes::float_type,
                             typename ArithmeticTypes::integral_type,
                             Functor..., Operator...>>;

      /// Build the object from a view of a string
      token_state(std::string_view view) : m_view{view}, m_pos{0u} {}

      /// Parse the next token
      token_type parse_token() {

        // TODO: parse the string, determine the token, define the value of the variant that holds it and return its value.

        // set the new point of view of the string
        m_view = std::string_view(m_view.cbegin() + m_pos, m_view.cend());
        m_pos = 0u;

        return {token_kind::none};
        // return {token_kind::, };
      }

    private:
      /// The string the state works on
      std::string_view m_view;
      /// The current position of the pointer in the overall string
      std::size_t m_pos;
    };

    /// Parse a string and build an arithmetic and relational functor
    template <class Signature, class ArithmeticTypes, class Functors,
              class Operators>
    runtime_arfunctor<Signature> parse_arfunctor_impl(std::string_view view) {

      token_state<Signature, ArithmeticTypes, Functors, Operators> state(view);

      while (auto tk = state.parse_token()) {

        switch (tk.kind) {

        case (token_kind::number):
          break;
        case (token_kind::function):
          break;
        case (token_kind::operation):
          break;
        case (token_kind::left_parenthesis):
          break;
        case (token_kind::right_parenthesis):
          break;
        case (token_kind::none):
          __builtin_unreachable();
        }
      }

      return {};
    }
  } // namespace

#ifndef MPT_DOXYGEN_WARD
  template <class ArithmeticTypes, class Functors, class Operators>
  struct arfunctors_parser;
#endif

  /*!\brief Small class to parse arithmetic and relational functors from strings

    This class allows to define a general type that would allow to process
    any operation (specified as a string) involving a set of functors and
    additional operators (functions) and convert it into a runtime functor.
   */
  template <class ArithmeticTypes, class... Functor, class... Operator>
  struct arfunctors_parser<ArithmeticTypes, types<Functor...>,
                           types<Operator...>> {

    static_assert(!has_repeated_template_arguments_v<Functor...>);
    static_assert(!has_repeated_template_arguments_v<Operator...>);
    static_assert(!has_repeated_template_arguments_v<Functor..., Operator...>);

    using arithmetic_types = ArithmeticTypes;
    using functor_types = types<Functor...>;
    using operator_types = types<Operator...>;

    template <class Signature>
    static runtime_arfunctor<Signature> parse(std::string_view const &input) {
      return parse_arfunctor_impl<Signature, arithmetic_types, functor_types,
                                  operator_types>(input);
    }

    template <class Signature>
    static runtime_arfunctor<Signature> parse(std::string const &input) {
      return parse<Signature>(std::string_view(input));
    }

    template <class Signature>
    static runtime_arfunctor<Signature> parse(const char *input) {
      return parse<Signature>(std::string_view(input));
    }
  };
} // namespace mpt
