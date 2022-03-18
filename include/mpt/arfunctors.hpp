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

  template <class BinaryOperator, class LeftOperand, class RightOperand>
  class binary_arfunctor;

  template <class UnaryOperator, class Operand> class unary_arfunctor;

  template <class DerivedFunctor> struct arfunctor;

  namespace {
    /// Function to build a binary functor from the operation and the operands
    template <class BinaryOperator, class LeftOperand, class RightOperand>
    binary_arfunctor<BinaryOperator, LeftOperand, RightOperand>
    make_binary_arfunctor(LeftOperand const &lop, RightOperand const &rop) {
      return {lop, rop};
    }

    /// Function to build an unary functor from the operation and the operands
    template <class UnaryOperator, class Operand>
    unary_arfunctor<UnaryOperator, Operand> make_unary_arfunctor(Operand &&op) {
      return {op};
    }
  } // namespace

  /// Check if the provided type is an arithmetic and relational functor
  template <class T> struct is_arfunctor {
    static constexpr auto value = std::is_base_of_v<arfunctor<T>, T>;
  };

  /// Whether the provided type is an arithmetic and relational functor
  template <class T>
  static constexpr auto is_arfunctor_v = is_arfunctor<T>::value;

  /*!\brief Base arithmetic and relational functor class

    This class defines the basic functionality of any arithmetic and relational functor.
    Objects inheriting from this class must define a constant call operator.
   */
  template <class DerivedFunctor> struct arfunctor {

    /// The actual functor type
    using functor_type = DerivedFunctor;

    // Arithmetic operators

    template <class FunctorOrValue>
    constexpr friend auto operator+(functor_type const &fctr,
                                    FunctorOrValue &&other) {
      return make_binary_arfunctor<add>(fctr, other);
    }

    template <class T>
    requires(!is_arfunctor_v<std::remove_cvref_t<T>>) constexpr friend auto
    operator+(T &&other, functor_type const &fctr) {
      return make_binary_arfunctor<add>(std::forward<T>(other), fctr);
    }

    template <class FunctorOrValue>
    constexpr friend auto operator-(functor_type const &fctr,
                                    FunctorOrValue &&other) {
      return make_binary_arfunctor<sub>(fctr,
                                        std::forward<FunctorOrValue>(other));
    }

    template <class T>
    requires(!is_arfunctor_v<std::remove_cvref_t<T>>) constexpr friend auto
    operator-(T &&other, functor_type const &fctr) {
      return make_binary_arfunctor<sub>(std::forward<T>(other), fctr);
    }

    template <class FunctorOrValue>
    constexpr friend auto operator*(functor_type const &fctr,
                                    FunctorOrValue &&other) {
      return make_binary_arfunctor<mul>(fctr,
                                        std::forward<FunctorOrValue>(other));
    }

    template <class T>
    requires(!is_arfunctor_v<std::remove_cvref_t<T>>) constexpr friend auto
    operator*(T &&other, functor_type const &fctr) {
      return make_binary_arfunctor<mul>(std::forward<T>(other), fctr);
    }

    template <class FunctorOrValue>
    constexpr friend auto operator/(functor_type const &fctr,
                                    FunctorOrValue &&other) {
      return make_binary_arfunctor<div>(fctr,
                                        std::forward<FunctorOrValue>(other));
    }

    template <class T>
    requires(!is_arfunctor_v<std::remove_cvref_t<T>>) constexpr friend auto
    operator/(T &&other, functor_type const &fctr) {
      return make_binary_arfunctor<div>(std::forward<T>(other), fctr);
    }

    template <class FunctorOrValue>
    constexpr friend auto operator%(functor_type const &fctr,
                                    FunctorOrValue &&other) {
      return make_binary_arfunctor<modulo>(fctr,
                                           std::forward<FunctorOrValue>(other));
    }

    template <class T>
    requires(!is_arfunctor_v<std::remove_cvref_t<T>>) constexpr friend auto
    operator%(T &&other, functor_type const &fctr) {
      return make_binary_arfunctor<modulo>(std::forward<T>(other), fctr);
    }

    // Relational operators

    template <class FunctorOrValue>
    constexpr friend auto operator==(functor_type const &fctr,
                                     FunctorOrValue &&other) {
      return make_binary_arfunctor<eq>(fctr,
                                       std::forward<FunctorOrValue>(other));
    }

    template <class T>
    requires(!is_arfunctor_v<std::remove_cvref_t<T>>) constexpr friend auto
    operator==(T &&other, functor_type const &fctr) {
      return make_binary_arfunctor<eq>(std::forward<T>(other), fctr);
    }

    template <class FunctorOrValue>
    constexpr friend auto operator!=(functor_type const &fctr,
                                     FunctorOrValue &&other) {
      return make_binary_arfunctor<neq>(fctr,
                                        std::forward<FunctorOrValue>(other));
    }

    template <class T>
    requires(!is_arfunctor_v<std::remove_cvref_t<T>>) constexpr friend auto
    operator!=(T &&other, functor_type const &fctr) {
      return make_binary_arfunctor<neq>(std::forward<T>(other), fctr);
    }

    template <class FunctorOrValue>
    constexpr friend auto operator<(functor_type const &fctr,
                                    FunctorOrValue &&other) {
      return make_binary_arfunctor<lt>(fctr,
                                       std::forward<FunctorOrValue>(other));
    }

    template <class T>
    requires(!is_arfunctor_v<std::remove_cvref_t<T>>) constexpr friend auto
    operator<(T &&other, functor_type const &fctr) {
      return make_binary_arfunctor<lt>(std::forward<T>(other), fctr);
    }

    template <class FunctorOrValue>
    constexpr friend auto operator<=(functor_type const &fctr,
                                     FunctorOrValue &&other) {
      return make_binary_arfunctor<leq>(fctr,
                                        std::forward<FunctorOrValue>(other));
    }

    template <class T>
    requires(!is_arfunctor_v<std::remove_cvref_t<T>>) constexpr friend auto
    operator<=(T &&other, functor_type const &fctr) {
      return make_binary_arfunctor<leq>(std::forward<T>(other), fctr);
    }

    template <class FunctorOrValue>
    constexpr friend auto operator>(functor_type const &fctr,
                                    FunctorOrValue &&other) {
      return make_binary_arfunctor<gt>(fctr,
                                       std::forward<FunctorOrValue>(other));
    }

    template <class T>
    requires(!is_arfunctor_v<std::remove_cvref_t<T>>) constexpr friend auto
    operator>(T &&other, functor_type const &fctr) {
      return make_binary_arfunctor<gt>(std::forward<T>(other), fctr);
    }

    template <class FunctorOrValue>
    constexpr friend auto operator>=(functor_type const &fctr,
                                     FunctorOrValue &&other) {
      return make_binary_arfunctor<geq>(fctr,
                                        std::forward<FunctorOrValue>(other));
    }

    template <class T>
    requires(!is_arfunctor_v<std::remove_cvref_t<T>>) constexpr friend auto
    operator>=(T &&other, functor_type const &fctr) {
      return make_binary_arfunctor<geq>(std::forward<T>(other), fctr);
    }

    // Logical operators

    template <class FunctorOrValue>
    constexpr friend auto operator&&(functor_type const &fctr,
                                     FunctorOrValue &&other) {
      return make_binary_arfunctor<logical_and>(
          fctr, std::forward<FunctorOrValue>(other));
    }

    template <class T>
    requires(!is_arfunctor_v<std::remove_cvref_t<T>>) constexpr friend auto
    operator&&(T &&other, functor_type const &fctr) {
      return make_binary_arfunctor<logical_and>(std::forward<T>(other), fctr);
    }

    template <class FunctorOrValue>
    constexpr friend auto operator||(functor_type const &fctr,
                                     FunctorOrValue &&other) {
      return make_binary_arfunctor<logical_or>(
          fctr, std::forward<FunctorOrValue>(other));
    }

    template <class T>
    requires(!is_arfunctor_v<std::remove_cvref_t<T>>) constexpr friend auto
    operator||(T &&other, functor_type const &fctr) {
      return make_binary_arfunctor<logical_or>(std::forward<T>(other), fctr);
    }

    constexpr friend auto operator!(functor_type const &fctr) {
      return make_unary_arfunctor<notop>(fctr);
    }

    // Bitwise operators

    template <class FunctorOrValue>
    constexpr friend auto operator&(functor_type const &fctr,
                                    FunctorOrValue &&other) {
      return make_binary_arfunctor<bitwise_and>(
          fctr, std::forward<FunctorOrValue>(other));
    }

    template <class T>
    requires(!is_arfunctor_v<std::remove_cvref_t<T>>) constexpr friend auto
    operator&(T &&other, functor_type const &fctr) {
      return make_binary_arfunctor<bitwise_and>(std::forward<T>(other), fctr);
    }

    template <class FunctorOrValue>
    constexpr friend auto operator|(functor_type const &fctr,
                                    FunctorOrValue &&other) {
      return make_binary_arfunctor<bitwise_or>(
          fctr, std::forward<FunctorOrValue>(other));
    }

    template <class T>
    requires(!is_arfunctor_v<std::remove_cvref_t<T>>) constexpr friend auto
    operator|(T &&other, functor_type const &fctr) {
      return make_binary_arfunctor<bitwise_or>(std::forward<T>(other), fctr);
    }

    template <class FunctorOrValue>
    constexpr friend auto operator^(functor_type const &fctr,
                                    FunctorOrValue &&other) {
      return make_binary_arfunctor<bitwise_xor>(
          fctr, std::forward<FunctorOrValue>(other));
    }

    template <class T>
    requires(!is_arfunctor_v<std::remove_cvref_t<T>>) constexpr friend auto
    operator^(T &&other, functor_type const &fctr) {
      return make_binary_arfunctor<bitwise_xor>(std::forward<T>(other), fctr);
    }

    constexpr friend auto operator~(functor_type const &fctr) {
      return make_unary_arfunctor<bitwise_complement>(fctr);
    }

    template <class FunctorOrValue>
    constexpr friend auto operator<<(functor_type const &fctr,
                                     FunctorOrValue &&other) {
      return make_binary_arfunctor<bitwise_shift_left>(
          fctr, std::forward<FunctorOrValue>(other));
    }

    template <class T>
    requires(!is_arfunctor_v<std::remove_cvref_t<T>>) constexpr friend auto
    operator<<(T &&other, functor_type const &fctr) {
      return make_binary_arfunctor<bitwise_shift_left>(std::forward<T>(other),
                                                       fctr);
    }

    template <class FunctorOrValue>
    constexpr friend auto operator>>(functor_type const &fctr,
                                     FunctorOrValue &&other) {
      return make_binary_arfunctor<bitwise_shift_right>(
          fctr, std::forward<FunctorOrValue>(other));
    }

    template <class T>
    requires(!is_arfunctor_v<std::remove_cvref_t<T>>) constexpr friend auto
    operator>>(T &&other, functor_type const &fctr) {
      return make_binary_arfunctor<bitwise_shift_right>(std::forward<T>(other),
                                                        fctr);
    }
  };

  namespace {
    /// Depending on the input argument type, evaluate the functor or return the value
    template <class T, class... Args>
    auto evaluate_arfunctor_or_value(T &&fv, [[maybe_unused]] Args &&... args) {
      if constexpr (is_arfunctor_v<std::remove_cvref_t<T>>)
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
  class binary_arfunctor
      : public arfunctor<
            binary_arfunctor<BinaryOperator, LeftOperand, RightOperand>> {

  public:
    /// Base class type
    using base_class_type =
        arfunctor<binary_arfunctor<BinaryOperator, LeftOperand, RightOperand>>;

    binary_arfunctor(LeftOperand const &lop, RightOperand const &rop)
        : base_class_type{}, m_left_operand{lop}, m_right_operand{rop} {}
    binary_arfunctor() = default;
    binary_arfunctor(binary_arfunctor const &) = default;
    binary_arfunctor(binary_arfunctor &&) = default;
    binary_arfunctor &operator=(binary_arfunctor const &) = default;
    binary_arfunctor &operator=(binary_arfunctor &&) = default;

    /// Operator type
    using binary_operator_type = BinaryOperator;
    /// Left operand type
    using left_operand_type = LeftOperand;
    /// Right operand type
    using right_operand_type = RightOperand;

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
  class unary_arfunctor
      : public arfunctor<unary_arfunctor<UnaryOperator, Operand>> {

  public:
    using base_class_type = arfunctor<unary_arfunctor<UnaryOperator, Operand>>;

    unary_arfunctor(Operand const &op) : base_class_type{}, m_operand{op} {}
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

} // namespace mpt
