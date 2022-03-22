/*!\file soa.hpp

  Implement types defining containers with a struct-of-arrays memory
  layout.
  These objects are meant to be used when aiming to use vectorization
  in calculations.
  The container accessors return proxies that modify the associated
  container if any of their values are modified.

  Vectors have a different structure depending whether it refers
  to a single field (containing only one value) or several fields
  (with several values).
  The objects returned by the accessor operators act as proxies.
  This means that a modification to the returned values implies a
  modification of the values stored in the vector.
  To obtain the corresponding value one can call to \ref
  mpt::soa_proxy_to_value.
 */
#pragma once
#include "mpt/types.hpp"
#include <tuple>
#include <vector>

namespace mpt {

#ifndef MPT_DOXYGEN_WARD
  template <class T, class Allocator> struct field;

  template <class... Fields> struct composite_field;

  template <class...> class soa_value;

  template <class, class> class soa_proxy;

  template <class, class> class soa_const_proxy;

  template <class, class> class soa_iterator;

  template <class, class> class soa_const_iterator;

  template <class... Fields> class soa_vector;

  template <class... Containers> class soa_zip;

  template <class... Containers> class soa_const_zip;
#endif

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

  /*!\brief A field refering to a container of single value-types

    Fields must be built inheriting from this class via
    \code{.cpp}
    struct position : mpt::field<float> { };
    \endcode
   */
  template <class T, class Allocator = std::allocator<T>> struct field {
    using field_type = field<T, Allocator>;
    using value_type = T;
    using allocator_type = Allocator;
  };

  /*!\brief A field that is composed by other fields

    Fields must be built inheriting from this class via
    \code{.cpp}
    struct x : mpt::field<float> { };
    struct y : mpt::field<float> { };
    struct z : mpt::field<float> { };
    struct position : mpt::composite_field<x, y, z> { };
    \endcode

    It is also possible to declare fields that depend on other
    composite fields.
   */
  template <class... Fields>
  requires(IsField<Fields> &&...) struct composite_field<Fields...> {
    using field_type = composite_field<Fields...>;
    template <class Container>
    using proxy = soa_proxy<Container, mpt::types<Fields...>>;
    template <class Container>
    using const_proxy = soa_const_proxy<Container, mpt::types<Fields...>>;
    using value_type = soa_value<Fields...>;
  };

  namespace {

    /// Helper struct to determine the type of a container given the field type
    template <class FieldType> struct resolve_soa_vector_type;

    template <class T, class Allocator>
    struct resolve_soa_vector_type<field<T, Allocator>> {
      using type = soa_vector<field<T, Allocator>>;
    };

    template <class... Fields>
    struct resolve_soa_vector_type<composite_field<Fields...>> {
      using type = soa_vector<Fields...>;
    };

    /// Determines the type of a container given the field type
    template <class FieldType>
    using resolve_soa_vector_type_t =
        typename resolve_soa_vector_type<FieldType>::type;

    /// Determines the index of the container with the given field
    template <std::size_t I, class Field, class... Containers>
    struct container_index_for_field_impl;

    template <std::size_t I, class Field, class First, class... V>
    requires mpt::templated_object_has_type_v<
        Field,
        typename First::
            fields_type> struct container_index_for_field_impl<I, Field, First,
                                                               V...> {
      static constexpr auto value = I;
    };

    template <std::size_t I, class Field, class First, class... C>
    requires(!mpt::templated_object_has_type_v<
             Field,
             typename First::
                 fields_type>) struct container_index_for_field_impl<I, Field,
                                                                     First,
                                                                     C...> {
      static constexpr auto value =
          container_index_for_field_impl<I + 1, Field, C...>::value;
    };
#ifndef MPT_DOXYGEN_WARD
    /// Determines the index of the container with the given field
    template <class Field, class... Containers>
    struct container_index_for_field {
      static constexpr auto value =
          container_index_for_field_impl<0, Field, Containers...>::value;
    };
#endif
    /// Index of the container with the given field
    template <class Field, class... Containers>
    static constexpr auto container_index_for_field_v =
        container_index_for_field<Field, Containers...>::value;

    /// Determines the subcontainer type associated to the given field
    template <class Container, class Field> struct container_type_for_field;

    template <class... F, class Field>
    requires HasType<Field, F...> struct container_type_for_field<
        soa_vector<F...>, Field> {
      using type = resolve_soa_vector_type_t<typename Field::field_type>;
    };

