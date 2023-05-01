#pragma once
#include "mpt/arfunctors/core/compiletime_arfunctor.hpp"
#include "mpt/arfunctors/core/runtime_arfunctor.hpp"
#include <string_view>
#include <utility>

namespace mpt::arfunctors {

  template <class T> constexpr auto as_arfunctor(T &&obj) {
    if constexpr (core::is_arfunctor_v<std::decay_t<T>>)
      return obj;
    else
      return core::make_arfunctor(std::forward<T>(obj));
  }

  namespace core {

    namespace {

      template <class Operator, class LeftOperand, class RightOperand>
      struct binary_operator_switcher {
        template <class L, class R>
        constexpr auto operator()(L &&lop, R &&rop) const {
          return make_composed_arfunctor<Operator>(
              as_arfunctor(std::forward<L>(lop)),
              as_arfunctor(std::forward<R>(rop)));
        }
      };

      template <class Operator, class Signature, class RightOperand>
      struct binary_operator_switcher<Operator, runtime_arfunctor<Signature>,
                                      RightOperand> {
        template <class L, class R>
        constexpr auto operator()(L &&lop, R &&rop) const {
          return make_runtime_composed_arfunctor<Operator, Signature>(
              std::forward<L>(lop), make_runtime_arfunctor<Signature>(
                                        as_arfunctor(std::forward<R>(rop))));
        }
      };

      template <class Operator, class Signature, class LeftOperand>
      struct binary_operator_switcher<Operator, LeftOperand,
                                      runtime_arfunctor<Signature>> {
        template <class L, class R>
        constexpr auto operator()(L &&lop, R &&rop) const {
          return make_runtime_composed_arfunctor<Operator, Signature>(
              make_runtime_arfunctor<Signature>(
                  as_arfunctor(std::forward<L>(lop))),
              std::forward<R>(rop));
        }
      };

      template <class Operator, class Signature>
      struct binary_operator_switcher<Operator, runtime_arfunctor<Signature>,
                                      runtime_arfunctor<Signature>> {
        template <class L, class R>
        constexpr auto operator()(L &&lop, R &&rop) const {
          return make_runtime_composed_arfunctor<Operator, Signature>(
              std::forward<L>(lop), std::forward<R>(rop));
        }
      };

      template <class Operator, class Operand> struct unary_operator_switcher {
        template <class T> constexpr auto operator()(T &&op) const {
          return make_composed_arfunctor<Operator>(std::forward<T>(op));
        }
      };

      template <class Operator, class Signature>
      struct unary_operator_switcher<Operator, runtime_arfunctor<Signature>> {
        template <class T> constexpr auto operator()(T &&op) const {
          return make_runtime_composed_arfunctor<Operator, Signature>(
              std::forward<T>(op));
        }
      };

      /// Return either a compile-time or run-time functor depending on  the input
      template <class Operator, class LeftOperand, class RightOperand>
      constexpr auto switch_binary_operator(LeftOperand &&lop,
                                            RightOperand &&rop) {
        return binary_operator_switcher<Operator, std::decay_t<LeftOperand>,
                                        std::decay_t<RightOperand>>{}(
            std::forward<LeftOperand>(lop), std::forward<RightOperand>(rop));
      }

      /// Return either a compile-time or run-time functor depending on  the input
      template <class Operator, class Operand>
      constexpr auto switch_unary_operator(Operand &&op) {
        return unary_operator_switcher<Operator, std::decay_t<Operand>>{}(
            std::forward<Operand>(op));
      }

