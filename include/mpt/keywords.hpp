#pragma once
#include "mpt/types.hpp"
#include "mpt/values.hpp"
#include <tuple>

namespace mpt {

  /// Represent the description of a keyword argument
  template <class T> struct keyword_argument {
    using value_type = T;
    value_type value;
  };

  /// Set of required keyword arguments
  template <class...> struct required_keyword_arguments;

  /// Set of required keyword arguments
  template <class... T>
  struct required_keyword_arguments<keyword_argument<T>...> {};

  /*!\brief Class that accepts keyword arguments in the constructor

    Keyword arguments are wrappers around floating point and integral values
    that are used in order to avoid having several inputs values with no visible
    correspondence to parameters in the class. The use of keywords also allows
    to provide the input arguments in any order.

    The keyword arguments are stored within the class, which inherits from
    \ref std::tuple. You can use the \ref keywords_parser::get and
    \ref keywords_parser::set member functions to manipulate the values.
   */
  template <class Required, class... Keyword> class keywords_parser;

  template <class... RequiredKeyword, class... Keyword>
  class keywords_parser<required_keyword_arguments<RequiredKeyword...>,
                        Keyword...>
      : protected std::tuple<typename RequiredKeyword::value_type...,
                             typename Keyword::value_type...> {

    static_assert(
        !has_repeated_template_arguments_v<RequiredKeyword..., Keyword...>,
        "Keyword arguments are repeated");

  public:
    /// Base type
    using base_type = std::tuple<typename RequiredKeyword::value_type...,
                                 typename Keyword::value_type...>;

    keywords_parser() = default;
    keywords_parser(keywords_parser const &) = default;
    keywords_parser(keywords_parser &&) = default;
    keywords_parser &operator=(keywords_parser const &) = default;
    keywords_parser &operator=(keywords_parser &&) = default;

    /// Constructor from the keyword arguments and a tuple of default values
    template <class Tuple, class... K>
    keywords_parser(Tuple &&defaults, K &&...v) noexcept
        : base_type{parse_keywords_with_defaults_and_required(
              std::forward<Tuple>(defaults), std::forward<K>(v)...)} {}

    /// Get a keyword argument
    template <class K> constexpr auto get() const {
      return std::get<mpt::index_v<K, RequiredKeyword..., Keyword...>>(*this);
    }

    /// Set a keyword argument
    template <class K> constexpr auto set(typename K::value_type v) const {
      std::get<mpt::index_v<K, RequiredKeyword..., Keyword...>>(*this) = v;
    }

  private:
    /*!\brief Parse a single keyword argument

      If a value is not provided, it is taken from the tuple of default values.
    */
    template <std::size_t I, class Tuple, class... K>
    static constexpr auto value_or_default(Tuple &&defaults, K &&...keyword) {

      using current_keyword_type =
          mpt::type_at_t<I, RequiredKeyword..., Keyword...>;

      typename current_keyword_type::value_type value;

      // conversion to the type from the current type descriptor
      if constexpr (mpt::has_type_v<current_keyword_type,
                                    std::remove_reference_t<K>...>) {
        value = mpt::value_at<mpt::index_v<current_keyword_type,
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
                                      Tuple &&defaults, K &&...keyword) {

      return {value_or_default<I>(std::forward<Tuple>(defaults),
                                  std::forward<K>(keyword)...)...};
    }

    template <class... Default>
    static constexpr void check_defaults(std::tuple<Default...> const &) {
      static_assert(!has_repeated_template_arguments_v<Default...>,
                    "Default keyword arguments are repeated");
      static_assert(
          !(mpt::has_type_v<RequiredKeyword, Default...> || ...),
          "Required keyword arguments are found in the list of default values");
    }

    template <class... K> static constexpr void check_required(K const &...) {
      static_assert(
          !has_repeated_template_arguments_v<std::remove_reference_t<K>...>,
          "Required keyword arguments are repeated");
      static_assert(
          (mpt::has_type_v<RequiredKeyword, std::remove_reference_t<K>...> &&
           ...),
          "Some required keyword arguments are not provided");
    }

    /// Parse the input keyword arguments with the given list of default values
    template <class Tuple, class... K>
    static constexpr base_type
    parse_keywords_with_defaults_and_required(Tuple &&defaults,
                                              K &&...keywords) {

      check_defaults(defaults);
      check_required(keywords...);

      return parse_keywords_with_defaults_impl(
          std::make_index_sequence<((sizeof...(RequiredKeyword)) +
                                    (sizeof...(Keyword)))>(),
          std::forward<Tuple>(defaults), std::forward<K>(keywords)...);
    }
  };
} // namespace mpt
