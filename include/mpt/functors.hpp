#pragma once
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

  namespace {
    /// Function to build a binary functor from the operation and the operands
    template <class BinaryOperator, class LeftOperand, class RightOperand>
    binary_arfunctor<BinaryOperator, LeftOperand, RightOperand>
    make_binary_arfunctor(LeftOperand &&lop, RightOperand &&rop) {
      return {std::forward<LeftOperand>(lop), std::forward<RightOperand>(rop)};
    }

    /// Function to build an unary functor from the operation and the operands
    template <class UnaryOperator, class Operand>
    unary_arfunctor<UnaryOperator, Operand> make_unary_arfunctor(Operand &&op) {
      return {std::forward<Operand>(op)};
    }
  } // namespace

  /*!\brief Base functor class

    This class defines the basic functionality of any arithmetic and relational functor.
    Objects inheriting from this class must define a constant call operator.
   */
  template <class DerivedFunctor> struct arfunctor {

    using functor_type = DerivedFunctor;

    // Arithmetic operators

    template <class FunctorOrValue>
    constexpr auto operator+(FunctorOrValue &&other) const {
      return make_binary_arfunctor<add>(
          *static_cast<functor_type const *>(this),
          std::forward<FunctorOrValue>(other));
    }

    template <class FunctorOrValue>
    constexpr auto operator-(FunctorOrValue &&other) const {
      return make_binary_arfunctor<sub>(
          *static_cast<functor_type const *>(this),
          std::forward<FunctorOrValue>(other));
    }

    template <class FunctorOrValue>
    constexpr auto operator*(FunctorOrValue &&other) const {
      return make_binary_arfunctor<mul>(
          *static_cast<functor_type const *>(this),
          std::forward<FunctorOrValue>(other));
    }

    template <class FunctorOrValue>
    constexpr auto operator/(FunctorOrValue &&other) const {
      return make_binary_arfunctor<div>(
          *static_cast<functor_type const *>(this),
          std::forward<FunctorOrValue>(other));
    }

    template <class FunctorOrValue>
    constexpr auto operator%(FunctorOrValue &&other) const {
      return make_binary_arfunctor<modulo>(
          *static_cast<functor_type const *>(this),
          std::forward<FunctorOrValue>(other));
    }

    // Relational operators

    template <class FunctorOrValue>
    constexpr auto operator==(FunctorOrValue &&other) const {
      return make_binary_arfunctor<eq>(*static_cast<functor_type const *>(this),
                                       std::forward<FunctorOrValue>(other));
    }

    template <class FunctorOrValue>
    constexpr auto operator!=(FunctorOrValue &&other) const {
      return make_binary_arfunctor<neq>(
          *static_cast<functor_type const *>(this),
          std::forward<FunctorOrValue>(other));
    }

    template <class FunctorOrValue>
    constexpr auto operator<(FunctorOrValue &&other) const {
      return make_binary_arfunctor<lt>(*static_cast<functor_type const *>(this),
                                       std::forward<FunctorOrValue>(other));
    }

    template <class FunctorOrValue>
    constexpr auto operator<=(FunctorOrValue &&other) const {
      return make_binary_arfunctor<leq>(
          *static_cast<functor_type const *>(this),
          std::forward<FunctorOrValue>(other));
    }

    template <class FunctorOrValue>
    constexpr auto operator>(FunctorOrValue &&other) const {
      return make_binary_arfunctor<gt>(*static_cast<functor_type const *>(this),
                                       std::forward<FunctorOrValue>(other));
    }

    template <class FunctorOrValue>
    constexpr auto operator>=(FunctorOrValue &&other) const {
      return make_binary_arfunctor<geq>(
          *static_cast<functor_type const *>(this),
          std::forward<FunctorOrValue>(other));
    }

    // Logical operators

    template <class FunctorOrValue>
    constexpr auto operator&&(FunctorOrValue &&other) const {
      return make_binary_arfunctor<logical_and>(
          *static_cast<functor_type const *>(this),
          std::forward<FunctorOrValue>(other));
    }

    template <class FunctorOrValue>
    constexpr auto operator||(FunctorOrValue &&other) const {
      return make_binary_arfunctor<logical_or>(
          *static_cast<functor_type const *>(this),
          std::forward<FunctorOrValue>(other));
    }

    constexpr auto operator!() const {
      return make_unary_arfunctor<notop>(
          *static_cast<functor_type const *>(this));
    }

    // Bitwise operators

    template <class FunctorOrValue>
    constexpr auto operator&(FunctorOrValue &&other) const {
      return make_binary_arfunctor<bitwise_and>(
          *static_cast<functor_type const *>(this),
          std::forward<FunctorOrValue>(other));
    }

    template <class FunctorOrValue>
    constexpr auto operator|(FunctorOrValue &&other) const {
      return make_binary_arfunctor<bitwise_or>(
          *static_cast<functor_type const *>(this),
          std::forward<FunctorOrValue>(other));
    }

    template <class FunctorOrValue>
    constexpr auto operator^(FunctorOrValue &&other) const {
      return make_binary_arfunctor<bitwise_xor>(
          *static_cast<functor_type const *>(this),
          std::forward<FunctorOrValue>(other));
    }

    constexpr auto operator~() const {
      return make_unary_arfunctor<bitwise_complement>(
          *static_cast<functor_type const *>(this));
    }

    template <class FunctorOrValue>
    constexpr auto operator<<(FunctorOrValue &&other) const {
      return make_binary_arfunctor<bitwise_shift_left>(
          *static_cast<functor_type const *>(this),
          std::forward<FunctorOrValue>(other));
    }

    template <class FunctorOrValue>
    constexpr auto operator>>(FunctorOrValue &&other) const {
      return make_binary_arfunctor<bitwise_shift_right>(
          *static_cast<functor_type const *>(this),
          std::forward<FunctorOrValue>(other));
    }
  };

  template <class T> struct is_arfunctor {
    static constexpr auto value = std::is_base_of_v<arfunctor<T>, T>;
  };

  template <class T>
  static constexpr auto is_arfunctor_v = is_arfunctor<T>::value;

  namespace {

    template <class T, class... Args>
    auto evaluate_arfunctor_or_value(T &&fv, [[maybe_unused]] Args &&... args) {
      if constexpr (is_arfunctor_v<std::remove_cvref_t<T>>)
        return fv(args...);
      else
        return fv;
    }
  } // namespace

  template <class BinaryOperator, class LeftOperand, class RightOperand>
  class binary_arfunctor
      : public arfunctor<
            binary_arfunctor<BinaryOperator, LeftOperand, RightOperand>> {

  public:
    using base_class_type =
        arfunctor<binary_arfunctor<BinaryOperator, LeftOperand, RightOperand>>;

    binary_arfunctor(LeftOperand &&lop, RightOperand &&rop)
        : base_class_type{}, m_left_operand{std::forward<LeftOperand>(lop)},
          m_right_operand{std::forward<RightOperand>(rop)} {}
    binary_arfunctor() = default;
    binary_arfunctor(binary_arfunctor const &) = default;
    binary_arfunctor(binary_arfunctor &&) = default;
    binary_arfunctor &operator=(binary_arfunctor const &) = default;
    binary_arfunctor &operator=(binary_arfunctor &&) = default;

    using binary_operator_type = BinaryOperator;
    using left_operand_type = LeftOperand;
    using right_operand_type = RightOperand;

    template <class... Args> constexpr auto operator()(Args &&... args) const {
      return binary_operator_type{}(
          evaluate_arfunctor_or_value(m_left_operand, args...),
          evaluate_arfunctor_or_value(m_right_operand, args...));
    }

  private:
    left_operand_type m_left_operand;
    right_operand_type m_right_operand;
  };

  template <class UnaryOperator, class Operand>
  class unary_arfunctor
      : public arfunctor<unary_arfunctor<UnaryOperator, Operand>> {

  public:
    using base_class_type = arfunctor<unary_arfunctor<UnaryOperator, Operand>>;

    unary_arfunctor(Operand &&op)
        : base_class_type{}, m_operand{std::forward<Operand>(op)} {}
    unary_arfunctor() = default;
    unary_arfunctor(unary_arfunctor const &) = default;
    unary_arfunctor(unary_arfunctor &&) = default;
    unary_arfunctor &operator=(unary_arfunctor const &) = default;
    unary_arfunctor &operator=(unary_arfunctor &&) = default;

    using unary_operator_type = UnaryOperator;
    using operand_type = Operand;

    constexpr auto operator()() const {
      return unary_operator_type{}(evaluate(m_operand));
    }

  private:
    operand_type m_operand;
  };

} // namespace mpt
