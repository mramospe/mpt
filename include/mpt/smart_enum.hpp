/*!\file

  This header defines tools to create and handle smart enumeration types.
  A smart enumeration type is an enumeration type that can be created/converted
  from/to a string of characters.
  The definition of smart enumeration types is done through the \ref
  MPT_SMART_ENUM macro. Consider, for example: \code{.cpp}
  MPT_SMART_ENUM(position, position_properties, int, unknown_position, bottom,
  middle, top);

  auto some_function(position p) {

    switch (p) {
      case (unknown_position):
        return ...;
      case (bottom):
        return ...;
      case (middle):
        return ...;
      case (top):
        return ...;
    }
  }
  \endcode
  In this case we are creating an enumeration type called \a position, whose
  properties will be saved in a new type called \a position_properties. The
  enumeration type will be of type \a int, whenever there is a problem with the
  processing of the enumeration type (like for conversion to a string of
  characters) the value \a unknown_position will be returned, and \a bottom, \a
  middle and \a top are the members of the enumeration type, with values \a 1,
  \a 2 and \a 3.
*/
#pragma once
#include <algorithm>
#include <array>
#include <stdexcept>
#include <string>
#include <string_view>
#include <variant>

namespace mpt {

  /// Type of a buffer on a enumeration type declaration
  template <std::size_t N> using array_buffer_type = std::array<char, N>;

  /// Type of the array of enumeration names
  template <std::size_t N>
  using array_of_string_view = std::array<std::string_view, N>;

  /// Type of an array of enumeration values
  template <class EnumType, std::size_t N>
  using array_of_smart_enum = std::array<EnumType, N>;

  namespace detail {

    /// Create an array of characters corresponding to the input to the
    /// declaration of an enumeration type
    template <class ConstexprString> constexpr auto make_buffer() {

      constexpr auto n = std::char_traits<char>::length(ConstexprString::value);

      std::array<char, n> buffer;
      std::size_t counter = 0;
      std::generate(buffer.begin(), buffer.end(),
                    [&counter]() { return ConstexprString::value[counter++]; });

      return buffer;
    }

    /// Determine the names of the enumeration values from a buffer string
    template <std::size_t N, std::size_t BufferSize>
    constexpr auto make_names(array_buffer_type<BufferSize> const &buffer) {

      array_of_string_view<N> result;

      // we change the status to false once we have processed an enumeration
      // value, and we change it back to true once we find a separator (a comma
      // ',')
      bool status = true;

      std::size_t counter = 0u, fc = 0u, sc = 0u;

      while (sc < buffer.size()) {

        auto c = buffer[sc];

        if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '=') {
          if (status) {
            if (fc != sc) {
              // we encountered a space after the name declaration
              status = false;
              result[counter++] = std::string_view(&buffer[fc], sc - fc);
            }

            // we might be at the begining of the string
            fc = ++sc;
          } else
            ++sc;
        } else if (c == ',') {
          // we reached the end of the declaration of an enumeration value

          if (status)
            // we still must parse this value
            result[counter++] = std::string_view(&buffer[fc], sc - fc);
          else
            status = true;

          fc = ++sc;
        } else if (sc + 1 == buffer.size() && status)
          result[counter++] = std::string_view(&buffer[fc], ++sc - fc);
        else
          ++sc;
      }

      return result;
    }

