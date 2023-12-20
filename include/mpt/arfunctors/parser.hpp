/*!\file
  Utilities to parse arithmetic and relational functors from strings
*/
#pragma once
#include "mpt/signature.hpp"
#include "mpt/types.hpp"
#include "mpt/arfunctors/core/operators.hpp"
#include "mpt/arfunctors/core/runtime_arfunctor.hpp"
#include <cstring>
#include <map>
#include <queue>
#include <stack>
#include <string>
#include <type_traits>
#include <variant>

namespace mpt::arfunctors {

  template<class ValueType, class ... Args>
  class function_wrapper {
    public:
      using value_type = ValueType;
      virtual std::size_t expected_number_of_arguments() const = 0;
      virtual value_type operator()(Args const& ... args) const = 0;
  };

  namespace {
    template<class Signatures>
    struct all_arfunctor_types;

    template<class ... Signature>
    struct all_arfunctor_types<mpt::types<Signature ...>> {
      using type = mpt::types<runtime_arfunctor<Signature> ...>;
    };

    template<class Signatures>
    using all_arfunctor_types_t = typename all_arfunctor_types<Signatures>::type;

    template<class ValueType, class IndexSequence>
    struct function_type_for_number_of_arguments_impl;

    template<std::size_t, class T>
    struct repeat {
      using type = T;
    };

    template<class ValueType, std::size_t ... I>
    struct function_type_for_number_of_arguments_impl<ValueType, std::index_sequence<I ...>> {
      using type = function_wrapper<ValueType, typename repeat<I, ValueType>::type ...>;
    };
  }

  template<class ValueType, std::size_t N>
  struct function_type_for_number_of_arguments {
    using type = typename function_type_for_number_of_arguments_impl<ValueType, decltype(std::make_index_sequence<N>())>::type;
  };

  template<class ValueType, std::size_t N>
  using function_type_for_number_of_arguments_t = typename function_type_for_number_of_arguments<ValueType, N>::type;

  template<class Function, class ValueType, class ... Args>
  class specialized_function_wrapper : public function_wrapper<ValueType, Args ...> {
    public:
      using base_class = function_wrapper<ValueType, Args ...>;
      using function_type = Function;
      using value_type = ValueType;
      static constexpr auto number_of_arguments = mpt::callable_number_of_input_arguments_v<Function>;
      specialized_function_wrapper() = delete;
      specialized_function_wrapper(Function function) : m_function{std::move(function)} { }
      std::size_t expected_number_of_arguments() const override {
        return number_of_arguments;
      }
      value_type operator()(Args const& ... args) const override {
        if constexpr ( number_of_arguments == sizeof ... (Args) )
          return m_function(args ...);
        else
          throw std::runtime_error("Attempt to call function with the wrong number of arguments");
      }
    private:
      function_type m_function;
  };

  namespace {

    template<class BaseType, class Function>
    struct specialized_function_wrapper_type_for_base;

    template<class ValueType, class Function, class ... Args>
    struct specialized_function_wrapper_type_for_base<function_wrapper<ValueType, Args ...>, Function> {
      using type = specialized_function_wrapper<Function, ValueType, Args ...>;
    };

    template<class ValueType, std::size_t MaximumNumberOfArguments, class Function>
    auto make_specialized_function_pointer(Function&& function) {
      using base_type = function_type_for_number_of_arguments_t<ValueType, MaximumNumberOfArguments>;
      using derived_type = typename specialized_function_wrapper_type_for_base<base_type, Function>::type;
      return static_cast<std::shared_ptr<base_type>>(std::make_shared<derived_type>(std::forward<Function>(function)));
    };
  }

  using arithmetic_types = mpt::types<bool, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double>;

  template<class Signatures, std::size_t MaximumNumberOfArguments=16ul>
  class function_proxy {
    public:
    
