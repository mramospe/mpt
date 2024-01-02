/*!\file
  Utilities to parse arithmetic and relational functors from strings
*/
#pragma once
#include "mpt/signature.hpp"
#include "mpt/types.hpp"
#include "mpt/numstr.hpp"
#include "mpt/arfunctors/core/operators.hpp"
#include "mpt/arfunctors/core/runtime_arfunctor.hpp"
#include <cstring>
#include <istream>
#include <map>
#include <queue>
#include <stack>
#include <string>
#include <type_traits>
#include <variant>

#define MPT_DEFAULT_MAXIMUM_NUMBER_OF_ARGUMENTS 16ul

namespace mpt::arfunctors {

  template<class ValueType, class ... Args>
  class function_wrapper {
    public:
      using value_type = ValueType;
      virtual std::size_t expected_number_of_arguments() const = 0;
      virtual value_type operator()(Args const& ... args) const = 0;
  };

  template<class ... Signature>
  using signatures = mpt::types<Signature ...>;

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
      specialized_function_wrapper(function_type function) : m_function{std::move(function)} { }
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
    auto make_specialized_function_pointer(Function function) {
      using base_type = function_type_for_number_of_arguments_t<ValueType, MaximumNumberOfArguments>;
      using derived_type = typename specialized_function_wrapper_type_for_base<base_type, Function>::type;
      return static_cast<std::shared_ptr<base_type>>(std::make_shared<derived_type>(std::move(function)));
    }
  }

  template<class Signatures>
  class functor_proxy {
    public:
      using value_type = mpt::specialize_template_from_types_t<std::variant, all_arfunctor_types_t<Signatures>>;

      functor_proxy() = delete;
      functor_proxy(functor_proxy const&) = default;
      functor_proxy(functor_proxy&&) = default;
      functor_proxy& operator=(functor_proxy const&) = default;
      functor_proxy& operator=(functor_proxy&&) = default;

      template<class Signature> requires HasType<Signature, Signatures>
      functor_proxy(runtime_arfunctor<Signature> f) : m_functor{std::move(f)} { }
      template<class Signature> requires HasType<Signature, Signatures>
      functor_proxy& operator=(runtime_arfunctor<Signature> f) { m_functor = std::move(f); return *this; }

      auto& get() {
        return m_functor;
      }

      auto const& get() const {
        return m_functor;
      }

    private:
      value_type m_functor;
  };

  template<class Signatures, std::size_t MaximumNumberOfArguments=MPT_DEFAULT_MAXIMUM_NUMBER_OF_ARGUMENTS>
  class function_proxy {
    public:

    using functor_types = all_arfunctor_types_t<Signatures>;
    using value_type = mpt::specialize_template_t<std::variant, mpt::concatenate_types_t<mpt::arithmetic_types, functor_types>>;
    using function_type = function_type_for_number_of_arguments_t<value_type, MaximumNumberOfArguments>;

    static constexpr auto maximum_number_of_arguments = MaximumNumberOfArguments;

    template<class Function>
    function_proxy(Function f) : m_function_ptr{make_specialized_function_pointer<value_type, maximum_number_of_arguments>(std::move(f))} { }

    function_proxy() = delete;
    function_proxy(function_proxy const&) = default;
    function_proxy(function_proxy&&) = default;
    function_proxy& operator=(function_proxy const&) = default;
    function_proxy& operator=(function_proxy&&) = default;

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

    private:
      std::shared_ptr<function_type> m_function_ptr;
  };

  template<class Signatures>
  using functor_map = std::map<std::string, functor_proxy<Signatures>>;

  template<class Signatures, std::size_t MaximumNumberOfArguments=MPT_DEFAULT_MAXIMUM_NUMBER_OF_ARGUMENTS>
  using function_map = std::map<std::string, function_proxy<Signatures, MaximumNumberOfArguments>>;

  namespace {

    // Types to refer to different tokens which do not really have an associated
    // value (unlike functors, functions and arithmetic types)
    struct null_token_t { } constexpr null_token;
    struct left_parenthesis_token_t { } constexpr left_parenthesis_token;
    struct right_parenthesis_token_t { } constexpr right_parenthesis_token;
    struct comma_token_t { } constexpr comma_token;

    using void_types = mpt::types<null_token_t, left_parenthesis_token_t, right_parenthesis_token_t, comma_token_t>;

#ifndef MPT_DOXYGEN_WARD
    template <class Signatures, std::size_t MaximumNumberOfArguments>
    class token_reader;
#endif

    /// Store the state for reading tokens in a string
    template <std::size_t MaximumNumberOfArguments, class ... Signature>
    class token_reader<signatures<Signature ...>, MaximumNumberOfArguments> : private std::istream {

    public:
      using value_type = mpt::specialize_template_from_types_t<std::variant, mpt::concatenate_types_t<void_types, mpt::arithmetic_types, mpt::types<function_proxy<signatures<Signature ...>, MaximumNumberOfArguments>, runtime_arfunctor<Signature>...>>>;
      using iterator_type = std::string_view::const_iterator;
      using functor_map_pointer_type = functor_map<signatures<Signature ...>> const*;
      using function_map_pointer_type = function_map<signatures<Signature ...>, MaximumNumberOfArguments> const*;

      /// Build the object from a view of a string
      token_reader(std::string_view view, functor_map_pointer_type functors, function_map_pointer_type functions) : m_view{view}, m_it{m_view.cbegin()}, m_functors{functors}, m_functions{functions} {}

      token_reader& operator>>(value_type& tk) {

        auto cend = m_view.cend();

        while ( m_it != cend && std::isblank(*m_it) )
          ++m_it;

        if ( m_it == cend ) {
          // there were blank characters before reaching the end of the string
          setstate(std::ios_base::eofbit);
          return *this;
        }

        tk = std::isdigit(*m_it) ? parse_number() : (std::isalpha(*m_it) || *m_it == '_') ? parse_functor_or_function() : *m_it == ',' ? parse_comma() : parse_operator();

        if ( m_it == cend )
          setstate(std::ios_base::eofbit);

        return *this;
      }

      operator bool() const {
        return good();
      }

    private:

      value_type parse_number() {
        return value_type{};
      }

      value_type parse_functor_or_function() {
        
        auto start = m_it;
        auto end = std::find_if_not(++m_it, m_view.cend(), [](auto&& v) { return std::isalnum(v) || v == '_'; });

        std::string_view name{start, end};

        auto functor_it = std::find_if(m_functors->cbegin(), m_functors->cend(), [&name](auto&& v) { return v.first == name; });
        if ( functor_it != m_functors->end() )
          return std::visit([](auto&& v) -> value_type { return v; }, functor_it->second.get());
        // TODO: fixme
        auto function_it = std::find_if(m_functions->cbegin(), m_functions->cend(), [&name](auto&& v) { return v.first == name; });
        if ( function_it != m_functions->end() )
          return value_type{function_it->second};

        throw std::runtime_error("Token " + std::string{name} + " not found in the functor or in the function registries");
      }

      value_type parse_comma() {
        ++m_it; // we already checked that the pointed value was a comma
        return {null_token};
      }

      value_type parse_operator() {

        if ( *m_it == '(' ) {
          ++m_it;
          return left_parenthesis_token;}
        
        if ( *m_it == ')' ) {
          ++m_it;
          return right_parenthesis_token;
        }

        // TODO: fixme

        return value_type{};
      }

      /// The string the state works on
      std::string_view m_view;
      /// The current position of the pointer in the overall string
      std::string_view::const_iterator m_it;
      /// Pointer to the functors
      functor_map_pointer_type m_functors;
      /// Pointer to the functions
      function_map_pointer_type m_functions;
    };

    namespace {
      template<class Variant>
      precedence_type determine_precedence(Variant const& tk) {
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

    template<std::size_t MaximumNumberOfArguments, class Signatures>
    token_reader<Signatures, MaximumNumberOfArguments> make_reader(std::string_view view, functor_map<Signatures> const* functors, function_map<Signatures, MaximumNumberOfArguments> const* functions) {
      return token_reader<Signatures, MaximumNumberOfArguments>(view, functors, functions);
    }

    namespace {
      template<class T>
      struct is_runtime_arfunctor : std::false_type { };

      template<class Signature>
      struct is_runtime_arfunctor<runtime_arfunctor<Signature>> : std::true_type { };

      template<class T>
      static constexpr auto is_runtime_arfunctor_v = is_runtime_arfunctor<T>::value;

      template<class T>
      struct is_function_proxy : std::false_type { };

      template<class Signatures, std::size_t MaximumNumberOfArguments>
      struct is_function_proxy<function_proxy<Signatures, MaximumNumberOfArguments>> : std::true_type { };

      template<class T>
      static constexpr auto is_function_proxy_v = is_function_proxy<T>::value;

      template<class T, class Variant>
      auto token_is_of_kind(Variant&& var) {
        return std::visit([](auto && v) { return std::is_same_v<std::decay_t<decltype(v)>, T>; }, var);
      }

      template<class Variant>
      auto token_is_function(Variant&& var) {
        return std::visit([](auto && v) { return is_function_proxy_v<std::decay_t<decltype(v)>>; }, var);
      }
    }

    /// Parse a string and build an arithmetic and relational functor
    template <class FunctorSignature, class Signatures, std::size_t MaximumNumberOfArguments>
    runtime_arfunctor<FunctorSignature> parse_impl(std::string_view view, functor_map<Signatures> const& functors, function_map<Signatures, MaximumNumberOfArguments> const& functions) {

      using token_reader_type = token_reader<Signatures, MaximumNumberOfArguments>;
      using token_type = typename token_reader_type::value_type;

      std::queue<token_type> output_queue;
      std::stack<token_type> operator_stack;

      auto reader = make_reader(view, &functors, &functions);

      token_type tk;
      while (reader >> tk) {
        std::visit([&output_queue, &operator_stack](auto&& v){

          using type = std::decay_t<decltype(v)>;

          if constexpr ( std::is_arithmetic_v<type> )
            output_queue.push(v);
          else if constexpr ( is_runtime_arfunctor_v<type> )
            output_queue.push(v);
          else if constexpr ( is_function_proxy_v<type> )
            operator_stack.emplace(v);
          else if constexpr ( std::is_same_v<type, comma_token_t> ) {
            while ( !operator_stack.empty() && token_is_of_kind<left_parenthesis_token_t>(operator_stack.top())){
              output_queue.push(std::move(operator_stack.top()));
              operator_stack.pop();
            }
          }
          else if constexpr ( std::is_same_v<type, left_parenthesis_token_t> )
            operator_stack.emplace(v);
          else if constexpr ( std::is_same_v<type, right_parenthesis_token_t> ) {
            do {
              if ( operator_stack.empty() )
                // there is no left parenthesis preceding it
                throw std::runtime_error("Mismatched parenthesis");

              output_queue.push(std::move(operator_stack.top()));
              operator_stack.pop();
            } while ( !operator_stack.empty() && token_is_of_kind<left_parenthesis_token_t>(operator_stack.top()) );

            if ( operator_stack.empty() )
              // there is no left parenthesis preceding it
              throw std::runtime_error("Mismatched parenthesis");
            
            operator_stack.pop(); // remove left parenthesis

            if ( !operator_stack.empty() && token_is_function(operator_stack.top()) ) {
              // if there is a function in the operator stack, the parentheses belong to it
              output_queue.push(std::move(operator_stack.top()));
              operator_stack.pop();
            }
          }
          else if constexpr ( mpt::templated_object_has_type_v<type, all_operators> ) {
            while ( !operator_stack.empty() && token_is_of_kind<left_parenthesis_token_t>(operator_stack.top()) && determine_precedence(operator_stack.top()) < determine_precedence(v) ) {
              output_queue.push(std::move(operator_stack.top()));
              operator_stack.pop();
            }
            operator_stack.push(v);
          }
          else // type must be a null token
            static_assert(std::is_same_v<type, null_token_t>, "Broken implementation of the parser");
        }, tk);
      }

      while ( !operator_stack.empty() ) {
        if ( token_is_of_kind<left_parenthesis_token_t>(operator_stack.top()) || token_is_of_kind<right_parenthesis_token_t>(operator_stack.top()) )
          throw std::runtime_error("Mismatched parenthesis");
        else {
          output_queue.push(std::move(operator_stack.top()));
          operator_stack.pop();
        }
      }

      // TODO: parse the output queue now that the tokens are sorted
      return std::visit([](auto&&v ) { return static_cast<runtime_arfunctor<FunctorSignature>>(v); }, functors.at("x").get());
    }
  } // namespace

#ifndef MPT_DOXYGEN_WARD
  template <class FunctorSignatures, std::size_t MaximumNumberOfArguments>
  struct parser;
#endif

  /*!\brief Small class to parse arithmetic and relational functors from strings

    This class allows to define a general type that would allow to process
    any operation (specified as a string) involving a set of functors and
    additional operators (functions) and convert it into a runtime functor.
   */
  template<std::size_t MaximumNumberOfArguments, class ... Signature>
  class parser<signatures<Signature ...>, MaximumNumberOfArguments> {

    public:
      
    using functor_map_type = functor_map<signatures<Signature ...>>;
    using function_map_type = function_map<signatures<Signature ...>, MaximumNumberOfArguments>;

    parser() = delete;

    template<class Signatures, std::size_t N>
    friend parser<Signatures, N> make_parser(functor_map<Signatures>, function_map<Signatures, N>);
    
    template <class FunctorSignature>
    runtime_arfunctor<FunctorSignature> parse(std::string_view const &input) const {
      return parse_impl<FunctorSignature>(input, m_functor_map, m_function_map);
    }

    template <class FunctorSignature>
    runtime_arfunctor<FunctorSignature> parse(std::string const &input) const {
      return parse<FunctorSignature>(std::string_view(input));
    }

    template <class FunctorSignature>
    runtime_arfunctor<FunctorSignature> parse(const char *input) const {
      return parse<FunctorSignature>(std::string_view(input));
    }

    auto& functors() {
      return m_functor_map;
    }

    auto const& functors() const {
      return m_functor_map;
    }

    auto& functions() {
      return m_function_map;
    }

    auto const& functions() const {
      return m_function_map;
    }

    private:

      parser(functor_map_type functors, function_map_type functions) : m_functor_map{std::move(functors)}, m_function_map{std::move(functions)} { }

      functor_map_type m_functor_map;
      function_map_type m_function_map;
  };

  template<class Signatures, std::size_t MaximumNumberOfArguments>
  parser<Signatures, MaximumNumberOfArguments> make_parser(functor_map<Signatures> functors, function_map<Signatures, MaximumNumberOfArguments> functions = {}) {
    return parser<Signatures, MaximumNumberOfArguments>(std::move(functors), std::move(functions));
  }
} // namespace mpt
