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
#include <tuple>
#include <type_traits>
#include <utility>

namespace mpt {

  namespace {
    /// Addition of two objects
    struct add {
      template <class L, class R>
      constexpr auto operator()(L &&lop, R &&rop) const {
        return std::forward<L>(lop) + std::forward<R>(rop);
      }
    };
    /// Subtraction of two objects
    struct sub {
      template <class L, class R>
      constexpr auto operator()(L &&lop, R &&rop) const {
        return std::forward<L>(lop) - std::forward<R>(rop);
      }
    };
    /// Multiplication of two objects
    struct mul {
      template <class L, class R>
      constexpr auto operator()(L &&lop, R &&rop) const {
        return std::forward<L>(lop) * std::forward<R>(rop);
      }
    };
    /// Division of two objects
    struct div {
      template <class L, class R>
      constexpr auto operator()(L &&lop, R &&rop) const {
        return std::forward<L>(lop) / std::forward<R>(rop);
      }
    };
    /// Modulo of two objects
    struct modulo {
      template <class L, class R>
      constexpr auto operator()(L &&lop, R &&rop) const {
        return std::forward<L>(lop) % std::forward<R>(rop);
      }
    };
    /// Whether the left operand is less than the right operand
    struct lt {
      template <class L, class R> constexpr auto operator()(L &&lop, R &&rop) {
        return std::forward<L>(lop) < std::forward<R>(rop);
      }
    };
    /// Whether the left operand is less or equal than the right operand
    struct leq {
      template <class L, class R>
      constexpr auto operator()(L &&lop, R &&rop) const {
        return std::forward<L>(lop) <= std::forward<R>(rop);
      }
    };
    /// Whether the left operand is greater than the right operand
    struct gt {
      template <class L, class R> constexpr auto operator()(L &&lop, R &&rop) {
        return std::forward<L>(lop) > std::forward<R>(rop);
      }
    };
    /// Whether the left operand is greater or equal than the right operand
    struct geq {
      template <class L, class R>
      constexpr auto operator()(L &&lop, R &&rop) const {
        return std::forward<L>(lop) >= std::forward<R>(rop);
      }
    };
    /// Whether the left operand is equal to the right operand
    struct eq {
      template <class L, class R>
      constexpr auto operator()(L &&lop, R &&rop) const {
        return std::forward<L>(lop) == std::forward<R>(rop);
      }
    };
    /// Whether the left operand is not equal to the right operand
    struct neq {
      template <class L, class R>
      constexpr auto operator()(L &&lop, R &&rop) const {
        return std::forward<L>(lop) != std::forward<R>(rop);
      }
    };
    /// Logical and operation between two operands
    struct logical_and {
      template <class L, class R>
      constexpr auto operator()(L &&lop, R &&rop) const {
        return std::forward<L>(lop) && std::forward<R>(rop);
      }
    };
    /// Logical or operation between two operands
    struct logical_or {
      template <class L, class R>
      constexpr auto operator()(L &&lop, R &&rop) const {
        return std::forward<L>(lop) || std::forward<R>(rop);
      }
    };
    /// Logical not operation of an operand
    struct notop {
      template <class Operand> constexpr auto operator()(Operand &&op) const {
        return !std::forward<Operand>(op);
      }
    };
    /// Bitwise and operation between two operands
    struct bitwise_and {
      template <class L, class R>
      constexpr auto operator()(L &&lop, R &&rop) const {
        return std::forward<L>(lop) & std::forward<R>(rop);
      }
    };
    /// Bitwise or operation between two operands
    struct bitwise_or {
      template <class L, class R>
      constexpr auto operator()(L &&lop, R &&rop) const {
        return std::forward<L>(lop) | std::forward<R>(rop);
      }
    };
    /// Bitwise xor operation between two operands
    struct bitwise_xor {
      template <class L, class R>
      constexpr auto operator()(L &&lop, R &&rop) const {
        return std::forward<L>(lop) ^ std::forward<R>(rop);
      }
    };
    /// Bitwise complement of an operand
    struct bitwise_complement {
      template <class Operand> constexpr auto operator()(Operand &&op) const {
        return ~std::forward<Operand>(op);
      }
    };
    /// Bitwise left shift operation between two operands
    struct bitwise_shift_left {
      template <class L, class R>
      constexpr auto operator()(L &&lop, R &&rop) const {
        return std::forward<L>(lop) << std::forward<R>(rop);
      }
    };
    /// Bitwise right shift operation between two operands
    struct bitwise_shift_right {
      template <class L, class R>
      constexpr auto operator()(L &&lop, R &&rop) const {
        return std::forward<L>(lop) >> std::forward<R>(rop);
      }
    };
  } // namespace