    template <class... Containers, class Field>
    struct container_type_for_field<soa_zip<Containers...>, Field> {
      using type = typename container_type_for_field<
          mpt::type_at_t<container_index_for_field_v<Field, Containers...>,
                         Containers...>,
          Field>::type;
    };

    template <class... Containers, class Field>
    struct container_type_for_field<soa_const_zip<Containers...>, Field> {
      using type = typename container_type_for_field<
          mpt::type_at_t<container_index_for_field_v<Field, Containers...>,
                         Containers...>,
          Field>::type;
    };

    /// Subcontainer type associated to the given field
    template <class Container, class Field>
    using container_type_for_field_t =
        typename container_type_for_field<Container, Field>::type;

    /// Determine the types of the values returned via the "get" accessors
    template <class Field> struct resolve_reference_types;

    template <class Field>
    requires IsBasicField<Field> struct resolve_reference_types<Field> {
      template <class> using reference_type = typename Field::value_type &;
      template <class>
      using const_reference_type = typename Field::value_type const &;
    };

    template <class Field>
    requires IsCompositeField<Field> struct resolve_reference_types<Field> {
      template <class Container>
      using reference_type = typename Field::template proxy<
          container_type_for_field_t<Container, Field>>;
      template <class Container>
      using const_reference_type = typename Field::template const_proxy<
          container_type_for_field_t<Container, Field>>;
    };

    /// Reference type for the given field
    template <class Container, class Field>
    using resolve_reference_type_t = typename resolve_reference_types<
        Field>::template reference_type<Container>;

    /// Constant reference type for the given field
    template <class Container, class Field>
    using resolve_const_reference_type_t = typename resolve_reference_types<
        Field>::template const_reference_type<Container>;

    /// Helper struct to access elements on a container
    template <class Field, class Container> struct get_element_t {

      using container_type = Container;
      using size_type = typename container_type::size_type;

      resolve_reference_type_t<container_type, Field>
      operator()(container_type &cont, size_type index) const {
        return cont.template get<Field>(index);
      }

      resolve_const_reference_type_t<container_type, Field>
      operator()(container_type const &cont, size_type index) const {
        return cont.template get<Field>(index);
      }
    };

    template <class Field>
    requires IsBasicField<Field> struct get_element_t<Field,
                                                      soa_vector<Field>> {

      using container_type = soa_vector<Field>;
      using size_type = typename container_type::size_type;

      resolve_reference_type_t<container_type, Field>
      operator()(container_type &cont, size_type index) const {
        return cont.at(index);
      }

      resolve_const_reference_type_t<container_type, Field>
      operator()(container_type const &cont, size_type index) const {
        return cont.at(index);
      }
    };

    /// Get the reference object for the given field
    template <class Field, class Container>
    auto get_element(Container &cont, typename Container::size_type index)
        -> decltype(get_element_t<Field, Container>{}(cont, index)) {
      return get_element_t<Field, Container>{}(cont, index);
    }

    /// Get the constant reference object for the given field
    template <class Field, class Container>
    auto get_element(Container const &cont, typename Container::size_type index)
        -> decltype(get_element_t<Field, Container>{}(cont, index)) {
      return get_element_t<Field, Container>{}(cont, index);
    }
  } // namespace

  /// Base of any proxy to SOA container values
  template <class DerivedProxy, class ContainerType> class base_soa_proxy {

    using derived_proxy_type = DerivedProxy;

  public:
    /// Type of the container that this class is a proxy of
    using container_type = ContainerType;
    /// Type of the size
    using size_type = typename container_type::size_type;

    // No default constructor
    base_soa_proxy() = delete;
    base_soa_proxy(base_soa_proxy const &) = default;
    base_soa_proxy(base_soa_proxy &&) = default;

    /// Build the class from a pointer to a container and an index
    base_soa_proxy(container_type *container, size_type index)
        : m_ptr{container}, m_index{index} {}

  protected:
    /// Pointer to the SOA container
    container_type *m_ptr = nullptr;
    /// Index associated to the proxy
    size_type m_index = 0;
  };

  /*!\brief Associated value of a struct-of-arrays layout element
   */
  template <class... Fields>
  class soa_value : protected std::tuple<typename Fields::value_type...> {

    using base_class_type = std::tuple<typename Fields::value_type...>;

  public:
    /// The set of fields used
    using fields_type = mpt::types<Fields...>;
    /// The reference type
    template <class Container, class FieldsSet>
    using proxy_type = soa_proxy<Container, FieldsSet>;
    /// The constant reference type
    template <class Container, class FieldsSet>
    using const_proxy_type = soa_const_proxy<Container, FieldsSet>;

