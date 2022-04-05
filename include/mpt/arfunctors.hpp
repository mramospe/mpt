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

#ifndef MPT_DOXYGEN_WARD
  template <class Signature> struct runtime_arfunctor_wrapper;
#endif

  /*!\brief Abstract runtime arithmetic and relation functor wrapper

    This is the base type for any run-time functor wrapper.
    It allows to call any functor using polymorphism.
   */
  template <class Output, class... Input>
  struct runtime_arfunctor_wrapper<Output(Input...)> {
  public:
    virtual ~runtime_arfunctor_wrapper() {}

    /// Force a signature to be used to call the functor
    virtual Output operator()(Input const &...) const = 0;

    /// Provide a clone of the wrapper
    virtual runtime_arfunctor_wrapper *clone() const = 0;
  };

  namespace {

    /*!\brief Runtime arithmetic and relation functor wrapper

    This object wraps any functor type without needing to inherit from
    any additional class (i.e. allows to work directly with
    \ref mpt::arfunctor objects).
   */
    template <class Functor, class Output, class... Input>
    class specialized_runtime_arfunctor_wrapper
        : public runtime_arfunctor_wrapper<Output(Input...)> {

    public:
      specialized_runtime_arfunctor_wrapper() = default;
      specialized_runtime_arfunctor_wrapper(Functor const &functor)
          : m_functor{functor} {}
      specialized_runtime_arfunctor_wrapper(Functor &&functor)
          : m_functor{std::move(functor)} {}
      specialized_runtime_arfunctor_wrapper(
          specialized_runtime_arfunctor_wrapper const &) = default;
      specialized_runtime_arfunctor_wrapper(
          specialized_runtime_arfunctor_wrapper &&) = default;
      specialized_runtime_arfunctor_wrapper &
      operator=(specialized_runtime_arfunctor_wrapper const &) = default;
      specialized_runtime_arfunctor_wrapper &
      operator=(specialized_runtime_arfunctor_wrapper &&) = default;

      /// Call the wrapped functor
      Output operator()(Input const &... args) const override {
        return m_functor(args...);
      }

      /// Return a clone of this object
      runtime_arfunctor_wrapper<Output(Input...)> *clone() const override {
        return new specialized_runtime_arfunctor_wrapper{*this};
      }

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
        : m_ptr{new specialized_runtime_arfunctor_wrapper<
              std::remove_cvref_t<Functor>, Output, Input...>{
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

  private:
    /// Pointer to the internal functor wrapper
    runtime_arfunctor_wrapper<Output(Input...)> *m_ptr = nullptr;
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
} // namespace mpt