    /// Throw an error if a string resolves to an unknown value
    void throw_unknown_enumeration_type(std::string const &str) {
      throw std::runtime_error("The string \"" + str +
                               "\" resolves to an unknown enumeration value");
    }
  } // namespace detail

  /*!\brief Access the properties of an enumeration type via ADL

    \see mpt::smart_enum_properties_t
  */
  template <class EnumType> struct smart_enum_properties {
    using type = decltype(properties(EnumType{}));
  };

  /*!\brief Access the properties of an enumeration type via ADL

    \code{.cpp}
    MPT_SMART_ENUM(position, position_properties, int, unknown_position, bottom, middle, top);

    static constexpr auto position_values = smart_enum_properties_t<position>::values_with_unknown;
    \endcode

    \see mpt::smart_enum_properties
  */
  template <class EnumType>
  using smart_enum_properties_t =
      typename smart_enum_properties<EnumType>::type;

  /// Whether the given value corresponds to an unknown value
  template <class EnumType> constexpr bool is_unknown(EnumType e) {
    return e == smart_enum_properties_t<EnumType>::unknown_value;
  }

  /*!\brief Determine the enumeration value from the given string view

    \see
    mpt::from_string
    mpt::from_string_view
    mpt::from_string_view_throw_if_unknown
  */
  template <class EnumType>
  constexpr auto from_string_view(std::string_view const &str) {

    for (auto i = 0u; i < smart_enum_properties_t<EnumType>::size_with_unknown;
         ++i)
      if (str == smart_enum_properties_t<EnumType>::names_with_unknown[i])
        return smart_enum_properties_t<EnumType>::values_with_unknown[i];

    return smart_enum_properties_t<EnumType>::unknown_value;
  }

  /*!\brief Determine the enumeration value from the given string view

    An error is thrown if it evaluates to an unknown enumeration value.

    \see
    mpt::from_string_view
    mpt::from_string
    mpt::from_string_throw_if_unknown
  */
  template <class EnumType>
  auto from_string_view_throw_if_unknown(std::string_view const &str) {

    auto value = from_string_view<EnumType>(str);

    if (value == smart_enum_properties_t<EnumType>::unknown_value)
      detail::throw_unknown_enumeration_type(
          std::string{str.data(), str.size()});

    return value;
  }

  /*!\brief Determine the enumeration value from the given string

    \see
    mpt::from_string
    mpt::from_string_view
    mpt::from_string_view_throw_if_unknown
  */
  template <class EnumType> constexpr auto from_string(std::string const &str) {

    return from_string_view<EnumType>(str.c_str());
  }

  /*!\brief Determine the enumeration value from the given string

    An error is thrown if it evaluates to an unknown enumeration value.

    \see
    mpt::from_string
    mpt::from_string_view
    mpt::from_string_throw_if_unknown
  */
  template <class EnumType>
  auto from_string_throw_if_unknown(std::string const &str) {

    auto value = from_string<EnumType>(str);

    if (value == smart_enum_properties_t<EnumType>::unknown_value)
      detail::throw_unknown_enumeration_type(str);

    return value;
  }

  /*!\brief Access the name of the given enumeration value

    \see
    mpt::to_string
  */
  template <class EnumType> constexpr auto to_string_view(EnumType e) {

    for (auto i = 0u; i < smart_enum_properties_t<EnumType>::size_with_unknown;
         ++i)
      if (e == smart_enum_properties_t<EnumType>::values_with_unknown[i])
        return smart_enum_properties_t<EnumType>::names_with_unknown[i];

    // return the default value
    return to_string_view(smart_enum_properties_t<EnumType>::unknown_value);
  }

  /*!\brief Access the name of the given enumeration value

    \see
    mpt::to_string_view
  */
  template <class EnumType> constexpr auto to_string(EnumType e) {

    for (auto i = 0u; i < smart_enum_properties_t<EnumType>::size_with_unknown;
         ++i)
      if (e == smart_enum_properties_t<EnumType>::values_with_unknown[i])
        return std::string{
            smart_enum_properties_t<EnumType>::names_with_unknown[i]};

    return to_string(smart_enum_properties_t<EnumType>::unknown_value);
  }

  namespace detail {
    /// Make the array of functors for a switch statement
    template <class EnumType, template <EnumType> class Functor,
              std::size_t... I>
    constexpr std::array<std::variant<Functor<smart_enum_properties_t<
                             EnumType>::values_with_unknown[I]>...>,
                         sizeof...(I)>
    make_array_of_functors(std::index_sequence<I...>) {
      return {Functor<
          smart_enum_properties_t<EnumType>::values_with_unknown[I]>{}...};
    }
  } // namespace detail

  /*!\brief Apply a functor depending on the value of an enumeration type

    This function avoids having to write long \a switch-case expressions with
    all the enumeration values.
    The provided functor in \a Functor must be default-constructible.

    \code{.cpp}
    MPT_SMART_ENUM(position, position_properties, int, unknown_position, bottom, middle, top);

    auto some_function(position p) {

      switch (p) {
        case (unknown_position):
          return ...;
        case (bottom):
          return ...;
        case (middle):
          return ...;
        case (top):
          return ...;
      }
    }

    template<position P>
    struct functor;

    template<>
    struct functor<unknown_position> {
      double operator(double i) const { return i * 0.; }
    };

    template<>
    struct functor<unknown_position> {
      double operator(double i) const { return i * 1.; }
    };

    template<>
    struct functor<bottom> {
      double operator(double i) const { return i * 2.; }
    };

    template<>
    struct functor<middle> {
      double operator(double i) const { return i * 3.; }
    };

    template<>
    struct functor<top> {
      double operator(double i) const { return i * 4.; }
    };

    auto optimal_function(position p) {
      return mpt::apply_with_switch<position, functor>(p, 2.);
    }
    \endcode

    Effectively it creates a compile-time array of functors, for which it
    creates a \ref std::variant object with the specializations of the given
    functor \a Functor for each enumeration value. To access the correct
    functor, it profits from the fact that the enumeration values are
    consecutive and can be safely casted to an integral value, which is used
    as an index.
  */
  template <class EnumType, template <EnumType> class Functor, class... Args>
  constexpr auto apply_with_switch(EnumType e, Args &&... args) {

    using enum_properties = smart_enum_properties_t<EnumType>;

    constexpr auto functors = detail::make_array_of_functors<EnumType, Functor>(
        std::make_index_sequence<enum_properties::size_with_unknown>());

    // this part only works as long as the members of the enumeration type are
    // consecutive starting at zero
    return std::visit(
        [&](auto &&functor) { return functor(std::forward<Args>(args)...); },
        functors[static_cast<typename enum_properties::underlying_type>(e)]);
  }
} // namespace mpt