      /// Unary plus
      struct unary_plus {
        static constexpr std::string_view chars = "+";
        template <class Operand> constexpr auto operator()(Operand &&op) const {
          return +std::forward<Operand>(op);
        }
      };
      /// Unary minus
      struct unary_minus {
        static constexpr std::string_view chars = "-";
        template <class Operand> constexpr auto operator()(Operand &&op) const {
          return -std::forward<Operand>(op);
        }
      };
      /// Addition of two objects
      struct add {
        static constexpr std::string_view chars = "+";
        template <class L, class R>
        constexpr auto operator()(L &&lop, R &&rop) const {
          return std::forward<L>(lop) + std::forward<R>(rop);
        }
      };
      /// Subtraction of two objects
      struct sub {
        static constexpr std::string_view chars = "-";
        template <class L, class R>
        constexpr auto operator()(L &&lop, R &&rop) const {
          return std::forward<L>(lop) - std::forward<R>(rop);
        }
      };
      /// Multiplication of two objects
      struct mul {
        static constexpr std::string_view chars = "*";
        template <class L, class R>
        constexpr auto operator()(L &&lop, R &&rop) const {
          return std::forward<L>(lop) * std::forward<R>(rop);
        }
      };
      /// Division of two objects
      struct div {
        static constexpr std::string_view chars = "/";
        template <class L, class R>
        constexpr auto operator()(L &&lop, R &&rop) const {
          return std::forward<L>(lop) / std::forward<R>(rop);
        }
      };
      /// Modulo of two objects
      struct modulo {
        static constexpr std::string_view chars = "%";
        template <class L, class R>
        constexpr auto operator()(L &&lop, R &&rop) const {
          return std::forward<L>(lop) % std::forward<R>(rop);
        }
      };
      /// Whether the left operand is less than the right operand
      struct lt {
        static constexpr std::string_view chars = "<";
        template <class L, class R>
        constexpr auto operator()(L &&lop, R &&rop) {
          return std::forward<L>(lop) < std::forward<R>(rop);
        }
      };
      /// Whether the left operand is less or equal than the right operand
      struct leq {
        static constexpr std::string_view chars = "<=";
        template <class L, class R>
        constexpr auto operator()(L &&lop, R &&rop) const {
          return std::forward<L>(lop) <= std::forward<R>(rop);
        }
      };
      /// Whether the left operand is greater than the right operand
      struct gt {
        static constexpr std::string_view chars = ">";
        template <class L, class R>
        constexpr auto operator()(L &&lop, R &&rop) const {
          return std::forward<L>(lop) > std::forward<R>(rop);
        }
      };
      /// Whether the left operand is greater or equal than the right operand
      struct geq {
        static constexpr std::string_view chars = ">=";
        template <class L, class R>
        constexpr auto operator()(L &&lop, R &&rop) const {
          return std::forward<L>(lop) >= std::forward<R>(rop);
        }
      };
      /// Whether the left operand is equal to the right operand
      struct eq {
        static constexpr std::string_view chars = "==";
        template <class L, class R>
        constexpr auto operator()(L &&lop, R &&rop) const {
          return std::forward<L>(lop) == std::forward<R>(rop);
        }
      };
      /// Whether the left operand is not equal to the right operand
      struct neq {
        static constexpr std::string_view chars = "!=";
        template <class L, class R>
        constexpr auto operator()(L &&lop, R &&rop) const {
          return std::forward<L>(lop) != std::forward<R>(rop);
        }
      };
      /// Logical and operation between two operands
      struct logical_and {
        static constexpr std::string_view chars = "&&";
        template <class L, class R>
        constexpr auto operator()(L &&lop, R &&rop) const {
          return std::forward<L>(lop) && std::forward<R>(rop);
        }
      };
      /// Logical or operation between two operands
      struct logical_or {
        static constexpr std::string_view chars = "||";
        template <class L, class R>
        constexpr auto operator()(L &&lop, R &&rop) const {
          return std::forward<L>(lop) || std::forward<R>(rop);
        }
      };
      /// Logical not operation of an operand
      struct notop {
        static constexpr std::string_view chars = "!";
        template <class Operand> constexpr auto operator()(Operand &&op) const {
          return !std::forward<Operand>(op);
        }
      };
      /// Bitwise and operation between two operands
      struct bitwise_and {
        static constexpr std::string_view chars = "&";
        template <class L, class R>
        constexpr auto operator()(L &&lop, R &&rop) const {
          return std::forward<L>(lop) & std::forward<R>(rop);
        }
      };
      /// Bitwise or operation between two operands
      struct bitwise_or {
        static constexpr std::string_view chars = "|";
        template <class L, class R>
        constexpr auto operator()(L &&lop, R &&rop) const {
          return std::forward<L>(lop) | std::forward<R>(rop);
        }
      };
      /// Bitwise xor operation between two operands
      struct bitwise_xor {
        static constexpr std::string_view chars = "^";
        template <class L, class R>
        constexpr auto operator()(L &&lop, R &&rop) const {
          return std::forward<L>(lop) ^ std::forward<R>(rop);
        }
      };
      /// Bitwise complement of an operand
      struct bitwise_complement {
        static constexpr std::string_view chars = "~";
        template <class Operand> constexpr auto operator()(Operand &&op) const {
          return ~std::forward<Operand>(op);
        }
      };
      /// Bitwise left shift operation between two operands
      struct bitwise_shift_left {
        static constexpr std::string_view chars = "<<";
        template <class L, class R>
        constexpr auto operator()(L &&lop, R &&rop) const {
          return std::forward<L>(lop) << std::forward<R>(rop);
        }
      };
      /// Bitwise right shift operation between two operands
      struct bitwise_shift_right {
        static constexpr std::string_view chars = ">>";
        template <class L, class R>
        constexpr auto operator()(L &&lop, R &&rop) const {
          return std::forward<L>(lop) >> std::forward<R>(rop);
        }
      };

