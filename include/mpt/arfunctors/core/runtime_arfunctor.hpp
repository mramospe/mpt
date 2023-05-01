#pragma once

namespace mpt::arfunctors {

  namespace core {

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

      /*!\brief Represent the functor as a string

      This is used for parsing functors from strings.
      A runtime instance whose underlying functor does
      not have a corresponding conversion function will
      lead to a runtime error.
     */
      virtual std::ostream &to_ostream(std::ostream &) const = 0;

      /// Provide a clone of the wrapper
      virtual runtime_arfunctor_wrapper *clone() const = 0;
    };

    namespace {

      /*!\brief Runtime arithmetic and relational functor wrapper

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
        Output operator()(Input const &...args) const override {
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
      Output operator()(Input const &...args) const {
        return m_ptr->operator()(args...);
      }

      /// Conversion to \ref std::string
      friend std::ostream &
      operator<<(std::ostream &os,
                 runtime_arfunctor<Output(Input...)> const &f) {
        return f.m_ptr->to_ostream(os);
      }

    private:
      /// Pointer to the internal functor wrapper
      runtime_arfunctor_wrapper<Output(Input...)> *m_ptr = nullptr;
    };

    namespace {
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
    auto make_runtime_arfunctor(Functor &&functor) {
      return make_runtime_arfunctor_t<Signature,
                                      std::remove_cvref_t<Functor>>{}(
          std::forward<Functor>(functor));
    }

    /// Build a composed functor from the operator and the operand types
    template <class Operator, class FunctorType, class... Operand>
    FunctorType make_runtime_composed_arfunctor(Operand &&...op) {
      return FunctorType{
          composed_arfunctor<Operator, std::remove_cvref_t<Operand>...>{
              std::forward<Operand>(op)...}};
    }
  } // namespace core

  template <class Signature>
  using runtime_arfunctor = runtime_arfunctor<Signature>;
} // namespace mpt::arfunctors