#pragma once
#include "mpt/types.hpp"
#include <tuple>
#include <vector>

namespace mpt {

  /// Define a basic field
  template <class T, class Allocator = std::allocator<T>> struct field {
    using field_type = field<T, Allocator>;
    using value_type = T;
    using allocator_type = Allocator;
  };

  /// Define a field that is composed by other fields
  template <class... Fields> struct composite_field {
    using field_type = composite_field<Fields...>;
  };

  /// Whether the given field type is basic
  template <class FieldType> struct is_basic_field_type : std::false_type {};

#ifndef MPT_DOXYGEN_WARD
  template <class T, class Allocator>
  struct is_basic_field_type<field<T, Allocator>> : std::true_type {};
#endif

  /// Whether the given field is a basic field
  template <class Field>
  struct is_basic_field : is_basic_field_type<typename Field::field_type> {};

  /// Whether the given field is a basic field
  template <class Field>
  static constexpr auto is_basic_field_v = is_basic_field<Field>::value;

  /// Whether the given field is a basic field
  template <class Field> concept IsBasicField = is_basic_field_v<Field>;

  /// Whether the given field type is a composite field type
  template <class FieldType>
  struct is_composite_field_type : std::false_type {};

#ifndef MPT_DOXYGEN_WARD
  template <class... Fields>
  struct is_composite_field_type<composite_field<Fields...>> : std::true_type {
  };
#endif

  /// Whether the given field is a composite field type
  template <class Field>
  struct is_composite_field
      : is_composite_field_type<typename Field::field_type> {};

  /// Whether the given field is a composite field type
  template <class Field>
  static constexpr auto is_composite_field_v = is_composite_field<Field>::value;

  /// Whether the given field is a composite field type
  template <class Field> concept IsCompositeField = is_composite_field_v<Field>;

  /// Whether the given type is a valid field type
  template <class T>
  struct is_field
      : std::conditional_t<is_basic_field_v<T> || is_composite_field_v<T>,
                           std::true_type, std::false_type> {};

  /// Whether the given type is a valid field type
  template <class T> static constexpr auto is_field_v = is_field<T>::value;

  /// Whether the given type is a valid field type
  template <class T> concept IsField = is_field_v<T>;

  /*!\brief A vector with a struct-of-arrays memory layout


   */
  template <class... Fields>
      requires NonEmptyTemplateArguments<Fields...> &&
      (IsField<Fields> && ...) class soa_vector;

  namespace {

    /// Helper struct to determine the type of a container given the field type
    template <class FieldType> struct resolve_soa_vector_type;

#ifndef MPT_DOXYGEN_WARD
    template <class T, class Allocator>
    struct resolve_soa_vector_type<field<T, Allocator>> {
      using type = soa_vector<field<T, Allocator>>;
    };

    template <class... Fields>
    struct resolve_soa_vector_type<composite_field<Fields...>> {
      using type = soa_vector<Fields...>;
    };
#endif

    /// Helper struct to determine the type of a container given the field type
    template <class FieldType>
    using resolve_soa_vector_type_t =
        typename resolve_soa_vector_type<FieldType>::type;
  } // namespace

#ifndef MPT_DOXYGEN_WARD
  template <class Field>
  requires IsBasicField<Field> class soa_vector<Field>
      : protected std::vector<typename Field::value_type,
                              typename Field::allocator_type> {

  protected:
    using base_class_type =
        std::vector<typename Field::value_type, typename Field::allocator_type>;

  public:
    using size_type = std::size_t;

    static constexpr auto number_of_fields = 1u;

    soa_vector() = default;
    soa_vector(soa_vector const &) = default;
    soa_vector(soa_vector &&) = default;
    soa_vector &operator=(soa_vector const &) = default;
    soa_vector &operator=(soa_vector &&) = default;

    soa_vector(size_type n) : base_class_type(n) {}

    void reserve(size_type n) { base_class_type::reserve(n); };

    void resize(size_type n) { base_class_type::resize(n); };

    size_type size() const { return base_class_type::size(); };
  };

  template <class... Fields>
      requires NonEmptyTemplateArguments<Fields...> &&
      (IsField<Fields> && ...) class soa_vector {

  public:
    using size_type = std::size_t;

    static constexpr auto number_of_fields = sizeof...(Fields);

    soa_vector() = default;
    soa_vector(soa_vector const &) = default;
    soa_vector(soa_vector &&) = default;
    soa_vector &operator=(soa_vector const &) = default;
    soa_vector &operator=(soa_vector &&) = default;

    soa_vector(size_type n)
        : m_containers{
              resolve_soa_vector_type_t<typename Fields::field_type>(n)...} {}

    void reserve(size_type n) {
      reserve(n, std::make_index_sequence<number_of_fields>());
    }

    void resize(size_type n) {
      resize(n, std::make_index_sequence<number_of_fields>());
    }

    size_type size() const { return std::get<0>(m_containers).size(); }

  private:
    std::tuple<resolve_soa_vector_type_t<typename Fields::field_type>...>
        m_containers;

    template <std::size_t... I>
    void reserve(size_type n, std::index_sequence<I...>) {
      (std::get<I>(m_containers).reserve(n), ...);
    }

    template <std::size_t... I>
    void resize(size_type n, std::index_sequence<I...>) {
      (std::get<I>(m_containers).resize(n), ...);
    }
  };
#endif
} // namespace mpt
