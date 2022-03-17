/*!\file
  Utilities to handle classes with more friendly default-value settings.
  The different classes and functions in this file allow to define classes built
  from arguments that might (or not) be required or have default values. The
  most straightforward use-case appears when defining classes that are
  configured from several numerical values (with defaults) in which we might
  need to modify some arguments without specifying others for which we want to
  use the default values:

  \code{.cpp}
  class algorithm {
    public:
      algorithm(double alpha, int beta=1, float delta=2.) : m_alpha{alpha},
  m_beta{beta}, m_delta{delta} { } private: double m_alpha; int m_beta; float
  m_delta;
  };

  void some_function() {
    algorithm op(1., 1, 4.); // here we must specify "beta" if we want to change
  "delta"
  }
  \endcode
*/
#pragma once
#include "mpt/types.hpp"
#include "mpt/values.hpp"
#include <tuple>

namespace mpt {

  /// Represent the description of a keyword argument
  template <class T> using keyword_argument = mpt::value_wrapper<T>;

#ifndef MPT_DOXYGEN_WARD
  template <class...> struct required_keyword_arguments;
#endif

  /// Set of required keyword arguments
  template <class... T>
  struct required_keyword_arguments<keyword_argument<T>...> {};

#ifndef MPT_DOXYGEN_WARD
  template <class...> struct keyword_arguments_with_default;
#endif

  /// Set of keyword arguments that have default values
  template <class... T>
  struct keyword_arguments_with_default<keyword_argument<T>...> {};

#ifndef MPT_DOXYGEN_WARD
  template <class Required, class Defaulted> class keyword_arguments_parser;
#endif

  /*!\brief Class that accepts keyword arguments in the constructor

    Keyword arguments are wrappers around floating point and integral values
    that are used in order to avoid having several inputs values with no visible
    correspondence to parameters in the class. The use of keywords also allows
    to provide the input arguments in any order.

    The keyword arguments are stored within the class, which inherits from
    \ref std::tuple. You can use the \ref mpt::keyword_arguments_parser::get() and
    \ref mpt::keyword_arguments_parser::set() member functions to manipulate the
    values.

    An example of this class would be:
    \code{.cpp}
    struct alpha : mpt::keyword_argument<float> {};
    struct beta : mpt::keyword_argument<int> {};
    struct delta : mpt::keyword_argument<float> {};

    class algorithm : public
    mpt::keyword_arguments_parser<mpt::required_keyword_arguments<alpha>,
                                                           mpt::keyword_arguments_with_default<beta,
    delta>> {

      using base_class =
    mpt::keyword_arguments_parser<mpt::required_keyword_arguments<alpha>,
                                                       mpt::keyword_arguments_with_default<beta,
    delta>>; using base_class::base_class;
    };

    void some_function(double a) {

      algorithm algo(std::make_tuple(beta{1}, delta{2.}), delta{4.f}, alpha{a});

      ...

      auto stored_alpha = algo.get<alpha>();

      ...
    }
    \endcode

    In this case, the value of \a alpha will be overwritten by the value passed
    to \a some_function. Note that the order of the arguments can be arbitrary.
  */
  template <class... R, class... D>
  class keyword_arguments_parser<required_keyword_arguments<R...>,
                                 keyword_arguments_with_default<D...>>
      : protected std::tuple<typename R::value_type...,
                             typename D::value_type...> {

    static_assert(!has_repeated_template_arguments_v<R..., D...>,
                  "Keyword arguments are repeated");

  public:
    /// Base type
    using base_type =
        std::tuple<typename R::value_type..., typename D::value_type...>;

    keyword_arguments_parser() = default;
    keyword_arguments_parser(keyword_arguments_parser const &) = default;
    keyword_arguments_parser(keyword_arguments_parser &&) = default;
    keyword_arguments_parser &
    operator=(keyword_arguments_parser const &) = default;
    keyword_arguments_parser &operator=(keyword_arguments_parser &&) = default;

    /// Constructor from the keyword arguments and a tuple of default values
    template <class Tuple, class... K>
    keyword_arguments_parser(Tuple &&defaults, K &&... v) noexcept
        : base_type{parse_keywords_with_defaults_and_required(
              std::forward<Tuple>(defaults), std::forward<K>(v)...)} {}

    /// Get a keyword argument
    template <class K> constexpr auto get() const {
      return std::get<mpt::type_index_v<K, R..., D...>>(*this);
    }

    /// Set a keyword argument
    template <class K, class Arg> constexpr void set(Arg &&v) {
      std::get<mpt::type_index_v<K, R..., D...>>(*this) = std::forward<Arg>(v);
    }

  private:
    /*!\brief Parse a single keyword argument

      If a value is not provided, it is taken from the tuple of default values.
    */
    template <std::size_t I, class Tuple, class... K>
    static constexpr auto value_or_default(Tuple &&defaults, K &&... keyword) {

      using current_keyword_type = mpt::type_at_t<I, R..., D...>;

      typename current_keyword_type::value_type value;

      // conversion to the type from the current type descriptor
      if constexpr (mpt::has_type_v<current_keyword_type,
                                    std::remove_reference_t<K>...>) {
        value = mpt::value_at<mpt::type_index_v<current_keyword_type,
                                                std::remove_reference_t<K>...>>(
                    std::forward<K>(keyword)...)
                    .value;
      } else
        value =
            std::get<current_keyword_type>(std::forward<Tuple>(defaults)).value;

      return value;
    }

    /// Parse the input keyword arguments with the given list of default values
    template <std::size_t... I, class Tuple, class... K>
    static constexpr base_type
    parse_keywords_with_defaults_impl(std::index_sequence<I...>,
                                      Tuple &&defaults, K &&... keyword) {

      return {value_or_default<I>(std::forward<Tuple>(defaults),
                                  std::forward<K>(keyword)...)...};
    }

    template <class... Default>
    static constexpr void check_defaults(std::tuple<Default...> const &) {
      static_assert(!has_repeated_template_arguments_v<Default...>,
                    "Default keyword arguments are repeated");
      static_assert(
          !(mpt::has_type_v<R, Default...> || ...),
          "Required keyword arguments are found in the list of default values");
    }

    template <class... K> static constexpr void check_required(K const &...) {
      static_assert(
          !has_repeated_template_arguments_v<std::remove_reference_t<K>...>,
          "Required keyword arguments are repeated");
      static_assert((mpt::has_type_v<R, std::remove_reference_t<K>...> && ...),
                    "Some required keyword arguments are not provided");
    }

    /// Parse the input keyword arguments with the given list of default values
    template <class Tuple, class... K>
    static constexpr base_type
    parse_keywords_with_defaults_and_required(Tuple &&defaults,
                                              K &&... keywords) {

      check_defaults(defaults);
      check_required(keywords...);

      return parse_keywords_with_defaults_impl(
          std::make_index_sequence<((sizeof...(R)) + (sizeof...(D)))>(),
          std::forward<Tuple>(defaults), std::forward<K>(keywords)...);
    }
  };
} // namespace mpt