    /// Constructors inherited from \ref std::tuple
    using base_class_type::base_class_type;
    /// Constructor from a proxy
    template <class Container, class FieldsSet>
    soa_value(proxy_type<Container, FieldsSet> const &other)
        : base_class_type{other.template get<Fields>()...} {}
    /// Constructor from a proxy
    template <class Container, class FieldsSet>
    soa_value(proxy_type<Container, FieldsSet> &&other)
        : base_class_type{std::move(other.template get<Fields>())...} {}
    /// Constructor from a constant proxy
    template <class Container, class FieldsSet>
    soa_value(const_proxy_type<Container, FieldsSet> const &other)
        : base_class_type{other.template get<Fields>()...} {}
    /// Assignment from a proxy
    template <class Container, class FieldsSet>
    soa_value &operator=(proxy_type<Container, FieldsSet> const &other) {
      ((this->template get<Fields>() = other.template get<Fields>()), ...);
      return *this;
    }
    /// Assignment from a proxy
    template <class Container, class FieldsSet>
    soa_value &operator=(proxy_type<Container, FieldsSet> &&other) {
      ((this->template get<Fields>() = std::move(other.template get<Fields>())),
       ...);
      return *this;
    }
    /// Assignment from a constant proxy
    template <class Container, class FieldsSet>
    soa_value &operator=(const_proxy_type<Container, FieldsSet> const &other) {
      ((this->template get<Fields>() = other.template get<Fields>()), ...);
      return *this;
    }
    /// Get the value of a field
    template <class F> auto &get() {
      return std::get<mpt::templated_object_type_index_v<F, fields_type>>(
          *this);
    }
    /// Get the value of a field
    template <class F> auto const &get() const {
      return std::get<mpt::templated_object_type_index_v<F, fields_type>>(
          *this);
    }
  };

  namespace {

    /// Determine the SOA value type for the given field
    template <class Field> struct soa_value_for_field;

    template <class... Fields>
    struct soa_value_for_field<composite_field<Fields...>> {
      using type = soa_value<Fields...>;
    };

    template <class Field> struct soa_value_for_field<field<Field>> {
      using type = typename Field::value_type;
    };

    /// Determine the SOA value type for the given field
    template <class Field>
    using soa_value_for_field_t = typename soa_value_for_field<Field>::type;

    /// Make an SOA value for the given field
    template <class Field, class... T>
    requires IsField<Field> soa_value_for_field_t<typename Field::field_type>
    make_soa_value_impl(T &&... v) {
      return {std::forward<T>(v)...};
    }

    /// Functor that converts proxies to values
    template <class Container, class TypeSet> struct soa_proxy_to_value_t;

    template <class Container, class... Fields>
    struct soa_proxy_to_value_t<Container, mpt::types<Fields...>> {

      soa_value<Fields...> operator()(
          soa_proxy<Container, mpt::types<Fields...>> const &proxy) const {
        return proxy;
      }

      soa_value<Fields...>
      operator()(soa_const_proxy<Container, mpt::types<Fields...>> const &proxy)
          const {
        return proxy;
      }
    };
  } // namespace

  /// Make an SOA value for the given field
  template <class Field, class... T>
  requires IsField<Field> auto make_soa_value(T &&... v) {
    return make_soa_value_impl<Field>(std::forward<T>(v)...);
  }

  /// Transform the given proxy to a value type
  template <class T> auto soa_proxy_to_value(T const &value) { return value; }

  /// Transform the given proxy to a value type
  template <class Container, class TypesSet>
  auto soa_proxy_to_value(soa_proxy<Container, TypesSet> const &proxy) {
    return soa_proxy_to_value_t<Container, TypesSet>{}(proxy);
  }

  /// Transform the given proxy to a value type
  template <class Container, class TypesSet>
  auto soa_proxy_to_value(soa_const_proxy<Container, TypesSet> const &proxy) {
    return soa_proxy_to_value_t<Container, TypesSet>{}(proxy);
  }

  /*!\brief Constant proxy for SOA container values.

    This type of proxy acts as a \ref mpt::soa_value object, but referring to
    values in a SOA container. The construction of this object is limited
    to copy and move constructors (where the assignment of values is
    not done).
   */
  template <class Container, class... Fields>
  requires(IsField<Fields>
               &&...) class soa_const_proxy<Container, mpt::types<Fields...>>
      : base_soa_proxy<soa_const_proxy<Container, mpt::types<Fields...>>,
                       Container const> {

  public:
    /// Type of the container
    using container_type = Container;
    /// The set of fields used
    using fields_type = mpt::types<Fields...>;
    /// Base class
    using base_class_type =
        base_soa_proxy<soa_const_proxy<container_type, fields_type>,
                       container_type const>;

    /// All constructors are implemented except for the empty constructor
    using base_class_type::base_class_type;

    /// A constant proxy can be built from a proxy when this is constant
    soa_const_proxy(soa_proxy<container_type, fields_type> const &other)
        : base_class_type{other.m_ptr, other.m_index} {}

    /// Get the value of a field
    template <class F>
    resolve_const_reference_type_t<container_type, F> get() const {
      return this->m_ptr->template get<F>(this->m_index);
    }
  };

