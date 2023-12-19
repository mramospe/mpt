#pragma once
#include <memory>
#include <type_traits>
#include <utility>

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
        specialized_runtime_arfunctor_wrapper() = delete;
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
      runtime_arfunctor(Functor &&functor)
          : m_ptr{std::make_shared<specialized_runtime_arfunctor_wrapper<
                std::remove_cvref_t<Functor>, Output, Input...>>(
                std::forward<Functor>(functor))} {}

      runtime_arfunctor(runtime_arfunctor const &other) = default;
      runtime_arfunctor(runtime_arfunctor &&other) = default;
      runtime_arfunctor &operator=(runtime_arfunctor const &other) = default;
      runtime_arfunctor &operator=(runtime_arfunctor &&other) = default;

      /// Call the internal functor
      Output operator()(Input const &...args) const {
        return m_ptr->operator()(args...);
      }

    private:
      /// Pointer to the internal functor wrapper
      std::shared_ptr<runtime_arfunctor_wrapper<Output(Input...)>> m_ptr =
          nullptr;
    };
  } // namespace core

  template <class Signature>
  using runtime_arfunctor = core::runtime_arfunctor<Signature>;

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
    return core::runtime_arfunctor<Signature>(std::forward<Functor>(functor));
  }
} // namespace mpt::arfunctors
