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
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

#include <iostream>

namespace mpt {

  namespace {
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
      template <class L, class R> constexpr auto operator()(L &&lop, R &&rop) {
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
      template <class L, class R> constexpr auto operator()(L &&lop, R &&rop) {
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

    using binary_arithmetic_operators = mpt::types<add, sub, mul, div, modulo>;
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
        mpt::templated_object_has_type_v<Operator, binary_arithmetic_operators>;

    template <class Operator>
    concept IsRelationalOperator =
        mpt::templated_object_has_type_v<Operator, binary_relational_operators>;
  } // namespace

  /*!\brief Base arithmetic and relational functor class

    This class defines the basic functionality of any arithmetic and relational functor.
    Objects inheriting from this class must define a constant call operator.
   */
  struct arfunctor {};

#ifndef MPT_DOXYGEN_WARD
  template <class Signature> struct arfunctor_wrapper;
#endif

  /*!\brief Abstract runtime arithmetic and relation functor wrapper

    This is the base type for any run-time functor wrapper.
    It allows to call any functor using polymorphism.
   */
  template <class Output, class... Input>
  struct arfunctor_wrapper<Output(Input...)> {
  public:
    virtual ~arfunctor_wrapper() {}

    /// Force a signature to be used to call the functor
    virtual Output operator()(Input const &...) const = 0;

    /*!\brief Represent the functor as a string

      This is used for parsing functors from strings.
      A runtime instance whose underlying functor does
      not have a corresponding conversion function will
      lead to a runtime error.
     */
    virtual std::string as_string() const = 0;

    /// Provide a clone of the wrapper
    virtual arfunctor_wrapper *clone() const = 0;
  };

  namespace {

    /*!\brief Runtime arithmetic and relation functor wrapper

    This object wraps any functor type without needing to inherit from
    any additional class (i.e. allows to work directly with
    \ref mpt::arfunctor objects).
   */
    template <class Functor, class Output, class... Input>
    class specialized_arfunctor_wrapper
        : public arfunctor_wrapper<Output(Input...)> {

    public:
      specialized_arfunctor_wrapper() = default;
      specialized_arfunctor_wrapper(Functor const &functor)
          : m_functor{functor} {}
      specialized_arfunctor_wrapper(Functor &&functor)
          : m_functor{std::move(functor)} {}
      specialized_arfunctor_wrapper(specialized_arfunctor_wrapper const &) =
          default;
      specialized_arfunctor_wrapper(specialized_arfunctor_wrapper &&) = default;
      specialized_arfunctor_wrapper &
      operator=(specialized_arfunctor_wrapper const &) = default;
      specialized_arfunctor_wrapper &
      operator=(specialized_arfunctor_wrapper &&) = default;

      /// Call the wrapped functor
      Output operator()(Input const &... args) const override {
        return m_functor(args...);
      }

      /// Return a clone of this object
      arfunctor_wrapper<Output(Input...)> *clone() const override {
        return new specialized_arfunctor_wrapper{*this};
      }

      /// Represent the functor as a string
      std::string as_string() const override { return to_string(m_functor); }

    private:
      /// Wrapped functor
      Functor m_functor;
    };

    /// Check if the provided type is an arithmetic and relational functor
    template <class T>
    struct is_arfunctor
        : std::conditional_t<
              std::is_base_of_v<arfunctor, std::remove_cvref_t<T>>,
              std::true_type, std::false_type> {};

    /// Whether the provided type is an arithmetic and relational functor
    template <class T>
    static constexpr auto is_arfunctor_v = is_arfunctor<T>::value;
  } // namespace

  template <class Signature> class runtime_arfunctor;

  /*!\brief Run-time arithmetic and relational functor

    This object is a run-time wrapper of any \ref mpt::arfunctor object.
    It allows to perform arithmetic and relational operations among functors
    at run-time.
    This requires to define the signature for which the functor will
    be called.
   */
  template <class Output, class... Input>
  class runtime_arfunctor<Output(Input...)> {

  public:
    runtime_arfunctor() = delete;

    /// Build the class from an arithmetic and relational functor
    template <class Functor>
    requires is_arfunctor_v<Functor> runtime_arfunctor(Functor &&functor)
        : m_ptr{new specialized_arfunctor_wrapper<std::remove_cvref_t<Functor>,
                                                  Output, Input...>{
              std::forward<Functor>(functor)}} {}

    runtime_arfunctor(runtime_arfunctor const &other)
        : m_ptr{other.m_ptr->clone()} {}

    runtime_arfunctor(runtime_arfunctor &&other) : m_ptr{other.m_ptr} {
      other.m_ptr = nullptr;
    }

    runtime_arfunctor &operator=(runtime_arfunctor const &other) {

      if (m_ptr)
        delete m_ptr;

      m_ptr = other.m_ptr->clone();
    }

    runtime_arfunctor &operator=(runtime_arfunctor &&other) {
      m_ptr = other.m_ptr;
      other.m_ptr = nullptr;
    }

    ~runtime_arfunctor() {
      if (m_ptr)
        delete m_ptr;
    }

    /// Call the internal functor
    Output operator()(Input const &... args) const {
      return m_ptr->operator()(args...);
    }

    /// Conversion to \ref std::string
    friend std::string to_string(runtime_arfunctor<Output(Input...)> const &f) {
      return f.m_ptr->as_string();
    }

  private:
    /// Pointer to the internal functor wrapper
    arfunctor_wrapper<Output(Input...)> *m_ptr = nullptr;
  };

  namespace {
    /// Check if the type refers to a runtime arithmetic and relational functor
    template <class T> struct is_runtime_arfunctor_impl : std::false_type {};

    /// Check if the type refers to a runtime arithmetic and relational functor
    template <class Output, class... Input>
    struct is_runtime_arfunctor_impl<runtime_arfunctor<Output(Input...)>>
        : std::true_type {};

    /// Check if the type refers to a runtime arithmetic and relational functor
    template <class T>
    struct is_runtime_arfunctor
        : is_runtime_arfunctor_impl<std::remove_cvref_t<T>> {};

    /// Whether the type refers to a runtime arithmetic and relational functor
    template <class T>
    static constexpr auto is_runtime_arfunctor_v =
        is_runtime_arfunctor<T>::value;

    /// Depending on the input argument type, evaluate the functor or return the value
    template <class T, class... Args>
    auto evaluate_arfunctor_or_value(T &&fv, [[maybe_unused]] Args &&... args) {
      if constexpr (is_arfunctor_v<T> || is_runtime_arfunctor_v<T>)
        return fv(args...);
      else
        return fv;
    }

    /// Class to create arfunctor objects that act at runtime
    template <class Signature, class Functor> struct make_runtime_arfunctor_t;

    /// Class to create arfunctor objects that act at runtime
    template <class Output, class Functor, class... Input>
    struct make_runtime_arfunctor_t<Output(Input...), Functor> {
      runtime_arfunctor<Output(Input...)> operator()(Functor &&functor) {
        return {std::move<Functor>(functor)};
      }
      runtime_arfunctor<Output(Input...)> operator()(Functor const &functor) {
        return {functor};
      }
    };
  } // namespace

  /*!\brief Create a run-time arithmetic and relational functor

    The call to this function will return a \ref mpt::runtime_arfunctor object, with
    a similar functionality as any \ref mpt::arfunctor object but where the
    types can be determined at runtime.
    This means that functors are allocated and stored as pointers using
    polymorphism internally.
    It is possible to combine compile-time and run-time functors, resulting in
    a run-time functor with pointers to both of them.
    It is recommended to make use of compile-time functors as much as possible
    to avoid allocations that can be avoided.
   */
  template <class Signature, class Functor>
  requires is_arfunctor_v<Functor> auto
  make_runtime_arfunctor(Functor &&functor) {
    return make_runtime_arfunctor_t<Signature, std::remove_cvref_t<Functor>>{}(
        std::forward<Functor>(functor));
  }

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

    operands_type const &operands() const { return m_operands; }

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

  /// Build a composed functor from the operator and the operand types
  template <class Operator, class... Operand>
  constexpr composed_arfunctor<Operator, std::remove_cvref_t<Operand>...>
  make_composed_arfunctor(Operand &&... op) {
    return {std::forward<Operand>(op)...};
  }

  namespace {
    /// Build a composed functor from the operator and the operand types
    template <class Operator, class FunctorType, class... Operand>
    FunctorType make_runtime_composed_arfunctor(Operand &&... op) {
      return FunctorType{
          composed_arfunctor<Operator, std::remove_cvref_t<Operand>...>{
              std::forward<Operand>(op)...}};
    }

    /// Check that at least one of the operands is a functor
    template <class... Operands>
    struct constrain_to_arfunctor_types
        : std::conditional_t<((is_arfunctor_v<Operands> ||
                               is_runtime_arfunctor_v<Operands>) ||
                              ...),
                             std::true_type, std::false_type> {};

    /// Whether one of the operands is a functor
    template <class... Operands>
    static constexpr auto constrain_to_arfunctor_types_v =
        constrain_to_arfunctor_types<Operands...>::value;

    /// Whether an operation between two types can be resolved with arfunctor operators
    template <class... Operands>
    concept ValidArithmeticOrRelationalOperands =
        constrain_to_arfunctor_types_v<Operands...>;

    /// Return either a compile-time or run-time functor depending on  the input
    template <class Operator, class LeftOperand, class RightOperand>
    constexpr auto switch_binary_operator(LeftOperand &&lop,
                                          RightOperand &&rop) {
      if constexpr (is_runtime_arfunctor_v<LeftOperand> ||
                    is_runtime_arfunctor_v<RightOperand>) {
        using functor_type =
            std::conditional_t<is_runtime_arfunctor_v<LeftOperand>,
                               std::remove_cvref_t<LeftOperand>,
                               std::remove_cvref_t<RightOperand>>;
        return make_runtime_composed_arfunctor<Operator, functor_type>(
            std::forward<LeftOperand>(lop), std::forward<RightOperand>(rop));
      } else
        return make_composed_arfunctor<Operator>(
            std::forward<LeftOperand>(lop), std::forward<RightOperand>(rop));
    }

    /// Return either a compile-time or run-time functor depending on  the input
    template <class Operator, class Operand>
    constexpr auto switch_unary_operator(Operand &&op) {
      if constexpr (is_runtime_arfunctor_v<Operand>)
        return make_runtime_composed_arfunctor<Operator,
                                               std::remove_cvref_t<Operand>>(
            std::forward<Operand>(op));
      else
        return make_composed_arfunctor<Operator>(std::forward<Operand>(op));
    }
  } // namespace

  //
  // Compile-time operators
  //

  // Arithmetic operators

  template <class Operand>
  requires ValidArithmeticOrRelationalOperands<Operand> constexpr auto
  operator+(Operand &&op) {
    return switch_unary_operator<unary_plus>(std::forward<Operand>(op));
  }

  template <class Operand>
  requires ValidArithmeticOrRelationalOperands<Operand> constexpr auto
  operator-(Operand &&op) {
    return switch_unary_operator<unary_minus>(std::forward<Operand>(op));
  }

  template <class LeftOperand, class RightOperand>
  requires ValidArithmeticOrRelationalOperands<LeftOperand,
                                               RightOperand> constexpr auto
  operator+(LeftOperand &&lop, RightOperand &&rop) {
    return switch_binary_operator<add>(std::forward<LeftOperand>(lop),
                                       std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires ValidArithmeticOrRelationalOperands<LeftOperand,
                                               RightOperand> constexpr auto
  operator-(LeftOperand &&lop, RightOperand &&rop) {
    return switch_binary_operator<sub>(std::forward<LeftOperand>(lop),
                                       std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires ValidArithmeticOrRelationalOperands<LeftOperand,
                                               RightOperand> constexpr auto
  operator*(LeftOperand &&lop, RightOperand &&rop) {
    return switch_binary_operator<mul>(std::forward<LeftOperand>(lop),
                                       std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires ValidArithmeticOrRelationalOperands<LeftOperand,
                                               RightOperand> constexpr auto
  operator/(LeftOperand &&lop, RightOperand &&rop) {
    return switch_binary_operator<div>(std::forward<LeftOperand>(lop),
                                       std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires ValidArithmeticOrRelationalOperands<LeftOperand,
                                               RightOperand> constexpr auto
  operator%(LeftOperand &&lop, RightOperand &&rop) {
    return switch_binary_operator<modulo>(std::forward<LeftOperand>(lop),
                                          std::forward<RightOperand>(rop));
  }

  // Relational operators

  template <class LeftOperand, class RightOperand>
  requires ValidArithmeticOrRelationalOperands<LeftOperand,
                                               RightOperand> constexpr auto
  operator==(LeftOperand &&lop, RightOperand &&rop) {
    return switch_binary_operator<eq>(std::forward<LeftOperand>(lop),
                                      std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires ValidArithmeticOrRelationalOperands<LeftOperand,
                                               RightOperand> constexpr auto
  operator!=(LeftOperand &&lop, RightOperand &&rop) {
    return switch_binary_operator<neq>(std::forward<LeftOperand>(lop),
                                       std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires ValidArithmeticOrRelationalOperands<LeftOperand,
                                               RightOperand> constexpr auto
  operator<(LeftOperand &&lop, RightOperand &&rop) {
    return switch_binary_operator<lt>(std::forward<LeftOperand>(lop),
                                      std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires ValidArithmeticOrRelationalOperands<LeftOperand,
                                               RightOperand> constexpr auto
  operator<=(LeftOperand &&lop, RightOperand &&rop) {
    return switch_binary_operator<leq>(std::forward<LeftOperand>(lop),
                                       std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires ValidArithmeticOrRelationalOperands<LeftOperand,
                                               RightOperand> constexpr auto
  operator>(LeftOperand &&lop, RightOperand &&rop) {
    return switch_binary_operator<gt>(std::forward<LeftOperand>(lop),
                                      std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires ValidArithmeticOrRelationalOperands<LeftOperand,
                                               RightOperand> constexpr auto
  operator>=(LeftOperand &&lop, RightOperand &&rop) {
    return switch_binary_operator<geq>(std::forward<LeftOperand>(lop),
                                       std::forward<RightOperand>(rop));
  }

  // Logical operators

  template <class LeftOperand, class RightOperand>
  requires ValidArithmeticOrRelationalOperands<LeftOperand,
                                               RightOperand> constexpr auto
  operator&&(LeftOperand &&lop, RightOperand &&rop) {
    return switch_binary_operator<logical_and>(std::forward<LeftOperand>(lop),
                                               std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires ValidArithmeticOrRelationalOperands<LeftOperand,
                                               RightOperand> constexpr auto
  operator||(LeftOperand &&lop, RightOperand &&rop) {
    return switch_binary_operator<logical_or>(std::forward<LeftOperand>(lop),
                                              std::forward<RightOperand>(rop));
  }

  template <class Operand>
  requires ValidArithmeticOrRelationalOperands<Operand> constexpr auto
  operator!(Operand &&op) {
    return switch_unary_operator<notop>(std::forward<Operand>(op));
  }

  // Bitwise operators

  template <class LeftOperand, class RightOperand>
  requires ValidArithmeticOrRelationalOperands<LeftOperand,
                                               RightOperand> constexpr auto
  operator&(LeftOperand &&lop, RightOperand &&rop) {
    return switch_binary_operator<bitwise_and>(std::forward<LeftOperand>(lop),
                                               std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires ValidArithmeticOrRelationalOperands<LeftOperand,
                                               RightOperand> constexpr auto
  operator|(LeftOperand &&lop, RightOperand &&rop) {
    return switch_binary_operator<bitwise_or>(std::forward<LeftOperand>(lop),
                                              std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires ValidArithmeticOrRelationalOperands<LeftOperand,
                                               RightOperand> constexpr auto
  operator^(LeftOperand &&lop, RightOperand &&rop) {
    return switch_binary_operator<bitwise_xor>(std::forward<LeftOperand>(lop),
                                               std::forward<RightOperand>(rop));
  }

  template <class Operand>
  requires ValidArithmeticOrRelationalOperands<Operand> constexpr auto
  operator~(Operand &&op) {
    return switch_unary_operator<bitwise_complement>(std::forward<Operand>(op));
  }

  template <class LeftOperand, class RightOperand>
  requires ValidArithmeticOrRelationalOperands<LeftOperand,
                                               RightOperand> constexpr auto
  operator<<(LeftOperand &&lop, RightOperand &&rop) {
    return switch_binary_operator<bitwise_shift_left>(
        std::forward<LeftOperand>(lop), std::forward<RightOperand>(rop));
  }

  template <class LeftOperand, class RightOperand>
  requires ValidArithmeticOrRelationalOperands<LeftOperand,
                                               RightOperand> constexpr auto
  operator>>(LeftOperand &&lop, RightOperand &&rop) {
    return switch_binary_operator<bitwise_shift_right>(
        std::forward<LeftOperand>(lop), std::forward<RightOperand>(rop));
  }

  //
  // String conversion
  //
  namespace {

    /// Allow to do conversion to \ref std::string for arithmetic types
    template <class T> std::string to_string(T const &t) {
      return std::to_string(t);
    }

    /// Concatenate strings
    template <class... String> std::string concatenate_strings(String &&... s) {
      std::string result;
      result.reserve((std::size(s) + ...));
      ((result += std::forward<String>(s)), ...);
      return result;
    }

    static constexpr auto space = std::string_view{" "};
    static constexpr auto lpar = std::string_view{"("};
    static constexpr auto rpar = std::string_view{")"};

    /// Convert a call to an operator (with several operands) to a string
    template <class Operator, class... Operand, std::size_t... I>
    std::string
    to_string_impl(composed_arfunctor<Operator, Operand...> const &f,
                   std::index_sequence<I...>) {
      return concatenate_strings(Operator::chars, lpar,
                                 to_string(std::get<I>(f.operands()))..., rpar);
    }

    /// Check if an operand is a composed functor
    template <class Operand> struct is_composed_arfunctor : std::false_type {};

    /// Check if an operand is a composed functor
    template <class Operator, class... Operand>
    struct is_composed_arfunctor<composed_arfunctor<Operator, Operand...>>
        : std::true_type {};

    /// Whether an operand is a composed functor or not
    template <class Operand>
    static constexpr auto is_composed_arfunctor_v =
        is_composed_arfunctor<Operand>::value;

    template <class Operator, class SafeOperators>
    struct must_be_parenthesized : std::false_type {};

    template <class Operator, class... SafeOperator>
    struct must_be_parenthesized<Operator, mpt::types<SafeOperator...>>
        : std::conditional_t<(IsBinaryOperator<Operator> &&
                              !mpt::has_type_v<Operator, SafeOperator...>),
                             std::true_type, std::false_type> {};

    template <class Operator, class... SafeOperator>
    static constexpr auto must_be_parenthesized_v =
        must_be_parenthesized<Operator, SafeOperator...>::value;

    /// Parse the strings of two operands
    template <class Operator, class LeftOperand, class RightOperand,
              class SafeLeftOperators, class SafeRightOperators>
    auto to_string_with_parentheses(LeftOperand const &lop,
                                    RightOperand const &rop, SafeLeftOperators,
                                    SafeRightOperators) {
      return concatenate_strings(to_string(lop), space, Operator::chars, space,
                                 to_string(rop));
    }

    /// Parse the strings of two operands in which that on the left is composed
    template <class Operator, class LO, class RightOperand, class... L,
              class SafeLeftOperators, class SafeRightOperators>
    auto to_string_with_parentheses(composed_arfunctor<LO, L...> const &lop,
                                    RightOperand const &rop, SafeLeftOperators,
                                    SafeRightOperators) {
      if constexpr (must_be_parenthesized_v<LO, SafeLeftOperators>)
        return concatenate_strings(lpar, to_string(lop), rpar, space,
                                   Operator::chars, space, to_string(rop));
      else
        return concatenate_strings(to_string(lop), space, Operator::chars,
                                   space, to_string(rop));
    }

    /// Parse the strings of two operands in which that on the right is composed
    template <class Operator, class RO, class LeftOperand, class... R,
              class SafeLeftOperators, class SafeRightOperators>
    auto to_string_with_parentheses(LeftOperand const &lop,
                                    composed_arfunctor<RO, R...> const &rop,
                                    SafeLeftOperators, SafeRightOperators) {

      if constexpr (must_be_parenthesized_v<RO, SafeRightOperators>)
        return concatenate_strings(to_string(lop), space, Operator::chars,
                                   space, lpar, to_string(rop), rpar);
      else
        return concatenate_strings(to_string(lop), space, Operator::chars,
                                   space, to_string(rop));
    }

    /// Parse the strings of two operands in which both are composed
    template <class Operator, class LO, class RO, class... L, class... R,
              class SafeLeftOperators, class SafeRightOperators>
    auto to_string_with_parentheses(composed_arfunctor<LO, L...> const &lop,
                                    composed_arfunctor<RO, R...> const &rop,
                                    SafeLeftOperators, SafeRightOperators) {
      // no wrap
      if constexpr (must_be_parenthesized_v<LO, SafeLeftOperators> &&
                    must_be_parenthesized_v<RO, SafeRightOperators>)
        return concatenate_strings(lpar, to_string(lop), rpar, space,
                                   Operator::chars, space, lpar, to_string(rop),
                                   rpar);
      // wrap only the right operation
      else if constexpr (!must_be_parenthesized_v<LO, SafeLeftOperators> &&
                         must_be_parenthesized_v<RO, SafeRightOperators>)
        return concatenate_strings(to_string(lop), space, Operator::chars,
                                   space, lpar, to_string(rop), rpar);
      // wrap only the left operation
      else if constexpr (must_be_parenthesized_v<LO, SafeLeftOperators> &&
                         !must_be_parenthesized_v<RO, SafeRightOperators>)
        return concatenate_strings(lpar, to_string(lop), rpar, space,
                                   Operator::chars, space, to_string(rop));
      // wrap both operations
      else
        return concatenate_strings(to_string(lop), space, Operator::chars,
                                   space, to_string(rop));
    }

    /*!\brief Helper class to determine when to wrap operands with parentheses

      Define operations that can be done without wrapping objects between
      parentheses. This must be done differently for operators on the
      left and right operands.
     */
    template <class Operator> struct binary_parentheses_handler;

    //
    // Arithmetic operators
    //

    template <> struct binary_parentheses_handler<add> {
      using safe_left_operators = mpt::types<add, sub, mul, div, modulo>;
      using safe_right_operators = safe_left_operators;
    };

    template <> struct binary_parentheses_handler<sub> {
      using safe_left_operators = mpt::types<add, sub, mul, div, modulo>;
      using safe_right_operators = mpt::types<mul, div, modulo>;
    };

    template <> struct binary_parentheses_handler<mul> {
      using safe_left_operators = mpt::types<mul, div, modulo>;
      using safe_right_operators = safe_left_operators;
    };

    template <> struct binary_parentheses_handler<div> {
      using safe_left_operators = mpt::types<mul, div, modulo>;
      using safe_right_operators = mpt::types<div>;
    };

    template <> struct binary_parentheses_handler<modulo> {
      using safe_left_operators = mpt::types<modulo>;
      using safe_right_operators = mpt::types<>;
    };

    //
    // Relational operators
    //
    using relational_safe_types =
        mpt::concatenate_types_t<binary_arithmetic_operators,
                                 binary_logical_operators>;

    template <> struct binary_parentheses_handler<lt> {
      using safe_left_operators = relational_safe_types;
      using safe_right_operators = relational_safe_types;
    };

    template <> struct binary_parentheses_handler<leq> {
      using safe_left_operators = relational_safe_types;
      using safe_right_operators = relational_safe_types;
    };

    template <> struct binary_parentheses_handler<gt> {
      using safe_left_operators = relational_safe_types;
      using safe_right_operators = relational_safe_types;
    };

    template <> struct binary_parentheses_handler<geq> {
      using safe_left_operators = relational_safe_types;
      using safe_right_operators = relational_safe_types;
    };

    template <> struct binary_parentheses_handler<eq> {
      using safe_left_operators = relational_safe_types;
      using safe_right_operators = relational_safe_types;
    };

    template <> struct binary_parentheses_handler<neq> {
      using safe_left_operators = relational_safe_types;
      using safe_right_operators = relational_safe_types;
    };

    using logical_safe_types =
        mpt::concatenate_types_t<binary_arithmetic_operators,
                                 binary_relational_operators>;

    template <> struct binary_parentheses_handler<logical_and> {
      using safe_left_operators =
          mpt::extend_types_t<logical_safe_types, logical_and>;
      using safe_right_operators = safe_left_operators;
    };

    template <> struct binary_parentheses_handler<logical_or> {
      using safe_left_operators =
          mpt::extend_types_t<logical_safe_types, logical_or>;
      using safe_right_operators = safe_left_operators;
    };

    //
    // Bitwise operators
    //
    using bitwise_shift_operators =
        mpt::types<bitwise_shift_left, bitwise_shift_right>;

    template <> struct binary_parentheses_handler<bitwise_and> {
      using safe_left_operators =
          mpt::extend_types_t<bitwise_shift_operators, bitwise_and>;
      using safe_right_operators = safe_left_operators;
    };

    template <> struct binary_parentheses_handler<bitwise_or> {
      using safe_left_operators =
          mpt::extend_types_t<bitwise_shift_operators, bitwise_or>;
      using safe_right_operators = safe_left_operators;
    };

    template <> struct binary_parentheses_handler<bitwise_xor> {
      using safe_left_operators =
          mpt::extend_types_t<bitwise_shift_operators, bitwise_xor>;
      using safe_right_operators = safe_left_operators;
    };

    using bitwise_binary_operators =
        mpt::types<bitwise_and, bitwise_or, bitwise_xor, bitwise_shift_left,
                   bitwise_shift_right>;

    template <> struct binary_parentheses_handler<bitwise_shift_left> {
      using safe_left_operators = bitwise_binary_operators;
      using safe_right_operators = safe_left_operators;
    };

    template <> struct binary_parentheses_handler<bitwise_shift_right> {
      using safe_left_operators = bitwise_binary_operators;
      using safe_right_operators = safe_left_operators;
    };
  } // namespace

  /// Conversion of user-defined operators to strings, as function calls
  template <class Operator, class... Operand>
  std::string to_string(composed_arfunctor<Operator, Operand...> const &f) {
    return to_string_impl(f, std::make_index_sequence<sizeof...(Operand)>());
  }

  /// Conversion of binary operators to strings
  template <class Operator, class LeftOperand, class RightOperand>
  requires IsBinaryOperator<Operator> std::string
  to_string(composed_arfunctor<Operator, LeftOperand, RightOperand> const &f) {
    using safe_lop =
        typename binary_parentheses_handler<Operator>::safe_left_operators;
    using safe_rop =
        typename binary_parentheses_handler<Operator>::safe_right_operators;
    return to_string_with_parentheses<Operator>(std::get<0>(f.operands()),
                                                std::get<1>(f.operands()),
                                                safe_lop{}, safe_rop{});
  }

  /// Conversion of unary operators to strings
  template <class Operator, class Operand>
  requires IsUnaryOperator<Operator> std::string
  to_string(composed_arfunctor<Operator, Operand> const &f) {
    if constexpr (is_composed_arfunctor_v<Operand>)
      return concatenate_strings(Operator::chars, lpar,
                                 to_string(std::get<0>(f.operands())), rpar);
    else
      return concatenate_strings(Operator::chars,
                                 to_string(std::get<0>(f.operands())));
  }
} // namespace mpt
