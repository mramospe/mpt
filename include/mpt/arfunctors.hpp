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
#include <type_traits>
#include <utility>

namespace mpt {

  namespace {
    /// Addition of two objects
    struct add {
      template <class L, class R>
      constexpr auto operator()(L &&lop, R &&rop) const {
        return lop + rop;
      }
    };
    /// Subtraction of two objects
    struct sub {
      template <class L, class R>
      constexpr auto operator()(L &&lop, R &&rop) const {
        return lop - rop;
      }
    };
    /// Multiplication of two objects
    struct mul {
      template <class L, class R>
      constexpr auto operator()(L &&lop, R &&rop) const {
        return lop * rop;
      }
    };
    /// Division of two objects
    struct div {
      template <class L, class R>
      constexpr auto operator()(L &&lop, R &&rop) const {
        return lop / rop;
      }
    };
    /// Modulo of two objects
    struct modulo {
      template <class L, class R>
      constexpr auto operator()(L &&lop, R &&rop) const {
        return lop % rop;
      }
    };
    /// Whether the left operand is less than the right operand
    struct lt {
      template <class L, class R> constexpr auto operator()(L &&lop, R &&rop) {
        return lop < rop;
      }
    };
    /// Whether the left operand is less or equal than the right operand
    struct leq {
      template <class L, class R>
      constexpr auto operator()(L &&lop, R &&rop) const {
        return lop <= rop;
      }
    };
    /// Whether the left operand is greater than the right operand
    struct gt {
      template <class L, class R> constexpr auto operator()(L &&lop, R &&rop) {
        return lop > rop;
      }
    };
    /// Whether the left operand is greater or equal than the right operand
    struct geq {
      template <class L, class R>
      constexpr auto operator()(L &&lop, R &&rop) const {
        return lop >= rop;
      }
    };
    /// Whether the left operand is equal to the right operand
    struct eq {
      template <class L, class R>
      constexpr auto operator()(L &&lop, R &&rop) const {
        return lop == rop;
      }
    };
    /// Whether the left operand is not equal to the right operand
    struct neq {
      template <class L, class R>
      constexpr auto operator()(L &&lop, R &&rop) const {
        return lop != rop;
      }
    };
    /// Logical and operation between two operands
    struct logical_and {
      template <class L, class R>
      constexpr auto operator()(L &&lop, R &&rop) const {
        return lop && rop;
      }
    };
    /// Logical or operation between two operands
    struct logical_or {
      template <class L, class R>
      constexpr auto operator()(L &&lop, R &&rop) const {
        return lop || rop;
      }
    };
    /// Logical not operation of an operand
    struct notop {
      template <class Operand> constexpr auto operator()(Operand &&op) const {
        return !op;
      }
    };
    /// Bitwise and operation between two operands
    struct bitwise_and {
      template <class L, class R>
      constexpr auto operator()(L &&lop, R &&rop) const {
        return lop & rop;
      }
    };
    /// Bitwise or operation between two operands
    struct bitwise_or {
      template <class L, class R>
      constexpr auto operator()(L &&lop, R &&rop) const {
        return lop | rop;
      }
    };
    /// Bitwise xor operation between two operands
    struct bitwise_xor {
      template <class L, class R>
      constexpr auto operator()(L &&lop, R &&rop) const {
        return lop ^ rop;
      }
    };
    /// Bitwise complement of an operand
    struct bitwise_complement {
      template <class Operand> constexpr auto operator()(Operand &&op) const {
        return ~op;
      }
    };
    /// Bitwise left shift operation between two operands
    struct bitwise_shift_left {
      template <class L, class R>
      constexpr auto operator()(L &&lop, R &&rop) const {
        return lop << rop;
      }
    };
    /// Bitwise right shift operation between two operands
    struct bitwise_shift_right {
      template <class L, class R>
      constexpr auto operator()(L &&lop, R &&rop) const {
        return lop >> rop;
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

  /*!\brief Binary operation between two objects

      Object that allows to apply an operator between two objects, which can be
      either functors or other values.
    */
  template <class BinaryOperator, class LeftOperand, class RightOperand>
  class binary_arfunctor : public arfunctor {

  public:
    /// Base class type
    using base_class_type = arfunctor;

    /// Operator type
    using binary_operator_type = BinaryOperator;
    /// Left operand type
    using left_operand_type = LeftOperand;
    /// Right operand type
    using right_operand_type = RightOperand;

    binary_arfunctor(left_operand_type const &lop,
                     right_operand_type const &rop)
        : base_class_type{}, m_left_operand{lop}, m_right_operand{rop} {}
    binary_arfunctor(left_operand_type &&lop, right_operand_type &&rop)
        : base_class_type{}, m_left_operand{std::forward<left_operand_type>(
                                 lop)},
          m_right_operand{std::forward<right_operand_type>(rop)} {}
    binary_arfunctor() = default;
    binary_arfunctor(binary_arfunctor const &) = default;
    binary_arfunctor(binary_arfunctor &&) = default;
    binary_arfunctor &operator=(binary_arfunctor const &) = default;
    binary_arfunctor &operator=(binary_arfunctor &&) = default;

    /// Evaluate the members (if they are functors) and apply the operand
    template <class... Args> constexpr auto operator()(Args &&... args) const {
      return binary_operator_type{}(
          evaluate_arfunctor_or_value(m_left_operand, args...),
          evaluate_arfunctor_or_value(m_right_operand, args...));
    }

  private:
    /// Left operand
    left_operand_type m_left_operand;
    /// Right operand
    right_operand_type m_right_operand;
  };

  /*!\brief Unary operation on an object

    Object that allows to apply an operator on an object.
   */
  template <class UnaryOperator, class Operand>
  class unary_arfunctor : public arfunctor {

  public:
    using base_class_type = arfunctor;

    unary_arfunctor(Operand const &op) : base_class_type{}, m_operand{op} {}
    unary_arfunctor(Operand &&op)
        : base_class_type{}, m_operand{std::forward<Operand>(op)} {}
    unary_arfunctor() = default;
    unary_arfunctor(unary_arfunctor const &) = default;
    unary_arfunctor(unary_arfunctor &&) = default;
    unary_arfunctor &operator=(unary_arfunctor const &) = default;
    unary_arfunctor &operator=(unary_arfunctor &&) = default;

    /// Operator type
    using unary_operator_type = UnaryOperator;
    /// Operand type
    using operand_type = Operand;

    /// Evaluate the operator on the stored member
    template <class... Args> constexpr auto operator()(Args &&... args) const {
      return unary_operator_type{}(
          evaluate_arfunctor_or_value(m_operand, args...));
    }

  private:
    /// Operand
    operand_type m_operand;
  };

  namespace {
    /// Function to build a binary functor from the operation and the operands
    template <class BinaryOperator, class LeftOperand, class RightOperand>
    binary_arfunctor<BinaryOperator, std::remove_cvref_t<LeftOperand>,
                     std::remove_cvref_t<RightOperand>>
    make_binary_arfunctor(LeftOperand &&lop, RightOperand &&rop) {
      return {std::forward<LeftOperand>(lop), std::forward<RightOperand>(rop)};
    }

    /// Function to build an unary functor from the operation and the operands
    template <class UnaryOperator, class Operand>
    unary_arfunctor<UnaryOperator, std::remove_cvref_t<Operand>>
    make_unary_arfunctor(Operand &&op) {
      return {std::forward<Operand>(op)};
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
    return make_binary_arfunctor<add>(std::forward<LeftOperand>(lop),
                                      std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires at_least_one_arfunctor_v<LeftOperand, RightOperand> constexpr auto
  operator-(LeftOperand &&lop, RightOperand &&rop) {
    return make_binary_arfunctor<sub>(std::forward<LeftOperand>(lop),
                                      std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires at_least_one_arfunctor_v<LeftOperand, RightOperand> constexpr auto
  operator*(LeftOperand &&lop, RightOperand &&rop) {
    return make_binary_arfunctor<mul>(std::forward<LeftOperand>(lop),
                                      std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires at_least_one_arfunctor_v<LeftOperand, RightOperand> constexpr auto
  operator/(LeftOperand &&lop, RightOperand &&rop) {
    return make_binary_arfunctor<div>(std::forward<LeftOperand>(lop),
                                      std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires at_least_one_arfunctor_v<LeftOperand, RightOperand> constexpr auto
  operator%(LeftOperand &&lop, RightOperand &&rop) {
    return make_binary_arfunctor<modulo>(std::forward<LeftOperand>(lop),
                                         std::forward<RightOperand>(rop));
  }

  // RightOperandelational operators

  template <class LeftOperand, class RightOperand>
  requires at_least_one_arfunctor_v<LeftOperand, RightOperand> constexpr auto
  operator==(LeftOperand &&lop, RightOperand &&rop) {
    return make_binary_arfunctor<eq>(std::forward<LeftOperand>(lop),
                                     std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires at_least_one_arfunctor_v<LeftOperand, RightOperand> constexpr auto
  operator!=(LeftOperand &&lop, RightOperand &&rop) {
    return make_binary_arfunctor<neq>(std::forward<LeftOperand>(lop),
                                      std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires at_least_one_arfunctor_v<LeftOperand, RightOperand> constexpr auto
  operator<(LeftOperand &&lop, RightOperand &&rop) {
    return make_binary_arfunctor<lt>(std::forward<LeftOperand>(lop),
                                     std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires at_least_one_arfunctor_v<LeftOperand, RightOperand> constexpr auto
  operator<=(LeftOperand &&lop, RightOperand &&rop) {
    return make_binary_arfunctor<leq>(std::forward<LeftOperand>(lop),
                                      std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires at_least_one_arfunctor_v<LeftOperand, RightOperand> constexpr auto
  operator>(LeftOperand &&lop, RightOperand &&rop) {
    return make_binary_arfunctor<gt>(std::forward<LeftOperand>(lop),
                                     std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires at_least_one_arfunctor_v<LeftOperand, RightOperand> constexpr auto
  operator>=(LeftOperand &&lop, RightOperand &&rop) {
    return make_binary_arfunctor<geq>(std::forward<LeftOperand>(lop),
                                      std::forward<RightOperand>(rop));
  }

  // LeftOperandogical operators

  template <class LeftOperand, class RightOperand>
  requires at_least_one_arfunctor_v<LeftOperand, RightOperand> constexpr auto
  operator&&(LeftOperand &&lop, RightOperand &&rop) {
    return make_binary_arfunctor<logical_and>(std::forward<LeftOperand>(lop),
                                              std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires at_least_one_arfunctor_v<LeftOperand, RightOperand> constexpr auto
  operator||(LeftOperand &&lop, RightOperand &&rop) {
    return make_binary_arfunctor<logical_or>(std::forward<LeftOperand>(lop),
                                             std::forward<RightOperand>(rop));
  }

  template <class Operand>
  requires is_arfunctor_v<Operand> constexpr auto operator!(Operand &&op) {
    return make_unary_arfunctor<notop>(std::forward<Operand>(op));
  }

  // Bitwise operators

  template <class LeftOperand, class RightOperand>
  requires at_least_one_arfunctor_v<LeftOperand, RightOperand> constexpr auto
  operator&(LeftOperand &&lop, RightOperand &&rop) {
    return make_binary_arfunctor<bitwise_and>(std::forward<LeftOperand>(lop),
                                              std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires at_least_one_arfunctor_v<LeftOperand, RightOperand> constexpr auto
  operator|(LeftOperand &&lop, RightOperand &&rop) {
    return make_binary_arfunctor<bitwise_or>(std::forward<LeftOperand>(lop),
                                             std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires at_least_one_arfunctor_v<LeftOperand, RightOperand> constexpr auto
  operator^(LeftOperand &&lop, RightOperand &&rop) {
    return make_binary_arfunctor<bitwise_xor>(std::forward<LeftOperand>(lop),
                                              std::forward<RightOperand>(rop));
  }

  template <class Operand>
  requires is_arfunctor_v<Operand> constexpr auto operator~(Operand &&op) {
    return make_unary_arfunctor<bitwise_complement>(std::forward<Operand>(op));
  }

  template <class LeftOperand, class RightOperand>
  requires at_least_one_arfunctor_v<LeftOperand, RightOperand> constexpr auto
  operator<<(LeftOperand &&lop, RightOperand &&rop) {
    return make_binary_arfunctor<bitwise_shift_left>(
        std::forward<LeftOperand>(lop), std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires at_least_one_arfunctor_v<LeftOperand, RightOperand> constexpr auto
  operator>>(LeftOperand &&lop, RightOperand &&rop) {
    return make_binary_arfunctor<bitwise_shift_right>(
        std::forward<LeftOperand>(lop), std::forward<RightOperand>(rop));
  }

} // namespace mpt