  /*!\brief Proxy for an element of a container

    A proxy acts as a \ref mpt::soa_value object, but any modification
    (including assignment) will modify the associated contents in
    the referenced container. The construction of this object is limited
    to copy and move constructors (where the assignment of values is
    not done).
   */
  template <class Container, class... Fields>
  class soa_proxy<Container, mpt::types<Fields...>>
      : base_soa_proxy<soa_proxy<Container, mpt::types<Fields...>>, Container> {

  public:
    /// Type of the container
    using container_type = Container;
    /// The set of fields used
    using fields_type = mpt::types<Fields...>;
    /// Base class
    using base_class_type =
        base_soa_proxy<soa_proxy<container_type, fields_type>, container_type>;

    /// All constructors are implemented except for the empty constructor
    using base_class_type::base_class_type;

    /// Proxies that are const return constant proxies on access
    friend class soa_const_proxy<container_type, fields_type>;

    /// Assign the elements of the container from a value
    soa_proxy &operator=(soa_value<Fields...> const &other) {
      ((this->template get<Fields>() = other.template get<Fields>()), ...);
      return *this;
    }
    /// Assign the elements of the container from a value
    soa_proxy &operator=(soa_value<Fields...> &&other) {
      ((this->template get<Fields>() = std::move(other.template get<Fields>())),
       ...);
      return *this;
    }
    /// Assign the elements of the container from another proxy
    soa_proxy &operator=(soa_proxy const &other) {
      ((this->template get<Fields>() = other.template get<Fields>()), ...);
      return *this;
    }
    /// Assign the elements of the container from another proxy
    soa_proxy &operator=(soa_proxy &&other) {
      ((this->template get<Fields>() = std::move(other.template get<Fields>())),
       ...);
      return *this;
    }
    /// Assign the elements of the container from another proxy
    soa_proxy &
    operator=(soa_const_proxy<container_type, fields_type> const &other) {
      ((this->template get<Fields>() = other.template get<Fields>()), ...);
      return *this;
    }
    /// Get the value of a field
    template <class F> resolve_reference_type_t<container_type, F> get() {
      return this->m_ptr->template get<F>(this->m_index);
    }
    /// Get the value of a field
    template <class F>
    resolve_const_reference_type_t<container_type, F> get() const {
      return this->m_ptr->template get<F>(this->m_index);
    }
  };

  /// Base class of any SOA iterator
  template <class Iterator, class ContainerType> class base_soa_iterator {

  public:
    /// Actual derived iterator
    using derived_iterator_type = Iterator;
    /// Type returned when subtracting iterators
    using difference_type = std::ptrdiff_t;
    /// Type of the container
    using container_type = ContainerType;
    /// Size type
    using size_type = typename container_type::size_type;

    /// An iterator must always have a reference to the container
    base_soa_iterator() = delete;
    /// Copy constructor
    base_soa_iterator(base_soa_iterator const &) = default;
    /// Move constructor
    base_soa_iterator(base_soa_iterator &&) = default;
    /// Copy assignment
    base_soa_iterator &operator=(base_soa_iterator const &) = default;
    /// Move assignment
    base_soa_iterator &operator=(base_soa_iterator &&) = default;
    /// Constructor from the reference to the container and the index
    base_soa_iterator(container_type *container, size_type index)
        : m_ptr{container}, m_index{index} {}

    auto operator==(derived_iterator_type const &other) {
      return other.m_ptr == m_ptr && other.m_index == m_index;
    }

    auto operator!=(derived_iterator_type const &other) {
      return !(*this == other);
    }

    auto operator>(derived_iterator_type const &other) {
      return m_index > other.m_index;
    }

    auto operator<(derived_iterator_type const &other) {
      return m_index < other.m_index;
    }

    auto operator>=(derived_iterator_type const &other) {
      return m_index >= other.m_index;
    }

    auto operator<=(derived_iterator_type const &other) {
      return m_index <= other.m_index;
    }

    difference_type operator-(derived_iterator_type const &other) {
      return m_index - other.m_index;
    }

    derived_iterator_type operator+(difference_type n) {
      return {m_ptr, m_index + n};
    }

    derived_iterator_type operator-(difference_type n) {
      return {m_ptr, m_index - n};
    }

    derived_iterator_type &operator+=(difference_type n) {
      m_index += n;
      return *static_cast<derived_iterator_type *>(this);
    }

    derived_iterator_type &operator-=(difference_type n) {
      m_index -= n;
      return *static_cast<derived_iterator_type *>(this);
    }

    derived_iterator_type &operator++() {
      ++m_index;
      return *static_cast<derived_iterator_type *>(this);
    }

    derived_iterator_type operator++(int) {
      derived_iterator_type copy{*this};
      ++m_index;
      return copy;
    }

    derived_iterator_type &operator--() {
      --m_index;
      return *static_cast<derived_iterator_type *>(this);
    }

    derived_iterator_type operator--(int) {
      derived_iterator_type copy{*this};
      --m_index;
      return copy;
    }

  protected:
    /// Pointer to the container
    container_type *m_ptr = nullptr;
    /// Index in the container
    size_type m_index = 0;
  };

