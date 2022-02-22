#pragma once
#include <cstdlib>
#include <type_traits>

namespace mpt {

  /// Wrapper around a set of types
  template <class... T> struct types {};

  /// Whether the type is in the given list
  template <class Reference, class... T> struct has_type : std::false_type {};

  /// Whether the type is in the given list
  template <class Reference, class... T>
  struct has_type<Reference, Reference, T...> : std::true_type {};

#ifndef MPT_DOXYGEN_WARD
  template <class Reference, class T0, class... T>
  struct has_type<Reference, T0, T...> : has_type<Reference, T...> {};
#endif

  /// Whether the type is in the given list
  template <class Reference, class... T>
  static constexpr auto has_type_v = has_type<Reference, T...>::value;

  /// Whether the type is in the given template type
  template <class Reference, class Object> struct templated_object_has_type;

#ifndef MPT_DOXYGEN_WARD
  template <class Reference, template <class...> class Object, class... T>
  struct templated_object_has_type<Reference, Object<T...>> {
    static constexpr auto value = has_type_v<Reference, T...>;
  };
#endif

  /// Whether the type is in the given template type
  template <class Reference, class Object>
  static constexpr auto templated_object_has_type_v =
      templated_object_has_type<Reference, Object>::value;

  /// Check if a list of template arguments has repeated types
  template <class... T>
  struct has_repeated_template_arguments : std::false_type {};

#ifndef MPT_DOXYGEN_WARD
  template <class T0, class... T>
  struct has_repeated_template_arguments<T0, T...>
      : std::conditional_t<has_type_v<T0, T...>, std::true_type,
                           has_repeated_template_arguments<T...>> {};
#endif

  /// Check if a list of template arguments has repeated types
  template <class... T>
  static constexpr auto has_repeated_template_arguments_v =
      has_repeated_template_arguments<T...>::value;

  /// Whether the type is in the given template type
  template <class Reference, class Object>
  struct templated_object_has_repeated_template_arguments;

#ifndef MPT_DOXYGEN_WARD
  template <class Reference, template <class...> class Object, class... T>
  struct templated_object_has_repeated_template_arguments<Reference,
                                                          Object<T...>> {
    static constexpr auto value =
        has_repeated_template_arguments_v<Reference, T...>;
  };
#endif

  /// Whether the type is in the given template type
  template <class Reference, class Object>
  static constexpr auto templated_object_has_repeated_template_arguments_v =
      templated_object_has_repeated_template_arguments<Reference,
                                                       Object>::value;

  /// Get the index of the type in the list of types
  template <class Match, class... T> struct type_index;

#ifndef MPT_DOXYGEN_WARD
  template <class Match, class T0, class... T>
  struct type_index<Match, T0, T...> {
    static constexpr auto value = type_index<Match, T...>::value + 1;
  };

  template <class Match, class... T> struct type_index<Match, Match, T...> {
    static_assert(!has_type_v<Match, T...>,
                  "Multiple matches found for the given type");
    static constexpr auto value = 0u;
  };
#endif

  /// Get the type at the given position
  template <class Match, class... T>
  static constexpr auto type_index_v = type_index<Match, T...>::value;

  /// Whether the type is in the given template type
  template <class Reference, class Object> struct templated_object_type_index;

#ifndef MPT_DOXYGEN_WARD
  template <class Reference, template <class...> class Object, class... T>
  struct templated_object_type_index<Reference, Object<T...>> {
    static constexpr auto value = type_index_v<Reference, T...>;
  };
#endif

  /// Whether the type is in the given template type
  template <class Reference, class Object>
  static constexpr auto templated_object_type_index_v =
      templated_object_type_index<Reference, Object>::value;

  /// Get the type at the given position
  template <std::size_t I, class T0, class... T> struct type_at {
    using type = typename type_at<I - 1, T...>::type;
  };

#ifndef MPT_DOXYGEN_WARD
  template <class T0, class... T> struct type_at<0, T0, T...> {
    using type = T0;
  };
#endif

  /// Get the template argument at the given position
  template <std::size_t I, class... T>
  using type_at_t = typename type_at<I, T...>::type;

  /// Whether the type is in the given template type
  template <std::size_t I, class Object> struct templated_object_type_at;

#ifndef MPT_DOXYGEN_WARD
  template <std::size_t I, template <class...> class Object, class... T>
  struct templated_object_type_at<I, Object<T...>> {
    using type = type_at_t<I, T...>;
  };
#endif

  /// Whether the type is in the given template type
  template <std::size_t I, class Object>
  using templated_object_type_at_t =
      typename templated_object_type_at<I, Object>::type;

  namespace {

    /// Expand a set of types object with a new type, if it does not contain it
    /// yet
    template <class TypesSet, class NewType, class Enable = void>
    struct expand_types_set;

    /// Expand a set of types with a new type, if it does not contain it yet
    template <class... T, class NewType>
    struct expand_types_set<types<T...>, NewType,
                            std::enable_if_t<!mpt::has_type_v<NewType, T...>>> {
      using type = types<T..., NewType>;
    };

    /// Expand a set of types with a new type, if it does not contain it yet
    template <class... T, class NewType>
    struct expand_types_set<types<T...>, NewType,
                            std::enable_if_t<mpt::has_type_v<NewType, T...>>> {
      using type = types<T...>;
    };

    /// Expand a set of types with a new type, if it does not contain it yet
    template <class TypesSet, class NewType>
    using expand_types_set_t =
        typename expand_types_set<TypesSet, NewType>::type;
  } // namespace

  /// Define a set of types with several types, avoiding repetitions
  template <class TypesSet, class... T> struct types_set;

#ifndef MPT_DOXYGEN_WARD
  template <class... T> struct types_set<types<T...>> {
    using type = types<T...>;
  };

  template <class... Types, class NewType, class... T>
  struct types_set<types<Types...>, NewType, T...> {
    using type =
        typename types_set<expand_types_set_t<types<Types...>, NewType>,
                           T...>::type;
  };
#endif

  /// Define a set of types with several types, avoiding repetitions
  template <class... NewTypes>
  using types_set_t = typename types_set<types<>, NewTypes...>::type;

  /// Make a set of types
  template <class... T> constexpr types_set_t<T...> make_type_set() {
    return {};
  }

  namespace {

    /// Specialize a template using mpt::types
    template <template <class...> class Template, class... T>
    struct specialize_template_from_types;

    /// Specialize a template using mpt::types
    template <template <class...> class Template, class... T>
    struct specialize_template_from_types<Template, types<T...>> {
      using type = Template<T...>;
    };

    /// Specialize a template using mpt::types
    template <template <class...> class Template, class TypesWrapper>
    using specialize_template_from_types_t =
        typename specialize_template_from_types<Template, TypesWrapper>::type;
  } // namespace

  /// Specialize the given template
  template <template <class...> class Template, class... T>
  struct specialize_template {
    using type = specialize_template_from_types_t<Template, types<T...>>;
  };

  /// Specialize the given template
  template <template <class...> class Template, class... T>
  using specialize_template_t =
      typename specialize_template<Template, T...>::type;

  /// Specialize the given template avoiding repetitions
  template <template <class...> class Template, class... T>
  struct specialize_template_avoid_repetitions {
    using type = specialize_template_from_types_t<Template, types_set_t<T...>>;
  };

  /// Specialize the given template avoiding repetitions
  template <template <class...> class Template, class... T>
  using specialize_template_avoid_repetitions_t =
      typename specialize_template_avoid_repetitions<Template, T...>::type;

} // namespace mpt