      using binary_arithmetic_operators =
          mpt::types<add, sub, mul, div, modulo>;
      using binary_relational_operators = mpt::types<lt, leq, gt, geq, eq, neq>;
      using binary_logical_operators = mpt::types<logical_and, logical_or>;
      using binary_bitwise_comparison_operators =
          mpt::types<bitwise_and, bitwise_or, bitwise_xor>;
      using binary_bitwise_shift_operators =
          mpt::types<bitwise_shift_left, bitwise_shift_right>;
      using binary_operators = mpt::concatenate_types_t<
          binary_arithmetic_operators, binary_relational_operators,
          binary_logical_operators, binary_bitwise_comparison_operators,
          binary_bitwise_shift_operators>;
      using unary_operators =
          mpt::types<unary_plus, unary_minus, notop, bitwise_complement>;

      template <class Operator>
      concept IsBinaryOperator =
          mpt::templated_object_has_type_v<Operator, binary_operators>;

      template <class Operator>
      concept IsUnaryOperator =
          mpt::templated_object_has_type_v<Operator, unary_operators>;

      template <class Operator>
      concept IsArithmeticOperator =
          mpt::templated_object_has_type_v<Operator,
                                           binary_arithmetic_operators>;

      template <class Operator>
      concept IsRelationalOperator =
          mpt::templated_object_has_type_v<Operator,
                                           binary_relational_operators>;
    } // namespace

    // Arithmetic operators

    template <class Operand> constexpr auto operator+(Operand &&op) {
      return switch_unary_operator<unary_plus>(std::forward<Operand>(op));
    }

    template <class Operand> constexpr auto operator-(Operand &&op) {
      return switch_unary_operator<unary_minus>(std::forward<Operand>(op));
    }

    template <class LeftOperand, class RightOperand>
    constexpr auto operator+(LeftOperand &&lop, RightOperand &&rop) {
      return switch_binary_operator<add>(std::forward<LeftOperand>(lop),
                                         std::forward<RightOperand>(rop));
    }

    template <class LeftOperand, class RightOperand>
    constexpr auto operator-(LeftOperand &&lop, RightOperand &&rop) {
      return switch_binary_operator<sub>(std::forward<LeftOperand>(lop),
                                         std::forward<RightOperand>(rop));
    }

    template <class LeftOperand, class RightOperand>
    constexpr auto operator*(LeftOperand &&lop, RightOperand &&rop) {
      return switch_binary_operator<mul>(std::forward<LeftOperand>(lop),
                                         std::forward<RightOperand>(rop));
    }

    template <class LeftOperand, class RightOperand>
    constexpr auto operator/(LeftOperand &&lop, RightOperand &&rop) {
      return switch_binary_operator<div>(std::forward<LeftOperand>(lop),
                                         std::forward<RightOperand>(rop));
    }