  /*!\brief Iterator over a container with a struct-of-arrays memory layout
   */
  template <class Container, class... Fields>
  class soa_iterator<Container, mpt::types<Fields...>>
      : public base_soa_iterator<soa_iterator<Container, mpt::types<Fields...>>,
                                 Container> {

  public:
    /// Type of the container
    using container_type = Container;
    /// The set of fields used
    using fields_type = mpt::types<Fields...>;
    /// Base class
    using base_class_type =
        base_soa_iterator<soa_iterator<Container, fields_type>, container_type>;
    /// Type of the proxy
    using reference_type =
        std::conditional_t<(sizeof...(Fields) > 1u),
                           soa_proxy<container_type, fields_type>,
                           typename mpt::type_at_t<0, Fields...>::value_type &>;
    /// Type of the constant proxy
    using const_reference_type = std::conditional_t<
        (sizeof...(Fields) > 1u), soa_const_proxy<container_type, fields_type>,
        typename mpt::type_at_t<0, Fields...>::value_type const &>;

    /// Size type
    using size_type = std::size_t;

    /// All constructors are implemented except for the empty constructor
    using base_class_type::base_class_type;

    /// Return a proxy to the current values
    reference_type operator*() {
      if constexpr (sizeof...(Fields) > 1u)
        return {this->m_ptr, this->m_index};
      else
        return this->m_ptr->at(this->m_index);
    }
    /// Return a constant proxy to the current values
    const_reference_type operator*() const {
      if constexpr (sizeof...(Fields) > 1u)
        return {this->m_ptr, this->m_index};
      else
        return this->m_ptr->at(this->m_index);
    }
  };

  /*!\brief Constant iterator over a container with a SOA memory layout
   */
  template <class Container, class... Fields>
  class soa_const_iterator<Container, mpt::types<Fields...>>
      : public base_soa_iterator<
            soa_const_iterator<Container, mpt::types<Fields...>>,
            Container const> {

  public:
    /// Type of the container
    using container_type = Container;
    /// The set of fields used
    using fields_type = mpt::types<Fields...>;
    /// Base class
    using base_class_type =
        base_soa_iterator<soa_const_iterator<container_type, fields_type>,
                          container_type const>;

    /// Type of the constant proxy
    using const_reference_type = std::conditional_t<
        (sizeof...(Fields) > 1), soa_const_proxy<container_type, fields_type>,
        typename mpt::type_at_t<0, Fields...>::value_type const &>;

    /// Size type
    using size_type = std::size_t;

    /// All constructors are implemented except for the empty constructor
    using base_class_type::base_class_type;

    /// Return a constant proxy to the current values
    const_reference_type operator*() const {
      if constexpr (sizeof...(Fields) > 1)
        return {this->m_ptr, this->m_index};
      else
        return this->m_ptr->at(this->m_index);
    }
  };