#ifndef MPT_DOXYGEN_WARD
#define MPT_DEFINE_STRING_CONVERTERS(enum_name)                                \
  std::string to_string(enum_name e) { return mpt::to_string(e); }             \
  std::string_view to_string_view(enum_name e) {                               \
    return mpt::to_string_view(e);                                             \
  }                                                                            \
  static_assert(true)
#endif

/*!\brief Declare a smart enumeration type

  The arguments are:

  - \a enum_name: name of the enumeration type, exposed in the current scope.
  - \a enum_properties_name: name of the structured object containing the
  properties of the enumeration type.
  - \a type: underlying type of the enumeration type.
  - \a unknown: name of the value that is reserved in case of problems parsing
    enumeration types.
  - \a ...: list of names that define the members of the enumeration type.

  The exposed \a enum_properties_name type will contain the following members

  - \a underlying_type: type provided in \a type
  - \a va_args_with_unknown: (for internal use only) representation of the
    input arguments as a string, enclosed in a dedicated type
  - \a va_args: (for internal use only) similar to \a va_args_with_unknown, but
    without the \a unknown value included
  - \a unknown_value: value that is used in case of problems with parsing
    enumeration values
  - \a size: number of names in the enumeration type, wihtout the unknown
  - \a size_with_unknown: total number of names in the enumeration type
  - \a names: names of the enumeration values, saved as \ref std::string_view
    objects
  - \a names_with_unknown: similar to \a names but with the unknown value
    included
  - \a values: values of the enumeration type saved in an array
  - \a values_with_unknown: similar to \a values but with the unknown value
    included

  Invoking this macro also creates a function called \a properties that is used
  in order to access the aforementioned class via \ref mpt::smart_enum_properties
  and \ref mpt::smart_enum_properties_t.

  \see
  mpt::smart_enum_properties
  mpt::smart_enum_properties_t
  mpt::to_string
  mpt::to_string_view
  mpt::from_string
  mpt::from_string_view
  mpt::from_string_throw_if_unknown
  mpt::from_string_view_throw_if_unknown
 */
#define MPT_SMART_ENUM(enum_name, enum_properties_name, type, unknown, ...)    \
  enum enum_name : type { unknown = 0, __VA_ARGS__ };                          \
  namespace {                                                                  \
    struct enum_properties_name {                                              \
      using underlying_type = type;                                            \
      struct va_args_with_unknown {                                            \
        static constexpr const char *value = #unknown "," #__VA_ARGS__;        \
        static constexpr auto buffer =                                         \
            mpt::detail::make_buffer<va_args_with_unknown>();                  \
      };                                                                       \
      struct va_args {                                                         \
        static constexpr const char *value = #__VA_ARGS__;                     \
        static constexpr auto buffer = mpt::detail::make_buffer<va_args>();    \
      };                                                                       \
      static constexpr auto unknown_value = unknown;                           \
      static constexpr std::size_t size =                                      \
          std::count_if(va_args::buffer.cbegin(), va_args::buffer.cend(),      \
                        [](auto const &v) { return v == ','; }) +              \
          1;                                                                   \
      static constexpr std::size_t size_with_unknown = size + 1;               \
      static constexpr mpt::array_of_string_view<size> names =                 \
          mpt::detail::make_names<size>(va_args::buffer);                      \
      static constexpr mpt::array_of_string_view<size_with_unknown>            \
          names_with_unknown = mpt::detail::make_names<size_with_unknown>(     \
              va_args_with_unknown::buffer);                                   \
      static constexpr mpt::array_of_smart_enum<enum_name, size> values = {    \
          __VA_ARGS__};                                                        \
      static constexpr mpt::array_of_smart_enum<enum_name, size_with_unknown>  \
          values_with_unknown = {unknown, __VA_ARGS__};                        \
    };                                                                         \
  }                                                                            \
  constexpr enum_properties_name properties(enum_name) { return {}; }          \
  MPT_DEFINE_STRING_CONVERTERS(enum_name);                                     \
  static_assert(true)

// TODO: With GCC 11 and Clang 13 we can make use of the "using enum
// <enum_name>" declaration to efficiently work with scoped enumeration types
