/*!\file
  Definition of functors implementing arithmetic and relational
  operations.
  User functors must inherit from \ref mpt::arfunctor and have the
  call operator defined for a particular set of objects.
  Arithmetic and relational operations are quaranteed to work
  as long as all the new functors have the call operator implemented
  for the same set of types.

  \code{.cpp}
  #include "mpt/arfunctors.hpp"

  struct position {
     float x, y;
  }

  struct access_x : mpt::functor {
     constexpr auto operator()(position const& pos) {
        return pos.x;
     }
  };

  struct access_y : mpt::functor {
     constexpr auto operator()(position const& pos) {
        return pos.y;
     }
  };

  int main() {

     position pos = {1.f, 2.f};

     auto a = (access_x + access_y)(pos); // calculate "pos.x + pos.y"

     ...

     return 0;
  }
  \endcode
 */
#pragma once
#include "mpt/types.hpp"
#include <ostream>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

namespace mpt::arfunctors {

  namespace core {

    /*!\brief Base arithmetic and relational functor class

    This class defines the basic functionality of any arithmetic and relational functor.
    Objects inheriting from this class must define a constant call operator.
   */
    struct arfunctor {};

    template <class FunctionLikeOrArithmeticType> class arfunctor_wrapper {
    public:
      template <class T>
      constexpr arfunctor_wrapper(T &&obj) : m_obj{std::forward<T>(obj)} {}

      template <class... Input>
      constexpr auto operator()(Input &&...arg) const {
        if constexpr (std::is_arithmetic_v<FunctionLikeOrArithmeticType>)
          return m_obj;
        else
          return m_obj(std::forward<Input>(arg)...);
      }

    private:
      FunctionLikeOrArithmeticType m_obj;
    };

    /// Make a functor from another callable that evaluates to a constant expression
    template <class FunctionLikeOrArithmeticType>
    constexpr auto make_arfunctor(FunctionLikeOrArithmeticType &&f) {
      return arfunctor_wrapper<std::decay_t<FunctionLikeOrArithmeticType>>(
          std::forward<FunctionLikeOrArithmeticType>(f));
    }

    /// Check if the provided type is an arithmetic and relational functor
    template <class T>
    struct is_arfunctor
        : std::conditional_t<
              std::is_base_of_v<arfunctor, std::remove_cvref_t<T>>,
              std::true_type, std::false_type> {};

    /// Whether the provided type is an arithmetic and relational functor
    template <class T>
    static constexpr auto is_arfunctor_v = is_arfunctor<T>::value;

    /*!\brief Composed operation between two objects

    This object allows to perform an operation that takes several values
    as an input, and which can come from other functors or be constant.
    */
    template <class Operator, class... Operand>
    class composed_arfunctor : public arfunctor {

    public:
      /// Base class type
      using base_class_type = arfunctor;

      /// Operator type
      using operator_type = Operator;
      /// Operand pack type
      using operands_type = std::tuple<Operand...>;

      composed_arfunctor(Operand const &...op)
          : base_class_type{}, m_operands{op...} {}
      composed_arfunctor(Operand &&...op)
          : base_class_type{}, m_operands{std::forward<Operand>(op)...} {}
      composed_arfunctor() = default;
      composed_arfunctor(composed_arfunctor const &) = default;
      composed_arfunctor(composed_arfunctor &&) = default;
      composed_arfunctor &operator=(composed_arfunctor const &) = default;
      composed_arfunctor &operator=(composed_arfunctor &&) = default;

      /// Evaluate the members (if they are functors) and apply the operand
      template <class... Args> constexpr auto operator()(Args &&...args) const {
        return this->call_impl(
            std::make_index_sequence<std::tuple_size_v<operands_type>>(),
            args...);
      }

      operands_type const &operands() const { return m_operands; }

    private:
      /// Operands
      operands_type m_operands;

      /// Call each operand with the provided arguments and evaluate the operation
      template <class... Args, std::size_t... I>
      auto call_impl(std::index_sequence<I...>, Args &&...args) const {
        return operator_type{}(std::get<I>(m_operands).operator()(args...)...);
      }
    };

  } // namespace core

  using arfunctor = mpt::arfunctors::core::arfunctor;

  /// Build a composed functor from the operator and the operand types
  template <class Operator, class... Operand>
  constexpr mpt::arfunctors::core::composed_arfunctor<
      Operator, std::remove_cvref_t<Operand>...>
  make_composed_arfunctor(Operand &&...op) {
    return {std::forward<Operand>(op)...};
  }
} // namespace mpt::arfunctors
