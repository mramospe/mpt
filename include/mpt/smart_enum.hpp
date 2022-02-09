#pragma once
#include <algorithm>
#include <array>
#include <stdexcept>
#include <string>
#include <string_view>

/*\brief Tools to handle smart enumeration types

  A smart enumeration type is an enumeration type that can be created/converted
  from/to a string of characters.
 */
namespace mpt::smart_enum {

  /// Type of a buffer on a enumeration type declaration
  template <std::size_t N> using buffer_type = std::array<char, N>;

  /// Type of the array of enumeration names
  template <std::size_t N> using names_type = std::array<std::string_view, N>;

  /// Type of an array of enumeration values
  template <class EnumType, std::size_t N>
  using values_type = std::array<EnumType, N>;

  /// Functions for developers only
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
    constexpr auto make_names(buffer_type<BufferSize> const &buffer) {

      names_type<N> result;

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

    void throw_unknown_enumeration_type(std::string const &str) {
      throw std::runtime_error("The string \"" + str +
                               "\" resolves to an unknown enumeration value");
    }
  } // namespace detail

  /// Access the properties of an enumeration type via ADL
  template <class EnumType> struct properties {
    using type = decltype(smart_enum_properties(EnumType{}));
  };

  /// Access the properties of an enumeration type via ADL
  template <class EnumType>
  using properties_t = typename properties<EnumType>::type;

  /// Whether the given value corresponds to an unknown value
  template <class EnumType> constexpr bool is_unknown(EnumType e) {
    return e == properties_t<EnumType>::unknown_value;
  }

  /// Determine the enumeration value from the given string
  template <class EnumType>
  constexpr auto from_string_view(std::string_view const &str) {

    for (auto i = 0u; i < properties_t<EnumType>::size_with_unknown; ++i)
      if (str == properties_t<EnumType>::names_with_unknown[i])
        return properties_t<EnumType>::values_with_unknown[i];

    return properties_t<EnumType>::unknown_value;
  }

  /// Determine the enumeration value from the given string view, throwing an
  /// error if unknown
  template <class EnumType>
  auto from_string_view_throw_if_unknown(std::string_view const &str) {

    auto value = from_string_view<EnumType>(str);

    if (value == properties_t<EnumType>::unknown_value)
      detail::throw_unknown_enumeration_type(
          std::string{str.data(), str.size()});

    return value;
  }

  /// Determine the enumeration value from the given string
  template <class EnumType> constexpr auto from_string(std::string const &str) {

    return from_string_view<EnumType>(str.c_str());
  }

  /// Determine the enumeration value from the given string, throwing an error
  /// if unknown
  template <class EnumType>
  auto from_string_throw_if_unknown(std::string const &str) {

    auto value = from_string<EnumType>(str);

    if (value == properties_t<EnumType>::unknown_value)
      detail::throw_unknown_enumeration_type(str);

    return value;
  }

  /// Access the name of the given enumeration value
  template <class EnumType> constexpr auto to_string_view(EnumType e) {

    for (auto i = 0u; i < properties_t<EnumType>::size_with_unknown; ++i)
      if (e == properties_t<EnumType>::values_with_unknown[i])
        return properties_t<EnumType>::names_with_unknown[i];

    // return the default value
    return to_string_view(properties_t<EnumType>::unknown_value);
  }

  /// Access the name of the given enumeration value
  template <class EnumType> constexpr auto to_string(EnumType e) {

    for (auto i = 0u; i < properties_t<EnumType>::size_with_unknown; ++i)
      if (e == properties_t<EnumType>::values_with_unknown[i])
        return std::string{properties_t<EnumType>::names_with_unknown[i]};

    // return the default value
    return to_string(properties_t<EnumType>::unknown_value);
  }
} // namespace mpt::smart_enum

#define MPT_DEFINE_STRING_CONVERTERS(enum_name)                                \
  std::string to_string(enum_name e) { return mpt::smart_enum::to_string(e); } \
  std::string_view to_string_view(enum_name e) {                               \
    return mpt::smart_enum::to_string_view(e);                                 \
  }                                                                            \
  static_assert(true)

/// Declare a smart enumeration type
#define MPT_SMART_ENUM(enum_name, enum_properties_name, type, unknown, ...)    \
  enum enum_name : type { unknown, __VA_ARGS__ };                              \
  namespace {                                                                  \
    struct enum_properties_name {                                              \
      using underlying_type = type;                                            \
      struct va_args_with_unknown {                                            \
        static constexpr const char *value = #unknown "," #__VA_ARGS__;        \
      };                                                                       \
      struct va_args {                                                         \
        static constexpr const char *value = #__VA_ARGS__;                     \
      };                                                                       \
      static constexpr auto unknown_value = unknown;                           \
      static constexpr auto buffer =                                           \
          mpt::smart_enum::detail::make_buffer<va_args>();                     \
      static constexpr auto buffer_with_unknown =                              \
          mpt::smart_enum::detail::make_buffer<va_args_with_unknown>();        \
      static constexpr std::size_t size =                                      \
          std::count_if(buffer.cbegin(), buffer.cend(),                        \
                        [](auto const &v) { return v == ','; }) +              \
          1;                                                                   \
      static constexpr std::size_t size_with_unknown = size + 1;               \
      static constexpr mpt::smart_enum::names_type<size> names =               \
          mpt::smart_enum::detail::make_names<size>(buffer);                   \
      static constexpr mpt::smart_enum::names_type<size_with_unknown>          \
          names_with_unknown =                                                 \
              mpt::smart_enum::detail::make_names<size_with_unknown>(          \
                  buffer_with_unknown);                                        \
      static constexpr mpt::smart_enum::values_type<enum_name, size> values =  \
          {__VA_ARGS__};                                                       \
      static constexpr mpt::smart_enum::values_type<enum_name,                 \
                                                    size_with_unknown>         \
          values_with_unknown = {unknown, __VA_ARGS__};                        \
    };                                                                         \
  }                                                                            \
  constexpr enum_properties_name smart_enum_properties(enum_name) {            \
    return {};                                                                 \
  }                                                                            \
  MPT_DEFINE_STRING_CONVERTERS(enum_name);                                     \
  static_assert(true)

// TODO: With GCC 11 and Clang 13 we can make use of the "using enum
// <enum_name>" declaration to efficiently work with scoped enumeration types
