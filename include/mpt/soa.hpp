/*\file soa.hpp

  Implement types defining containers with a struct-of-arrays memory
  layout.
  These objects are meant to be used when aiming to use vectorization
  in calculations.
  The container accessors return proxies that modify the associated
  container if any of their values are modified.
 */
#pragma once
#include "mpt/types.hpp"
#include <tuple>
#include <vector>

namespace mpt {

#ifndef MPT_DOXYGEN_WARD
  template <class...> class soa_value;

  template <class...> class soa_proxy;

  template <class...> class soa_const_proxy;

  template <class, class...> class soa_iterator;

  template <class, class...> class soa_const_iterator;
#endif

  /// Define a basic field
  template <class T, class Allocator = std::allocator<T>> struct field {
    using field_type = field<T, Allocator>;
    using value_type = T;
    using allocator_type = Allocator;
  };

  /// Define a field that is composed by other fields
  template <class... Fields> struct composite_field {
    using field_type = composite_field<Fields...>;
    using proxy_type = soa_proxy<Fields...>;
    using const_proxy_type = soa_const_proxy<Fields...>;
    using value_type = soa_value<Fields...>;
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

    This object has a different structure depending whether it refers
    to a single field (containing only one value) or several fields
    (with several values).
    The objects returned by the accessor operators act as proxies.
    This means that a modification to the returned values implies a
    modification of the values stored in the vector.
    To obtain the corresponding value one can call to \ref
    mpt::soa_proxy_to_value.
   */
  template <class... Fields>
      requires NonEmptyTemplateArguments<Fields...> &&
      (IsField<Fields> && ...) class soa_vector;

  namespace {

    /// Determine the types of the values returned via the "get" accessors
    template <class Field> struct resolve_reference_types;

    template <class Field>
    requires IsBasicField<Field> struct resolve_reference_types<Field> {
      using reference_type = typename Field::value_type &;
      using const_reference_type = typename Field::value_type const &;
    };

    template <class Field>
    requires IsCompositeField<Field> struct resolve_reference_types<Field> {
      using reference_type = typename Field::proxy_type;
      using const_reference_type = typename Field::const_proxy_type;
    };

    /// Determine the types of the values returned via the "get" accessors
    template <class... Field>
    requires(
        IsField<Field> &&...) struct resolve_reference_types_for_fields_pack {
      using last_field_type = mpt::type_at_t<(sizeof...(Field) - 1), Field...>;
      using reference_type =
          typename resolve_reference_types<last_field_type>::reference_type;
      using const_reference_type = typename resolve_reference_types<
          last_field_type>::const_reference_type;
    };

    template <class... Field>
    using resolve_reference_type_for_fields_pack_t =
        typename resolve_reference_types_for_fields_pack<
            Field...>::reference_type;

    template <class... Field>
    using resolve_const_reference_type_for_fields_pack_t =
        typename resolve_reference_types_for_fields_pack<
            Field...>::const_reference_type;

    /// Base of any proxy
    template <class DerivedProxy, class VectorType> class base_soa_proxy {

      using derived_proxy_type = DerivedProxy;

    public:
      /// Type of the vector that this class is a proxy of
      using vector_type = VectorType;
      /// Type of the size
      using size_type = typename vector_type::size_type;

      // No default constructor
      base_soa_proxy() = delete;
      base_soa_proxy(base_soa_proxy const &) = default;
      base_soa_proxy(base_soa_proxy &&) = default;

      /// Build the class from a pointer to a vector and an index
      base_soa_proxy(vector_type *vector, size_type index)
          : m_ptr{vector}, m_index{index} {}

    protected:
      /// Pointer to the SOA vector
      vector_type *m_ptr = nullptr;
      /// Index associated to the proxy
      size_type m_index = 0;
    };
  } // namespace

  /*!\brief Associated value of a struct-of-arrays layout element
   */
  template <class... Fields>
  class soa_value : protected std::tuple<typename Fields::value_type...> {

    using base_class_type = std::tuple<typename Fields::value_type...>;

  public:
    using proxy_type = soa_proxy<Fields...>;
    using const_proxy_type = soa_const_proxy<Fields...>;

    /// Constructors inherited from \ref std::tuple
    using base_class_type::base_class_type;
    /// Constructor from a proxy
    soa_value(proxy_type const &other)
        : base_class_type{other.template get<Fields>()...} {}
    /// Constructor from a proxy
    soa_value(proxy_type &&other)
        : base_class_type{std::move(other.template get<Fields>())...} {}
    /// Constructor from a constant proxy
    soa_value(const_proxy_type const &other)
        : base_class_type{other.template get<Fields>()...} {}
    /// Assignment from a proxy
    soa_value &operator=(proxy_type const &other) {
      ((this->template get<Fields>() = other.template get<Fields>()), ...);
      return *this;
    }
    /// Assignment from a proxy
    soa_value &operator=(proxy_type &&other) {
      ((this->template get<Fields>() = std::move(other.template get<Fields>())),
       ...);
      return *this;
    }
    /// Assignment from a constant proxy
    soa_value &operator=(const_proxy_type const &other) {
      ((this->template get<Fields>() = other.template get<Fields>()), ...);
      return *this;
    }
    /// Get the value of a field
    template <class Field, class... Next> auto &get() {
      if constexpr (sizeof...(Next) == 0)
        return std::get<mpt::type_index_v<Field, Fields...>>(*this);
      else
        return std::get<mpt::type_index_v<Field, Fields...>>(*this)
            .template get<Next...>();
    }
    /// Get the value of a field
    template <class Field, class... Next> auto const &get() const {
      if constexpr (sizeof...(Next) == 0)
        return std::get<mpt::type_index_v<Field, Fields...>>(*this);
      else
        return std::get<mpt::type_index_v<Field, Fields...>>(*this)
            .template get<Next...>();
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
  } // namespace

  /// Make an SOA value for the given field
  template <class Field, class... T>
  requires IsField<Field> auto make_soa_value(T &&... v) {
    return make_soa_value_impl<Field>(std::forward<T>(v)...);
  }

  /// Transform the given proxy to a value type
  template <class... Fields>
  requires(IsField<Fields> &&...) soa_value<Fields...> soa_proxy_to_value(
      soa_proxy<Fields...> const &proxy) {
    return proxy;
  }

  /*!\brief Proxy that maintains constant the values of a container
   */
  template <class... Fields>
  class soa_const_proxy : base_soa_proxy<soa_const_proxy<Fields...>,
                                         soa_vector<Fields...> const> {

    using base_class_type =
        base_soa_proxy<soa_const_proxy<Fields...>, soa_vector<Fields...> const>;

  public:
    using base_class_type::base_class_type;

    using proxy_type = soa_proxy<Fields...>;

    /// A constant proxy can be built from a proxy when this is constant
    soa_const_proxy(proxy_type const &other)
        : base_class_type{other.m_ptr, other.m_index} {}

    /// Get the value of a field
    template <class... F>
    resolve_const_reference_type_for_fields_pack_t<F...> get() const {
      return this->m_ptr->template get<F...>(this->m_index);
    }
  };

  /*!\brief Proxy for an element of a container
   */
  template <class... Fields>
  class soa_proxy
      : base_soa_proxy<soa_proxy<Fields...>, soa_vector<Fields...>> {

    using base_class_type =
        base_soa_proxy<soa_proxy<Fields...>, soa_vector<Fields...>>;

  public:
    using value_type = soa_value<Fields...>;
    using const_proxy_type = soa_const_proxy<Fields...>;

    using base_class_type::base_class_type;

    /// Proxies that are const return constant proxies on access
    friend class soa_const_proxy<Fields...>;

    /// Assign the elements of the container from a value
    soa_proxy &operator=(value_type const &other) {
      ((this->template get<Fields>() = other.template get<Fields>()), ...);
      return *this;
    }
    /// Assign the elements of the container from a value
    soa_proxy &operator=(value_type &&other) {
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
    soa_proxy &operator=(const_proxy_type const &other) {
      ((this->template get<Fields>() = other.template get<Fields>()), ...);
      return *this;
    }
    /// Get the value of a field
    template <class... F> resolve_reference_type_for_fields_pack_t<F...> get() {
      return this->m_ptr->template get<F...>(this->m_index);
    }
    /// Get the value of a field
    template <class... F>
    resolve_const_reference_type_for_fields_pack_t<F...> get() const {
      return this->m_ptr->template get<F...>(this->m_index);
    }
  };

  namespace {

    /// Base class of any SOA iterator
    template <class Iterator, class VectorType> class base_soa_iterator {

    public:
      using derived_iterator_type = Iterator;
      using difference_type = std::ptrdiff_t;
      using vector_type = VectorType;
      using size_type = typename vector_type::size_type;

      base_soa_iterator() = delete;
      base_soa_iterator(base_soa_iterator const &) = default;
      base_soa_iterator(base_soa_iterator &&) = default;
      base_soa_iterator &operator=(base_soa_iterator const &) = default;
      base_soa_iterator &operator=(base_soa_iterator &&) = default;

      base_soa_iterator(vector_type *vector, size_type index)
          : m_ptr{vector}, m_index{index} {}

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
      vector_type *m_ptr = nullptr;
      size_type m_index = 0;
    };
  } // namespace

  /*!\brief Iterator over a vector with a struct-of-arrays memory layout
   */
  template <class F, class... Fields>
  class soa_iterator : public base_soa_iterator<soa_iterator<F, Fields...>,
                                                soa_vector<F, Fields...>> {

    using base_class_type =
        base_soa_iterator<soa_iterator<F, Fields...>, soa_vector<F, Fields...>>;

  public:
    /// Type of the proxy
    using reference_type =
        std::conditional_t<(sizeof...(Fields) > 0), soa_proxy<F, Fields...>,
                           typename F::value_type &>;
    /// Type of the constant proxy
    using const_reference_type =
        std::conditional_t<(sizeof...(Fields) > 0),
                           soa_const_proxy<F, Fields...>,
                           typename F::value_type const &>;

    using size_type = std::size_t;

    using base_class_type::base_class_type;

    /// Return a proxy to the current values
    reference_type operator*() {
      if constexpr (sizeof...(Fields) > 0)
        return {this->m_ptr, this->m_index};
      else
        return this->m_ptr->at(this->m_index);
    }
    /// Return a constant proxy to the current values
    const_reference_type operator*() const {
      if constexpr (sizeof...(Fields) > 0)
        return {this->m_ptr, this->m_index};
      else
        return this->m_ptr->at(this->m_index);
    }
  };

  /*!\brief Constant iterator over a vector with a SOA memory layout
   */
  template <class F, class... Fields>
  class soa_const_iterator
      : public base_soa_iterator<soa_const_iterator<F, Fields...>,
                                 soa_vector<F, Fields...> const> {

    using base_class_type = base_soa_iterator<soa_const_iterator<F, Fields...>,
                                              soa_vector<F, Fields...> const>;

  public:
    /// Type of the constant proxy
    using const_reference_type =
        std::conditional_t<(sizeof...(Fields) > 0),
                           soa_const_proxy<F, Fields...>,
                           typename F::value_type const &>;
    using size_type = std::size_t;

    using base_class_type::base_class_type;

    /// Return a constant proxy to the current values
    const_reference_type operator*() const {
      if constexpr (sizeof...(Fields) > 0)
        return {this->m_ptr, this->m_index};
      else
        return this->m_ptr->at(this->m_index);
    }
  };

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

  /*!\brief A vector with a struct-of-arrays memory layout

    Specialization for a vector with a single field.
 */
  template <class Field>
  requires IsBasicField<Field> class soa_vector<Field>
      : protected std::vector<typename Field::value_type,
                              typename Field::allocator_type> {

  protected:
    using base_class_type =
        std::vector<typename Field::value_type, typename Field::allocator_type>;

  public:
    using iterator_type = soa_iterator<Field>;
    using const_iterator_type = soa_const_iterator<Field>;
    using size_type = std::size_t;
    using value_type = typename Field::value_type;
    using reference_type = value_type &;
    using const_reference_type = value_type const &;

    static constexpr auto number_of_fields = 1u;

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

  /*!\brief A vector with a struct-of-arrays memory layout

    Specialization for a vector containing several fields.
   */
  template <class... Fields>
      requires NonEmptyTemplateArguments<Fields...> &&
      (IsField<Fields> && ...) class soa_vector {

  public:
    using iterator_type = soa_iterator<Fields...>;
    using const_iterator_type = soa_const_iterator<Fields...>;
    using proxy_type = soa_proxy<Fields...>;
    using const_proxy_type = soa_const_proxy<Fields...>;
    using size_type = std::size_t;
    using value_type = soa_value<Fields...>;

    static constexpr auto number_of_fields = sizeof...(Fields);

    soa_vector() = default;
    soa_vector(soa_vector const &) = default;
    soa_vector(soa_vector &&) = default;
    soa_vector &operator=(soa_vector const &) = default;
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
    template <class Field, class... Next>
    resolve_reference_type_for_fields_pack_t<Field, Next...>
    get(size_type index) {
      if constexpr (sizeof...(Next) == 0)
        return std::get<mpt::type_index_v<Field, Fields...>>(m_containers)
            .at(index);
      else
        return std::get<mpt::type_index_v<Field, Fields...>>(m_containers)
            .template get<Next...>(index);
    }
    /// Access the constant proxy of a field at the given index
    template <class Field, class... Next>
    resolve_const_reference_type_for_fields_pack_t<Field, Next...>
    get(size_type index) const {
      if constexpr (sizeof...(Next) == 0)
        return std::get<mpt::type_index_v<Field, Fields...>>(m_containers)
            .at(index);
      else
        return std::get<mpt::type_index_v<Field, Fields...>>(m_containers)
            .template get<Next...>(index);
    }

    /// Reserve memory for the given number of elements
    void reserve(size_type n) {
      reserve(n, std::make_index_sequence<number_of_fields>());
    }

    /// Change the size of the vector
    void resize(size_type n) {
      resize(n, std::make_index_sequence<number_of_fields>());
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
} // namespace mpt
