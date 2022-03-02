/*!\file

  Definition of a typed \a any type and functions to work with it.
 */
#pragma once
#include "mpt/types.hpp"
#include <any>
#include <array>
#include <functional>

namespace mpt {

  namespace detail {
    /// Find the first class in the template argument list that is
    /// default-constructible
    template <class... T> struct first_object_default_constructible;

#ifndef MPT_DOXYGEN_WARD
    template <class T>
    concept IsDefaultConstructible = std::is_default_constructible_v<T>;

    template <class T0, class... T>
    requires IsDefaultConstructible<
        T0> struct first_object_default_constructible<T0, T...> {
      using type = T0;
    };

    template <class T0, class... T>
    requires(!IsDefaultConstructible<
             T0>) struct first_object_default_constructible<T0, T...> {
      using type = typename first_object_default_constructible<T...>::type;
    };
#endif

    /// First class in the template argument list that is default-constructible
    template <class... T>
    using first_object_default_constructible_t =
        typename first_object_default_constructible<T...>::type;
  } // namespace detail

  /*!\brief Represent an object that can hold any of a set of types, saving the
    index of the type

    The behaviour is quite similar to std::variant, but in this case the size of
    the object is not determined by the size of the biggest object from the
    template argument list. The value is stored on a std::any object and the
    type index is saved as an integer. The actual value can be accessed using
    mpt::visit_typed_any (similarly to std::variant).

    This class is meant to be used when it is needed to have an object that
    might be any of a set of types that have very different size in memory.

    It is not recommended to use this class on lightweight classes stored in
    large containers due to the cost of having an additional integer saving the
    type of each of them. If all of them are of the same type, it is probably
    better to use std::variant since all the containers store a pointer and an
    integer (i.e. are of the same size in memory).
   */
  template <class IndexType, class... Type>
  requires std::is_integral_v<IndexType> &&mpt::UniqueTemplateArguments<Type...>
      &&mpt::NonEmptyTemplateArguments<Type...> class basic_typed_any {

    /// Object to build if using the default constructor
    using default_value_type =
        detail::first_object_default_constructible_t<Type...>;

  public:
    /// Type for the index defining the actual type being stored
    using index_type = IndexType;

    // Default constructors and assignment operators
    basic_typed_any() = default;
    basic_typed_any(basic_typed_any const &) = default;
    basic_typed_any(basic_typed_any &&) = default;
    basic_typed_any &operator=(basic_typed_any const &) = default;
    basic_typed_any &operator=(basic_typed_any &&) = default;

    /// Initialize the object from a value type, which must be in the template
    /// argument list
    template <class T>
    requires mpt::HasType<T, Type...> constexpr basic_typed_any(T const &t)
        : m_value{t}, m_type_index{mpt::type_index_v<T, Type...>} {}
    /// Initialize the object from a value type, which must be in the template
    /// argument list
    template <class T>
    requires mpt::HasType<T, Type...> constexpr basic_typed_any(T &&t)
        : m_value{std::forward<T>(t)}, m_type_index{
                                           mpt::type_index_v<T, Type...>} {}
    /// Assign the contained value another value that must be in the template
    /// argument list
    template <class T>
    requires mpt::HasType<T, Type...> constexpr basic_typed_any &
    operator=(T const &t) {
      m_value = t;
      m_type_index = mpt::type_index_v<T, Type...>;
      return *this;
    }
    /// Assign the contained value another value that must be in the template
    /// argument list
    template <class T>
    requires mpt::HasType<T, Type...> constexpr basic_typed_any &
    operator=(T &&t) {
      m_value = std::forward<T>(t);
      m_type_index = mpt::type_index_v<T, Type...>;
      return *this;
    }

    /// Return the contained std::any object
    constexpr auto &value() { return m_value; }
    /// Return the contained std::any object
    constexpr auto const &value() const { return m_value; }
    /// Return the index of the contained type
    constexpr auto type_index() const { return m_type_index; }

  protected:
    /// Any object this class accepts
    std::any m_value = default_value_type{};
    /// Index of the current type
    index_type m_type_index = mpt::type_index_v<default_value_type, Type...>;
  };

  namespace detail {

    /// Access a value on a std::any object
    template <class Function, class T>
    auto access(Function &&function, std::any &a) {
      return function(std::any_cast<T>(a));
    }

    /// Access a value on a std::any object
    template <class Function, class T>
    auto access(Function &&function, std::any const &a) {
      return function(std::any_cast<T const>(a));
    }

    /// Access a value on a std::any object
    template <class Function, class T>
    auto access(Function &&function, std::any &&a) {
      return function(std::any_cast<T>(std::move(a)));
    }

    /// Requirements for any accessor to a \ref mpt::basic_typed_any object
    template <class Function, class... T>
    concept Accessor = (mpt::NonEmptyTemplateArguments<T...> &&
                        mpt::are_same_v<decltype(std::invoke(
                            std::declval<Function>(), std::declval<T>()))...>);

    /// Define an array of functions to work with \ref mpt::basic_typed_any
    template <class TypedAny, class Function> struct accessors;

    /// Specialization by reference
    template <class Function, class IndexType, class... T>
    requires Accessor<Function, T...> struct accessors<
        basic_typed_any<IndexType, T...> &, Function> {

      using output_type = decltype(std::invoke(
          std::declval<Function>(), std::declval<mpt::type_at_t<0, T...>>()));

      static constexpr std::array<output_type (*)(Function &&, std::any &),
                                  sizeof...(T)>
          value = {&access<Function, T>...};
    };

    /// Specialization for constant reference
    template <class Function, class IndexType, class... T>
    requires Accessor<Function, T...> struct accessors<
        basic_typed_any<IndexType, T...> const &, Function> {

      using output_type = decltype(
          std::invoke(std::declval<Function>(),
                      std::declval<mpt::type_at_t<0, T...> const &>()));

      static constexpr std::array<
          output_type (*)(Function &&, std::any const &), sizeof...(T)>
          value = {&access<Function, T const>...};
    };

    /// Specialization for move operations
    template <class Function, class IndexType, class... T>
    requires Accessor<Function, T...> struct accessors<
        basic_typed_any<IndexType, T...> &&, Function> {

      using output_type = decltype(std::invoke(
          std::declval<Function>(), std::declval<mpt::type_at_t<0, T...>>()));

      static constexpr std::array<output_type (*)(Function &&, std::any &&),
                                  sizeof...(T)>
          value = {&access<Function, T>...};
    };
  } // namespace detail

  /*!\brief Main definition a typed "any" object

    To use other index types see mpt::basic_typed_any.
  */
  template <class... Type>
  using typed_any = basic_typed_any<unsigned short int, Type...>;

  /// Apply a function on the given typed "any" object
  template <class Function, class TypedAny>
  auto visit_typed_any(Function &&function, TypedAny &&any) {

    static constexpr auto is_rvalue_reference =
        std::is_rvalue_reference_v<decltype(any)>;

    static constexpr auto accessors =
        std::conditional_t<is_rvalue_reference,
                           detail::accessors<TypedAny &&, Function>,
                           detail::accessors<TypedAny, Function>>::value;

    if constexpr (is_rvalue_reference)
      return accessors[any.type_index()](std::forward<Function>(function),
                                         std::move(any.value()));
    else
      return accessors[any.type_index()](std::forward<Function>(function),
                                         any.value());
  }
} // namespace mpt