  /*!\brief A container with a struct-of-arrays memory layout

    Specialization for a container with a single field.
 */
  template <class Field>
  requires IsBasicField<Field> class soa_vector<Field>
      : protected std::vector<typename Field::value_type,
                              typename Field::allocator_type> {

  protected:
    using container_type = soa_vector<Field>;

  public:
    using fields_type = mpt::types<Field>;
    using iterator_type = soa_iterator<soa_vector, fields_type>;
    using const_iterator_type = soa_const_iterator<soa_vector, fields_type>;
    using size_type = std::size_t;
    using value_type = typename Field::value_type;
    using reference_type = value_type &;
    using const_reference_type = value_type const &;

    using base_class_type =
        std::vector<typename Field::value_type, typename Field::allocator_type>;

    soa_vector() = default;
    soa_vector(soa_vector const &) = default;
    soa_vector(soa_vector &&) = default;
    soa_vector &operator=(soa_vector const &) = default;
    soa_vector &operator=(soa_vector &&) = default;

    /// Construct a vector with \a n elements
    soa_vector(size_type n) : base_class_type(n) {}

    reference_type at(size_type index) { return base_class_type::at(index); }

    const_reference_type at(size_type index) const {
      return base_class_type::at(index);
    }

    iterator_type begin() { return {this, 0u}; }

    iterator_type end() { return {this, size()}; }

    const_iterator_type begin() const { return {this, 0u}; }

    const_iterator_type end() const { return {this, size()}; }

    const_iterator_type cbegin() const { return {this, 0u}; }

    const_iterator_type cend() const { return {this, size()}; }
    void reserve(size_type n) { base_class_type::reserve(n); };

    void resize(size_type n) { base_class_type::resize(n); };

    size_type size() const { return base_class_type::size(); };
  };

  /*!\brief A container with a struct-of-arrays memory layout

    Specialization for a container containing several fields.
   */
  template <class... Fields>
      requires NonEmptyTemplateArguments<Fields...> &&
      (IsField<Fields> && ...) class soa_vector<Fields...> {

    static_assert(!mpt::has_repeated_template_arguments_v<Fields...>,
                  "Repeated fields in SOA vector definition");

    using container_type = soa_vector<Fields...>;

  public:
    /// The set of fields used
    using fields_type = mpt::types<Fields...>;
    /// Iterator type
    using iterator_type = soa_iterator<soa_vector, fields_type>;
    /// Constant iterator type
    using const_iterator_type = soa_const_iterator<soa_vector, fields_type>;
    /// Reference type
    using proxy_type = soa_proxy<soa_vector, fields_type>;
    /// Constant reference type
    using const_proxy_type = soa_const_proxy<soa_vector, fields_type>;
    /// Size type
    using size_type = std::size_t;
    /// Value type
    using value_type = soa_value<Fields...>;

    /// Build an empty vector
    soa_vector() = default;
    /// Copy constructor
    soa_vector(soa_vector const &) = default;
    /// Move constructor
    soa_vector(soa_vector &&) = default;
    /// Copy assignment
    soa_vector &operator=(soa_vector const &) = default;
    /// Move assignment
    soa_vector &operator=(soa_vector &&) = default;

    /// Construct a vector with \a n elements
    soa_vector(size_type n)
        : m_containers{
              resolve_soa_vector_type_t<typename Fields::field_type>(n)...} {}

    /// Access the element at the given index
    proxy_type at(size_type index) { return {this, index}; }

    /// Access the element at the given index
    const_proxy_type at(size_type index) const { return {this, index}; }

    /// Iterator pointing to the beginning of the vector
    iterator_type begin() { return {this, 0u}; }

    /// Iterator pointing to the end of the vector
    iterator_type end() { return {this, size()}; }

    /// Constant iterator pointing to the beginning of the vector
    const_iterator_type begin() const { return {this, 0u}; }

    /// Constant iterator pointing to the end of the vector
    const_iterator_type end() const { return {this, size()}; }

    /// Constant iterator pointing to the beginning of the vector
    const_iterator_type cbegin() const { return {this, 0u}; }

    /// Constant iterator pointing to the end of the vector
    const_iterator_type cend() const { return {this, size()}; }

    /// Access the proxy of a field at the given index
    template <class F>
    resolve_reference_type_t<container_type, F> get(size_type index) {
      return std::get<mpt::templated_object_type_index_v<F, fields_type>>(
                 m_containers)
          .at(index);
    }
    /// Access the constant proxy of a field at the given index
    template <class F>
    resolve_const_reference_type_t<container_type, F>
    get(size_type index) const {
      return std::get<mpt::templated_object_type_index_v<F, fields_type>>(
                 m_containers)
          .at(index);
    }

    /// Reserve memory for the given number of elements
    void reserve(size_type n) {
      reserve(n, std::make_index_sequence<sizeof...(Fields)>());
    }

    /// Change the size of the vector
    void resize(size_type n) {
      resize(n, std::make_index_sequence<sizeof...(Fields)>());
    }

    /// Size of the vector
    size_type size() const { return std::get<0>(m_containers).size(); }

  private:
    /// Containers of the SOA vector
    std::tuple<resolve_soa_vector_type_t<typename Fields::field_type>...>
        m_containers;

#ifndef MPT_DOXYGEN_WARD
    template <std::size_t... I>
    void reserve(size_type n, std::index_sequence<I...>) {
      (std::get<I>(m_containers).reserve(n), ...);
    }

    template <std::size_t... I>
    void resize(size_type n, std::index_sequence<I...>) {
      (std::get<I>(m_containers).resize(n), ...);
    }
#endif
  };