  /*!\brief Base arithmetic and relational functor class

    This class defines the basic functionality of any arithmetic and relational functor.
    Objects inheriting from this class must define a constant call operator.
   */
  struct arfunctor {};

  namespace {
    /// Check if the provided type is an arithmetic and relational functor
    template <class T> struct is_arfunctor {
      static constexpr auto value =
          std::is_base_of_v<arfunctor, std::remove_cvref_t<T>>;
    };

    /// Whether the provided type is an arithmetic and relational functor
    template <class T>
    static constexpr auto is_arfunctor_v = is_arfunctor<T>::value;

    /// Depending on the input argument type, evaluate the functor or return the value
    template <class T, class... Args>
    auto evaluate_arfunctor_or_value(T &&fv, [[maybe_unused]] Args &&... args) {
      if constexpr (is_arfunctor_v<T>)
        return fv(args...);
      else
        return fv;
    }
  } // namespace

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

    composed_arfunctor(Operand const &... op)
        : base_class_type{}, m_operands{op...} {}
    composed_arfunctor(Operand &&... op)
        : base_class_type{}, m_operands{std::forward<Operand>(op)...} {}
    composed_arfunctor() = default;
    composed_arfunctor(composed_arfunctor const &) = default;
    composed_arfunctor(composed_arfunctor &&) = default;
    composed_arfunctor &operator=(composed_arfunctor const &) = default;
    composed_arfunctor &operator=(composed_arfunctor &&) = default;

    /// Evaluate the members (if they are functors) and apply the operand
    template <class... Args> constexpr auto operator()(Args &&... args) const {
      return this->call_impl(
          std::make_index_sequence<std::tuple_size_v<operands_type>>(),
          args...);
    }

  private:
    /// Operands
    operands_type m_operands;