    using functor_types = all_arfunctor_types_t<Signatures>;
    using value_type = mpt::specialize_template_t<std::variant, mpt::concatenate_types_t<arithmetic_types, functor_types>>;
    using function_type = function_type_for_number_of_arguments_t<value_type, MaximumNumberOfArguments>;

    static constexpr auto maximum_number_of_arguments = MaximumNumberOfArguments;

    template<class Function>
    function_proxy(Function&& f) : m_function_ptr{make_specialized_function_pointer<value_type, maximum_number_of_arguments>(std::forward<Function>(f))} { }

    auto& operator*() {
      return *m_function_ptr;
    }

    auto const& operator*() const {
      return *m_function_ptr;
    }

    auto* operator->() {
      return m_function_ptr.get();
    }

    auto const* operator->() const {
      return m_function_ptr.get();
    }

    function_proxy() = delete;
    function_proxy(function_proxy&&) = default;
    function_proxy(function_proxy const&) = default;
    function_proxy& operator=(function_proxy&&) = default;
    function_proxy& operator=(function_proxy const&) = default;

    private:
      std::shared_ptr<function_type> m_function_ptr;
  };

  template<class ... Signature>
  using functor_map = std::map<std::string, std::variant<runtime_arfunctor<Signature> ...>>;

  template<class ... Signature>
  using function_map = std::map<std::string, function_proxy<Signature ...>>;

  namespace {

    /// Kind of token
    enum class token_kind : int {
      none,
      number,
      functor,
      function,
      operation,
      comma,
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
    template <class Signature, class Functors, class Operators>
    class token_state;
#endif

    /// Store the state for reading tokens in a string
    template <class Signature, class... Functor, class... Operator>
    class token_state<Signature, mpt::types<Functor...>, mpt::types<Operator...>> {

    public:
      using token_type =
          token<mpt::specialize_template_from_types_t<std::variant, mpt::concatenate_types_t<mpt::types<Functor...>, mpt::types<Operator ...>, arithmetic_types>>>;

      /// Build the object from a view of a string
      token_state(std::string_view view) : m_view{view}, m_pos{0u} {}

      /// Parse the next token
      token_type parse_token() {

        // TODO: parse the string, determine the token, define the value of the variant that holds it and return its value.

        // set the new point of view of the string
        m_view = std::string_view(m_view.cbegin() + m_pos, m_view.cend());
        m_pos = 0u;

        return {token_kind::none, 0.};
        // return {token_kind::, };
      }

    private:
      /// The string the state works on
      std::string_view m_view;
      /// The current position of the pointer in the overall string
      std::size_t m_pos;
    };

    namespace {
      template<class Variant>
      precedence_type determine_precedence(token<Variant> const& tk) {
        return std::visit([](auto&& v) {

          using type = std::decay_t<decltype(v)>;

          if constexpr ( mpt::templated_object_has_type_v<type, all_operators> )
            return type::precedence;
          else
            // there is a function with no parenthesis
            throw std::runtime_error("Missing parenthesis");

        }, tk.value);
      }
    }

