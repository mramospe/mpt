#pragma once
#include "mpt/indexed_container.hpp"
#include <algorithm>
#include <array>
#include <functional>
#include <utility>
#include <vector>

namespace mpt {

  namespace {

    template <class SizeContainerType>
    constexpr auto product_of_sizes(SizeContainerType &&prod) {
      using value_type = std::decay_t<SizeContainerType>::value_type;
      auto first_size = prod.front();
      std::copy(std::next(prod.cbegin()), prod.cend(), prod.begin());
      prod.back() = 1ul;
      std::transform(std::next(prod.crbegin()), prod.crend(), prod.crbegin(),
                     std::next(prod.rbegin()), std::multiplies<value_type>());
      return std::make_pair(std::move(prod), prod.front() * first_size);
    }
  } // namespace

  template <class SizeContainerType> class multi_index {

  public:
    using container_type = SizeContainerType;
    using value_type = typename container_type::value_type;
    using size_type = value_type;

    constexpr multi_index() = delete;

  private:
    constexpr multi_index(std::pair<container_type, value_type> data)
        : m_products{std::move(data.first)}, m_size{std::move(data.second)} {}

  public:
    template <class... S>
    constexpr multi_index(S &&...s)
        : multi_index(container_type{std::forward<S>(s)...}) {}

    constexpr multi_index(container_type &&sizes)
        : multi_index(product_of_sizes(std::move(sizes))) {}
    constexpr multi_index(container_type const &sizes)
        : multi_index(product_of_sizes(sizes)) {}

    constexpr auto dim() const { return m_products.size(); }

    constexpr auto size() const { return m_size; }

    constexpr auto unravel(std::size_t dim, std::size_t index) const {

      for (auto i = 0ul; i < dim; ++i)
        index %= m_products[i];

      return index / m_products[dim];
    }

    constexpr auto at(value_type index) const {

      container_type result = m_products;
      for (auto i = 0ul; i < m_products.size(); ++i) {
        result[i] = index / m_products[i];
        index %= m_products[i];
      }

      return result;
    }

    constexpr auto operator[](value_type index) const {
      return at(std::move(index));
    }

    auto begin() const {
      return mpt::make_indexed_container_proxy(*this).begin();
    }

    auto end() const { return mpt::make_indexed_container_proxy(*this).end(); }

    auto cbegin() const {
      return mpt::make_indexed_container_proxy(*this).cbegin();
    }

    auto cend() const {
      return mpt::make_indexed_container_proxy(*this).cend();
    }

    auto rbegin() const {
      return mpt::make_indexed_container_proxy(*this).rbegin();
    }

    auto rend() const {
      return mpt::make_indexed_container_proxy(*this).rend();
    }

    auto crbegin() const {
      return mpt::make_indexed_container_proxy(*this).crbegin();
    }

    auto crend() const {
      return mpt::make_indexed_container_proxy(*this).crend();
    }

  private:
    container_type m_products;
    value_type m_size;
  };

  template <std::size_t N, class SizeType = std::size_t>
  using multi_index_array = multi_index<std::array<SizeType, N>>;

  template <class SizeType = std::size_t, class... S>
  auto make_multi_index_array(S &&...s) {
    return multi_index_array<sizeof...(S), SizeType>(std::forward<S>(s)...);
  }

  template <class SizeType = std::size_t,
            class Allocator = std::allocator<SizeType>>
  using multi_index_vector = multi_index<std::vector<SizeType, Allocator>>;

  template <class SizeType = std::size_t,
            class Allocator = std::allocator<SizeType>, class... S>
  auto make_multi_index_vector(S &&...s) {
    return multi_index_vector<SizeType, Allocator>(std::forward<S>(s)...);
  }
} // namespace mpt