  namespace {

    /// Check if two sets have different fields
    template <class...> struct have_different_fields;

    template <class... U, class... V, class... F>
    struct have_different_fields<mpt::types<U...>, mpt::types<V...>, F...> {
      static constexpr auto value =
          (!(has_type_v<U, V...> || ...) &&
           (have_different_fields<mpt::types<U...>, F...>::value &&
            have_different_fields<mpt::types<V...>, F...>::value));
    };

    template <class... U, class... V>
    struct have_different_fields<mpt::types<U...>, mpt::types<V...>> {
      static constexpr auto value = !(has_type_v<U, V...> || ...);
    };

    /// Whether the given packs of fields have intersecting fields or not
    template <class... FieldPacks>
    static constexpr auto have_different_fields_v =
        have_different_fields<FieldPacks...>::value;

    /// Base class for SOA zip objects
    template <class... Containers>
    requires NonEmptyTemplateArguments<Containers...> class base_soa_zip
        : protected std::tuple<Containers *...> {

      static_assert(
          have_different_fields_v<typename Containers::fields_type...>,
          "Attempt to make a zip of containers with clashing fields");

    public:
      /// Base class
      using base_class_type = std::tuple<Containers *...>;

      /// Size type
      using size_type = std::size_t;

      base_soa_zip() = delete;
      base_soa_zip(base_soa_zip const &) = default;
      base_soa_zip(base_soa_zip &&) = default;
      base_soa_zip &operator=(base_soa_zip const &) = default;
      base_soa_zip &operator=(base_soa_zip &&) = default;

      /// Size of the containers
      size_type size() const { return std::get<0>(*this)->size(); }

    protected:
      base_soa_zip(Containers *... ptrs) : base_class_type{ptrs...} {}
    };

    /// Build a concatenated set of fields
    template <class... FieldsPack> struct concatenated_field_types;

#ifndef MPT_DOXYGEN_WARD
    template <class... U, class... V, class... FieldsPack>
    struct concatenated_field_types<mpt::types<U...>, mpt::types<V...>,
                                    FieldsPack...> {
      using type =
          typename concatenated_field_types<mpt::types_set_t<U..., V...>,
                                            FieldsPack...>::type;
    };

    template <class... F> struct concatenated_field_types<mpt::types<F...>> {
      using type = mpt::types<F...>;
    };
#endif

    /// Concatenated set of fields
    template <class... FieldsPack>
    using concatenated_field_types_t =
        typename concatenated_field_types<FieldsPack...>::type;

    /// Build the value associated to the given set of types
    template <class TypesSet> struct concatenated_value_types;

#ifndef MPT_DOXYGEN_WARD
    template <class... F> struct concatenated_value_types<mpt::types<F...>> {
      using type = soa_value<F...>;
    };
#endif

    /// Value type associated to the given set of types
    template <class TypesSet>
    using concatenated_value_types_t =
        typename concatenated_value_types<TypesSet>::type;

    /// Build the value type associated to the given sets of field types
    template <class... FieldsPack>
    struct concatenated_value_types_from_field_sets {
      using type =
          concatenated_value_types_t<concatenated_field_types_t<FieldsPack...>>;
    };

    /// Value type associated to the given sets of field types
    template <class... FieldsPack>
    using concatenated_value_types_from_field_sets_t =
        typename concatenated_value_types_from_field_sets<FieldsPack...>::type;
  } // namespace

  /*!\brief A zip of containers

    This object stores pointers (without ownership) to containers and allows
    to iterate over them simultaneously.
   */
  template <class... Containers>
  class soa_zip : public base_soa_zip<Containers...> {

  public:
    /// Base class
    using base_class_type = base_soa_zip<Containers...>;
    /// Type of the container
    using container_type = soa_zip<Containers...>;

    /// Enable default constructors and assignments (empty/copy/move)
    using base_class_type::base_class_type;