    /// Parse a string and build an arithmetic and relational functor
    template <class Signature, class Operators, class ... S>
    runtime_arfunctor<Signature> parse_impl(std::string_view view, functor_map<S ...> const& functors, function_map<S ...> const& functions) {

      using functor_types = mpt::types<runtime_arfunctor<S> ...>;

      using token_state_type = token_state<Signature, functor_types, Operators>;
      using token_type = typename token_state_type::token_type;
      using operator_or_function_var = mpt::specialize_template_from_types<std::variant, Operators>;

      std::queue<token_type> output_queue;
      std::stack<operator_or_function_var> operator_stack;

      token_state_type state(view);

      while (auto tk = state.parse_token()) {

        switch (tk.kind) {
        case (token_kind::number):
          output_queue.push_back(std::move(tk));
          break;
        case (token_kind::functor):
          output_queue.push_back(std::move(tk));
          break;
        case (token_kind::function):
          operator_stack.emplace(std::move(tk));
          break;
        case (token_kind::operation):
          while ( !operator_stack.empty() && operator_stack.top().kind != token_kind::left_parenthesis && determine_precedence(operator_stack.top()) < determine_precedence(tk) ) {
            output_queue.push_back(std::move(operator_stack.top()));
            operator_stack.pop();
          }
          operator_stack.push(std::move(tk));
          break;
        case (token_kind::comma):
          while ( !operator_stack.emtpy() && operator_stack.top().kind != token_kind::left_parenthesis){
            output_queue.push_back(std::move(operator_stack.top()));
            operator_stack.pop();
          }
          break;
        case (token_kind::left_parenthesis):
          operator_stack.emplace(std::move(tk));
          break;
        case (token_kind::right_parenthesis):
          do {
            if ( operator_stack.empty() )
              // there is no left parenthesis preceding it
              throw std::runtime_error("Mismatched parenthesis");

            output_queue.push_back(std::move(operator_stack.top()));
            operator_stack.pop();
          } while ( !operator_stack.empty() && operator_stack.top().kind != token_kind::left_parenthesis );

          if ( operator_stack.empty() )
            // there is no left parenthesis preceding it
            throw std::runtime_error("Mismatched parenthesis");
          
          operator_stack.pop(); // remove left parenthesis

          if ( !operator_stack.empty() && operator_stack.top().kind == token_kind::function ) {
            // if there is a function in the operator stack, the parentheses belong to it
            output_queue.push_back(std::move(operator_stack.top()));
            operator_stack.pop();
          }

          break;
        case (token_kind::none): // TODO: remove
          __builtin_unreachable();
        }
      }

      while ( !operator_stack.empty() ) {
        if ( operator_stack.top() == token_kind::left_parenthesis || operator_stack.top() == token_kind::right_parenthesis )
          throw std::runtime_error("Mismatched parenthesis");
        else {
          output_queue.push_back(std::move(operator_stack.top()));
          operator_stack.pop();
        }
      }

      // TODO: parse the output queue now that the tokens are sorted

      return {};
    }
  } // namespace

#ifndef MPT_DOXYGEN_WARD
  template <class Operators, class ... FunctorSignature>
  struct parser;
#endif

  /*!\brief Small class to parse arithmetic and relational functors from strings

    This class allows to define a general type that would allow to process
    any operation (specified as a string) involving a set of functors and
    additional operators (functions) and convert it into a runtime functor.
   */
  template<class ... Operator, class ... Output, class ... Input>
  class parser<mpt::types<Operator ...>, Output(Input ...) ...> {

    public:
      
      static_assert((mpt::templated_object_has_type_v<Operator, all_operators> && ...), "Wrong operators specified in parser");

    using functor_map_type = functor_map<Output(Input ...) ...>;
    using function_map_type = function_map<Output(Input ...) ...>;

    parser() = delete;

    template<class Operators, class ... Signature>
    friend parser<Operators, Signature ...> make_parser(functor_map_type, function_map_type);
    
    template <class Signature>
    runtime_arfunctor<Signature> parse(std::string_view const &input) const {
      return parse_impl<Signature, mpt::types<Operator ...>>(input, m_functor_map, m_function_map);
    }

    template <class Signature>
    runtime_arfunctor<Signature> parse(std::string const &input) const {
      return parse<Signature>(std::string_view(input));
    }

    template <class Signature>
    runtime_arfunctor<Signature> parse(const char *input) const {
      return parse<Signature>(std::string_view(input));
    }

    private:

      parser(functor_map_type functors, function_map_type functions) : m_functor_map{std::move(functors)}, m_function_map{std::move(functions)} { }

      functor_map_type m_functor_map;
      function_map_type m_function_map;
  };

  template<class Operators, class ... Signature>
  parser<Operators, Signature ...> make_parser(functor_map<Signature ...> functors, function_map<Signature ...> functions) {
    return parser<Operators, Signature...>(std::move(functors), std::move(functions));
  }
} // namespace mpt