    /// Call each operand with the provided arguments and evaluate the operation
    template <class... Args, std::size_t... I>
    auto call_impl(std::index_sequence<I...>, Args &&... args) const {
      return operator_type{}(
          evaluate_arfunctor_or_value(std::get<I>(m_operands), args...)...);
    }
  };

  namespace {
    /// Function to build a composed functor from the operator and the operand types
    template <class Operator, class... Operand>
    constexpr composed_arfunctor<Operator, std::remove_cvref_t<Operand>...>
    make_composed_arfunctor(Operand &&... op) {
      return {std::forward<Operand>(op)...};
    }

    /// Check that at least one of the operands is a functor
    template <class LeftOperand, class RightOperand>
    struct at_least_one_arfunctor {
      static constexpr auto value =
          (is_arfunctor_v<LeftOperand> || is_arfunctor_v<RightOperand>);
    };

    /// Whether one of the operands is a functor
    template <class LeftOperand, class RightOperand>
    static constexpr auto at_least_one_arfunctor_v =
        at_least_one_arfunctor<LeftOperand, RightOperand>::value;
  } // namespace

  // Arithmetic operators

  template <class LeftOperand, class RightOperand>
  requires at_least_one_arfunctor_v<LeftOperand, RightOperand> constexpr auto
  operator+(LeftOperand &&lop, RightOperand &&rop) {
    return make_composed_arfunctor<add>(std::forward<LeftOperand>(lop),
                                        std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires at_least_one_arfunctor_v<LeftOperand, RightOperand> constexpr auto
  operator-(LeftOperand &&lop, RightOperand &&rop) {
    return make_composed_arfunctor<sub>(std::forward<LeftOperand>(lop),
                                        std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires at_least_one_arfunctor_v<LeftOperand, RightOperand> constexpr auto
  operator*(LeftOperand &&lop, RightOperand &&rop) {
    return make_composed_arfunctor<mul>(std::forward<LeftOperand>(lop),
                                        std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires at_least_one_arfunctor_v<LeftOperand, RightOperand> constexpr auto
  operator/(LeftOperand &&lop, RightOperand &&rop) {
    return make_composed_arfunctor<div>(std::forward<LeftOperand>(lop),
                                        std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires at_least_one_arfunctor_v<LeftOperand, RightOperand> constexpr auto
  operator%(LeftOperand &&lop, RightOperand &&rop) {
    return make_composed_arfunctor<modulo>(std::forward<LeftOperand>(lop),
                                           std::forward<RightOperand>(rop));
  }

  // RightOperandelational operators

  template <class LeftOperand, class RightOperand>
  requires at_least_one_arfunctor_v<LeftOperand, RightOperand> constexpr auto
  operator==(LeftOperand &&lop, RightOperand &&rop) {
    return make_composed_arfunctor<eq>(std::forward<LeftOperand>(lop),
                                       std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires at_least_one_arfunctor_v<LeftOperand, RightOperand> constexpr auto
  operator!=(LeftOperand &&lop, RightOperand &&rop) {
    return make_composed_arfunctor<neq>(std::forward<LeftOperand>(lop),
                                        std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires at_least_one_arfunctor_v<LeftOperand, RightOperand> constexpr auto
  operator<(LeftOperand &&lop, RightOperand &&rop) {
    return make_composed_arfunctor<lt>(std::forward<LeftOperand>(lop),
                                       std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires at_least_one_arfunctor_v<LeftOperand, RightOperand> constexpr auto
  operator<=(LeftOperand &&lop, RightOperand &&rop) {
    return make_composed_arfunctor<leq>(std::forward<LeftOperand>(lop),
                                        std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires at_least_one_arfunctor_v<LeftOperand, RightOperand> constexpr auto
  operator>(LeftOperand &&lop, RightOperand &&rop) {
    return make_composed_arfunctor<gt>(std::forward<LeftOperand>(lop),
                                       std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires at_least_one_arfunctor_v<LeftOperand, RightOperand> constexpr auto
  operator>=(LeftOperand &&lop, RightOperand &&rop) {
    return make_composed_arfunctor<geq>(std::forward<LeftOperand>(lop),
                                        std::forward<RightOperand>(rop));
  }

  // LeftOperandogical operators

  template <class LeftOperand, class RightOperand>
  requires at_least_one_arfunctor_v<LeftOperand, RightOperand> constexpr auto
  operator&&(LeftOperand &&lop, RightOperand &&rop) {
    return make_composed_arfunctor<logical_and>(
        std::forward<LeftOperand>(lop), std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires at_least_one_arfunctor_v<LeftOperand, RightOperand> constexpr auto
  operator||(LeftOperand &&lop, RightOperand &&rop) {
    return make_composed_arfunctor<logical_or>(std::forward<LeftOperand>(lop),
                                               std::forward<RightOperand>(rop));
  }

  template <class Operand>
  requires is_arfunctor_v<Operand> constexpr auto operator!(Operand &&op) {
    return make_composed_arfunctor<notop>(std::forward<Operand>(op));
  }

  // Bitwise operators

  template <class LeftOperand, class RightOperand>
  requires at_least_one_arfunctor_v<LeftOperand, RightOperand> constexpr auto
  operator&(LeftOperand &&lop, RightOperand &&rop) {
    return make_composed_arfunctor<bitwise_and>(
        std::forward<LeftOperand>(lop), std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires at_least_one_arfunctor_v<LeftOperand, RightOperand> constexpr auto
  operator|(LeftOperand &&lop, RightOperand &&rop) {
    return make_composed_arfunctor<bitwise_or>(std::forward<LeftOperand>(lop),
                                               std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires at_least_one_arfunctor_v<LeftOperand, RightOperand> constexpr auto
  operator^(LeftOperand &&lop, RightOperand &&rop) {
    return make_composed_arfunctor<bitwise_xor>(
        std::forward<LeftOperand>(lop), std::forward<RightOperand>(rop));
  }

  template <class Operand>
  requires is_arfunctor_v<Operand> constexpr auto operator~(Operand &&op) {
    return make_composed_arfunctor<bitwise_complement>(
        std::forward<Operand>(op));
  }

  template <class LeftOperand, class RightOperand>
  requires at_least_one_arfunctor_v<LeftOperand, RightOperand> constexpr auto
  operator<<(LeftOperand &&lop, RightOperand &&rop) {
    return make_composed_arfunctor<bitwise_shift_left>(
        std::forward<LeftOperand>(lop), std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires at_least_one_arfunctor_v<LeftOperand, RightOperand> constexpr auto
  operator>>(LeftOperand &&lop, RightOperand &&rop) {
    return make_composed_arfunctor<bitwise_shift_right>(
        std::forward<LeftOperand>(lop), std::forward<RightOperand>(rop));
  }
} // namespace mpt
