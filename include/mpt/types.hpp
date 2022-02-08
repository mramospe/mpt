#pragma once
#include <cstdlib>
#include <type_traits>
#include <variant>

namespace mpt {

  template <class T> struct type_wrapper {};

  /// Whether the type is in the given list
  template <class Reference, class... T> struct has_type : std::false_type {};

  /// Whether the type is in the given list
  template <class Reference, class... T>
  struct has_type<Reference, Reference, T...> : std::true_type {};

  /// Whether the type is in the given list
  template <class Reference, class T0, class... T>
  struct has_type<Reference, T0, T...> : has_type<Reference, T...> {};

  /// Whether the type is in the given list
  template <class Reference, class... T>
  static constexpr auto has_type_v = has_type<Reference, T...>::value;

  /// Check if a list of template arguments has repeated types
  template <class... T>
  struct has_repeated_template_arguments : std::false_type {};

  /// Check if a list of template arguments has repeated types
  template <class T0, class... T>
  struct has_repeated_template_arguments<T0, T...>
      : std::conditional_t<has_type_v<T0, T...>, std::true_type,
                           has_repeated_template_arguments<T...>> {};

  /// Check if a list of template arguments has repeated types
  template <class... T>
  static constexpr auto has_repeated_template_arguments_v =
      has_repeated_template_arguments<T...>::value;

  /// Check at compile-time if a reference template is in the list of template
  /// arguments
  template <template <class> class Ref, template <class> class... T>
  struct has_single_template : std::false_type {};

  /// Check at compile-time if a reference template is in the list of template
  /// arguments
  template <template <class> class Ref>
  struct has_single_template<Ref> : std::false_type {};

  /// Check at compile-time if a reference template is in the list of template
  /// arguments
  template <template <class> class Ref, template <class> class T0,
            template <class> class... T>
  struct has_single_template<Ref, T0, T...> : has_single_template<Ref, T...> {};

  /// Check at compile-time if a reference template is in the list of template
  /// arguments
  template <template <class> class Ref, template <class> class... T>
  struct has_single_template<Ref, Ref, T...> : std::true_type {};

  /// Check at compile-time if a reference template is in the list of template
  /// arguments
  template <template <class> class Ref, template <class> class... T>
  static constexpr auto has_single_template_v =
      has_single_template<Ref, T...>::value;

  /// Get the index of the type in the list of types
  template <class Match, class... T> struct index;

  /// Get the index of the type in the list of types
  template <class Match, class T0, class... T> struct index<Match, T0, T...> {
    static constexpr auto value = index<Match, T...>::value + 1;
  };

  /// Get the index of the type in the list of types
  template <class Match, class... T> struct index<Match, Match, T...> {
    static_assert(!has_type_v<Match, T...>,
                  "Multiple matches found for the given type");
    static constexpr auto value = 0u;
  };

  /// Get the type at the given position
  template <class Match, class... T>
  static constexpr auto index_v = index<Match, T...>::value;

  /// Get the index of the type in the list of types
  template <template <class> class Match, template <class> class... T>
  struct template_index;

  /// Get the index of the type in the list of types
  template <template <class> class Match, template <class> class T0,
            template <class> class... T>
  struct template_index<Match, T0, T...> {
    static constexpr auto value = template_index<Match, T...>::value + 1;
  };

  /// Get the index of the type in the list of types
  template <template <class> class Match, template <class> class... T>
  struct template_index<Match, Match, T...> {
    static constexpr auto value = 0u;
  };

  /// Get the type at the given position
  template <template <class> class Match, template <class> class... T>
  static constexpr auto template_index_v = template_index<Match, T...>::value;

  /// Get the type at the given position
  template <std::size_t I, class T0, class... T> struct type_at {
    using type = typename type_at<I - 1, T...>::type;
  };

  /// Get the type at the given position
  template <class T0, class... T> struct type_at<0, T0, T...> {
    using type = T0;
  };

  /// Get the template argument at the given position
  template <std::size_t I, class... T>
  using type_at_t = typename type_at<I, T...>::type;

  /// Get the template argument at the given position
  template <std::size_t I, template <class> class T0,
            template <class> class... T>
  struct template_at {
    template <class V>
    using tpl = typename template_at<I - 1, T...>::template tpl<V>;
  };

  /// Get the template argument at the given position
  template <template <class> class T0, template <class> class... T>
  struct template_at<0, T0, T...> {
    template <class V> using tpl = T0<V>;
  };

  /// Expand a std::variant object with a new type, if it does not contain it
  /// yet
  template <class Variant, class NewType, class Enable = void>
  struct expand_variant;

  /// Expand a std::variant object with a new type, if it does not contain it
  /// yet
  template <class... T, class NewType>
  struct expand_variant<std::variant<T...>, NewType,
                        std::enable_if_t<!mpt::has_type_v<NewType, T...>>> {
    using type = std::variant<T..., NewType>;
  };

  /// Expand a std::variant object with a new type, if it does not contain it
  /// yet
  template <class... T, class NewType>
  struct expand_variant<std::variant<T...>, NewType,
                        std::enable_if_t<mpt::has_type_v<NewType, T...>>> {
    using type = std::variant<T...>;
  };

  /// Expand a std::variant object with a new type, if it does not contain it
  /// yet
  template <class Variant, class NewType>
  using expand_variant_t = typename expand_variant<Variant, NewType>::type;

  /// Expand a std::variant with several types, avoiding repetitions
  template <class Variant, class... T> struct expand_variant_with_types;

  /// Expand a std::variant with several types, avoiding repetitions
  template <class... T> struct expand_variant_with_types<std::variant<T...>> {
    using type = std::variant<T...>;
  };

  /// Expand a std::variant with several types, avoiding repetitions
  template <class... VariantTypes, class NewType, class... T>
  struct expand_variant_with_types<std::variant<VariantTypes...>, NewType,
                                   T...> {
    using type = typename expand_variant_with_types<
        expand_variant_t<std::variant<VariantTypes...>, NewType>, T...>::type;
  };

  /// Expand a std::variant with several types, avoiding repetitions
  template <class Variant, class... NewTypes>
  using expand_variant_with_types_t =
      typename expand_variant_with_types<Variant, NewTypes...>::type;
} // namespace mpt