    template <class LeftOperand, class RightOperand>
    constexpr auto operator%(LeftOperand &&lop, RightOperand &&rop) {
      return switch_binary_operator<modulo>(std::forward<LeftOperand>(lop),
                                            std::forward<RightOperand>(rop));
    }

    // Relational operators

    template <class LeftOperand, class RightOperand>
    constexpr auto operator==(LeftOperand &&lop, RightOperand &&rop) {
      return switch_binary_operator<eq>(std::forward<LeftOperand>(lop),
                                        std::forward<RightOperand>(rop));
    }

    template <class LeftOperand, class RightOperand>
    constexpr auto operator!=(LeftOperand &&lop, RightOperand &&rop) {
      return switch_binary_operator<neq>(std::forward<LeftOperand>(lop),
                                         std::forward<RightOperand>(rop));
    }

    template <class LeftOperand, class RightOperand>
    constexpr auto operator<(LeftOperand &&lop, RightOperand &&rop) {
      return switch_binary_operator<lt>(std::forward<LeftOperand>(lop),
                                        std::forward<RightOperand>(rop));
    }

    template <class LeftOperand, class RightOperand>
    constexpr auto operator<=(LeftOperand &&lop, RightOperand &&rop) {
      return switch_binary_operator<leq>(std::forward<LeftOperand>(lop),
                                         std::forward<RightOperand>(rop));
    }

    template <class LeftOperand, class RightOperand>
    constexpr auto operator>(LeftOperand &&lop, RightOperand &&rop) {
      return switch_binary_operator<gt>(std::forward<LeftOperand>(lop),
                                        std::forward<RightOperand>(rop));
    }

    template <class LeftOperand, class RightOperand>
    constexpr auto operator>=(LeftOperand &&lop, RightOperand &&rop) {
      return switch_binary_operator<geq>(std::forward<LeftOperand>(lop),
                                         std::forward<RightOperand>(rop));
    }

    // Logical operators

    template <class LeftOperand, class RightOperand>
    constexpr auto operator&&(LeftOperand &&lop, RightOperand &&rop) {
      return switch_binary_operator<logical_and>(
          std::forward<LeftOperand>(lop), std::forward<RightOperand>(rop));
    }

    template <class LeftOperand, class RightOperand>
    constexpr auto operator||(LeftOperand &&lop, RightOperand &&rop) {
      return switch_binary_operator<logical_or>(
          std::forward<LeftOperand>(lop), std::forward<RightOperand>(rop));
    }

    template <class Operand> constexpr auto operator!(Operand &&op) {
      return switch_unary_operator<notop>(std::forward<Operand>(op));
    }

    // Bitwise operators

    template <class LeftOperand, class RightOperand>
    constexpr auto operator&(LeftOperand &&lop, RightOperand &&rop) {
      return switch_binary_operator<bitwise_and>(
          std::forward<LeftOperand>(lop), std::forward<RightOperand>(rop));
    }

    template <class LeftOperand, class RightOperand>
    constexpr auto operator|(LeftOperand &&lop, RightOperand &&rop) {
      return switch_binary_operator<bitwise_or>(
          std::forward<LeftOperand>(lop), std::forward<RightOperand>(rop));
    }

    template <class LeftOperand, class RightOperand>
    constexpr auto operator^(LeftOperand &&lop, RightOperand &&rop) {
      return switch_binary_operator<bitwise_xor>(
          std::forward<LeftOperand>(lop), std::forward<RightOperand>(rop));
    }

    template <class Operand> constexpr auto operator~(Operand &&op) {
      return switch_unary_operator<bitwise_complement>(
          std::forward<Operand>(op));
    }

    template <class LeftOperand, class RightOperand>
    constexpr auto operator<<(LeftOperand &&lop, RightOperand &&rop) {
      return switch_binary_operator<bitwise_shift_left>(
          std::forward<LeftOperand>(lop), std::forward<RightOperand>(rop));
    }

    template <class LeftOperand, class RightOperand>
    constexpr auto operator>>(LeftOperand &&lop, RightOperand &&rop) {
      return switch_binary_operator<bitwise_shift_right>(
          std::forward<LeftOperand>(lop), std::forward<RightOperand>(rop));
    }

  } // namespace core
} // namespace mpt::arfunctors