    /// The set of fields used
    using fields_type =
        concatenated_field_types_t<typename Containers::fields_type...>;
    /// Iterator type
    using iterator_type = soa_iterator<soa_zip, fields_type>;
    /// Constant iterator type
    using const_iterator_type = soa_const_iterator<soa_zip, fields_type>;
    /// Reference type
    using proxy_type = soa_proxy<soa_zip, fields_type>;
    /// Constant reference type
    using const_proxy_type = soa_const_proxy<soa_zip, fields_type>;
    /// Value type
    using value_type = concatenated_value_types_from_field_sets_t<
        typename Containers::fields_type...>;
    /// Size type
    using size_type = base_class_type::size_type;

    /// Access the element at the given index
    proxy_type at(size_type index) { return {this, index}; }

    /// Access the element at the given index
    const_proxy_type at(size_type index) const { return {this, index}; }

    /// Iterator pointing to the beginning of the container
    iterator_type begin() { return {this, 0u}; }

    /// Iterator pointing to the end of the container
    iterator_type end() { return {this, this->size()}; }

    /// Constant iterator pointing to the beginning of the container
    const_iterator_type begin() const { return {this, 0u}; }

    /// Constant iterator pointing to the end of the container
    const_iterator_type end() const { return {this, this->size()}; }

    /// Constant iterator pointing to the beginning of the container
    const_iterator_type cbegin() const { return {this, 0u}; }

    /// Constant iterator pointing to the end of the container
    const_iterator_type cend() const { return {this, this->size()}; }

    /// Access the proxy of a field at the given index
    template <class F>
    resolve_reference_type_t<container_type, F> get(size_type index) {
      return get_element<F>(
          *std::get<container_index_for_field_v<F, Containers...>>(*this),
          index);
    }
    /// Access the constant proxy of a field at the given index
    template <class F>
    resolve_const_reference_type_t<container_type, F>
    get(size_type index) const {
      return get_element<F>(
          *std::get<container_index_for_field_v<F, Containers...>>(*this),
          index);
    }

    /// The only function that can build zip objects
    friend soa_zip make_soa_zip<Containers...>(Containers &...);

  protected:
    /// Constructor from the pointers to the containers
    soa_zip(Containers *... ptrs) : base_class_type{ptrs...} {}
  };

  /*!\brief A zip of constant containers

    This object stores pointers (without ownership) to containers and allows
    to iterate over them simultaneously.
   */
  template <class... Containers>
  class soa_const_zip : public base_soa_zip<Containers const...> {

  public:
    /// Base class
    using base_class_type = base_soa_zip<Containers const...>;
    /// Type of the container
    using container_type = soa_const_zip<Containers...>;
    /// Enable default constructors and assignments (empty/copy/move)
    using base_class_type::base_class_type;

    /// The set of fields used
    using fields_type =
        concatenated_field_types_t<typename Containers::fields_type...>;
    /// Constant iterator type
    using const_iterator_type = soa_const_iterator<soa_const_zip, fields_type>;
    /// Constant reference type
    using const_proxy_type = soa_const_proxy<soa_const_zip, fields_type>;
    /// Value type
    using value_type = concatenated_value_types_from_field_sets_t<
        typename Containers::fields_type...>;
    /// Size type
    using size_type = base_class_type::size_type;

    /// Access the element at the given index
    const_proxy_type at(size_type index) const { return {this, index}; }

    /// Constant iterator pointing to the beginning of the container
    const_iterator_type begin() const { return {this, 0u}; }

    /// Constant iterator pointing to the end of the container
    const_iterator_type end() const { return {this, this->size()}; }

    /// Constant iterator pointing to the beginning of the container
    const_iterator_type cbegin() const { return {this, 0u}; }

    /// Constant iterator pointing to the end of the container
    const_iterator_type cend() const { return {this, this->size()}; }

    /// Access the constant proxy of a field at the given index
    template <class F>
    resolve_const_reference_type_t<container_type, F>
    get(size_type index) const {
      return get_element<F>(
          *std::get<container_index_for_field_v<F, Containers...>>(*this),
          index);
    }

    /// The only function that can build zip objects
    friend soa_const_zip make_soa_zip<Containers...>(Containers const &...);

  protected:
    /// Constructor from the pointers to the constant containers
    soa_const_zip(Containers const *... ptrs) : base_class_type{ptrs...} {}
  };

  /// Create a zip of the given containers
  template <class... Containers>
  soa_zip<Containers...> make_soa_zip(Containers &... v) {
    return {&v...};
  }

  /// Create a constant zip of the given containers
  template <class... Containers>
  soa_const_zip<Containers...> make_soa_zip(Containers const &... v) {
    return {&v...};
  }
} // namespace mpt
