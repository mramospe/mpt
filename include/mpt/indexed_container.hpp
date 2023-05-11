#pragma once
#include <iterator>
#include <limits>
#include <type_traits>
#include <utility>

namespace mpt {

  enum class iterator_direction : int { forward, backward };

  namespace {

    template <class T> auto increase(T index) { return ++index; }

    template <class T> auto decrease(T index) { return --index; }

    template <iterator_direction> struct indexed_iterator_traits;

    template <> struct indexed_iterator_traits<iterator_direction::backward> {

      template <class T> static constexpr auto next(T index) {
        return decrease(index);
      }

      template <class T> static constexpr auto prev(T index) {
        return increase(index);
      }
    };

    template <> struct indexed_iterator_traits<iterator_direction::forward> {

      template <class T> static constexpr auto next(T index) {
        return increase(index);
      }

      template <class T> static constexpr auto prev(T index) {
        return decrease(index);
      }
    };
  } // namespace

  template <class ContainerType> class indexed_container_proxy;

  template <class ContainerType, iterator_direction IteratorDirection>
  class base_indexed_container_iterator {

  public:
    friend class indexed_container_proxy<ContainerType>;

    base_indexed_container_iterator() = delete;

    using adaptor_type = indexed_iterator_traits<IteratorDirection>;
    using container_type = ContainerType;
    using container_pointer_type = container_type *;
    using size_type = typename ContainerType::size_type;
    using return_value_type =
        decltype(std::declval<container_type>().operator[](
            std::declval<size_type>()));

    using iterator_category = std::random_access_iterator_tag;
    using difference_type = size_type;
    using value_type = std::decay_t<return_value_type>;
    using reference = std::add_lvalue_reference_t<return_value_type>;

    static_assert(std::is_unsigned_v<size_type>,
                  "The index must be of unsigned integral type");
    static_assert(size_type(-1) == std::numeric_limits<size_type>::max(),
                  "The negative value of the index type must evaluate to the "
                  "maximum value of that type");

    return_value_type operator*() { return m_ptr->operator[](m_pos); }

    auto &operator++() {
      m_pos = adaptor_type::next(m_pos);
      return *this;
    }

    auto operator++(int) {
      auto tmp = *this;
      ++(*this);
      return tmp;
    }

    auto &operator--() {
      m_pos = adaptor_type::prev(m_pos);
      return *this;
    }

    auto operator--(int) {
      auto tmp = *this;
      --(*this);
      return tmp;
    }

    auto operator-(base_indexed_container_iterator const &other) const {
      if constexpr (IteratorDirection == iterator_direction::forward)
        return m_pos - other.m_pos;
      else
        return other.m_pos - m_pos;
    }

    auto &operator-=(size_type v) {
      m_pos -= v;
      return *this;
    }

    auto &operator+=(size_type v) {
      m_pos += v;
      return *this;
    }

    auto operator-(size_type v) {
      auto tmp = *this;
      return tmp -= v;
    }

    auto operator+(size_type v) {
      auto tmp = *this;
      return tmp += v;
    }

    auto operator==(base_indexed_container_iterator const &other) const {
      return m_pos == other.m_pos;
    }

    auto operator!=(base_indexed_container_iterator const &other) const {
      return !(*this == other);
    }

  private:
    base_indexed_container_iterator(container_pointer_type ptr, size_type pos)
        : m_ptr{std::move(ptr)}, m_pos{std::move(pos)} {}

    container_pointer_type m_ptr = nullptr;
    size_type m_pos = 0ul;
  };

  template <class ContainerType>
  using forward_indexed_container_iterator =
      base_indexed_container_iterator<ContainerType,
                                      iterator_direction::forward>;

  template <class ContainerType>
  using backward_indexed_container_iterator =
      base_indexed_container_iterator<ContainerType,
                                      iterator_direction::backward>;

  template <class ContainerType>
  indexed_container_proxy<ContainerType>
  make_indexed_container_proxy(ContainerType &);

  template <class ContainerType> class indexed_container_proxy {

  public:
    indexed_container_proxy() = delete;

    template <class T>
    friend indexed_container_proxy<T> make_indexed_container_proxy(T &);

    using container_type = ContainerType;
    using container_pointer_type = container_type *;
    using size_type = typename ContainerType::size_type;
    using return_value_type =
        decltype(std::declval<container_type>().operator[](
            std::declval<size_type>()));

    auto size() const { return std::size(*m_ptr); }

    return_value_type operator[](size_type index) const {
      return m_ptr->operator[](std::move(index));
    }

    auto begin() const {
      return forward_indexed_container_iterator<container_type>(m_ptr, 0ul);
    }

    auto end() const {
      return forward_indexed_container_iterator<container_type>(m_ptr, size());
    }

    auto cbegin() const {
      return forward_indexed_container_iterator<
          std::add_const_t<container_type>>(m_ptr, 0ul);
    }

    auto cend() const {
      return forward_indexed_container_iterator<
          std::add_const_t<container_type>>(m_ptr, size());
    }

    auto rbegin() const {
      return backward_indexed_container_iterator<container_type>(
          m_ptr, size() - size_type{1});
    }

    auto rend() const {
      return backward_indexed_container_iterator<container_type>(
          m_ptr, std::numeric_limits<size_type>::max());
    }

    auto crbegin() const {
      return backward_indexed_container_iterator<
          std::add_const_t<container_type>>(m_ptr, size() - size_type{1});
    }

    auto crend() const {
      return backward_indexed_container_iterator<
          std::add_const_t<container_type>>(
          m_ptr, std::numeric_limits<size_type>::max());
    }

  private:
    indexed_container_proxy(container_pointer_type ptr)
        : m_ptr{std::move(ptr)} {}

    container_pointer_type m_ptr = nullptr;
  };

  template <class ContainerType>
  indexed_container_proxy<ContainerType>
  make_indexed_container_proxy(ContainerType &cont) {
    return indexed_container_proxy<ContainerType>(&cont);
  }
} // namespace mpt
