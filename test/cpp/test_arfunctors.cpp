#include "mpt/arfunctors.hpp"
#include "test_utils.hpp"

struct position {
  float x, y, z;
};

struct functor_x_t : public mpt::arfunctor<functor_x_t> {
  template <class Operand> auto operator()(Operand &&op) const { return op.x; }
} constexpr functor_x;

struct functor_y_t : public mpt::arfunctor<functor_y_t> {
  template <class Operand> auto operator()(Operand &&op) const { return op.y; }
} constexpr functor_y;

struct functor_z_t : public mpt::arfunctor<functor_z_t> {
  template <class Operand> auto operator()(Operand &&op) const { return op.z; }
} constexpr functor_z;

mpt::test::errors test_simple() {

  mpt::test::errors errors;

  auto mod2 =
      functor_x * functor_x + functor_y * functor_y + functor_z * functor_z;

  position pos = {1.f, 2.f, 3.f};

  if (!mpt::test::is_close(mod2(pos), 14.f))
    errors.push_back("Invalid value of the squared module");

  auto shift_x = functor_x + 10.f;

  if (!mpt::test::is_close(shift_x(pos), 11.f))
    errors.push_back("Invalid addition by a constant");

  auto requirements = 10 * shift_x < 1000.f && mod2 > 10.f &&
                      !(shift_x < 900.f || shift_x < 1.f);

  if (!requirements(pos))
    errors.push_back("Relational operators failed");

  return errors;
}

class configurable_functor : public mpt::arfunctor<configurable_functor> {

public:
  configurable_functor(float param_x, float param_y)
      : m_param_x{param_x}, m_param_y{param_y} {}

  template <class Object> auto operator()(Object &&obj) const {
    return m_param_y * obj.y + m_param_x * obj.x + obj.z;
  }

private:
  float m_param_x = 0.f;
  float m_param_y = 0.f;
};

mpt::test::errors test_configurable() {

  mpt::test::errors errors;

  position pos = {1.f, 2.f, 3.f};

  configurable_functor conf_1{2.f, 4.f};

  if (!mpt::test::is_close(conf_1(pos), 13.f))
    errors.push_back("Problems executing a configurable functor");

  configurable_functor conf_2{3.f, 5.f};

  if (!mpt::test::is_close((conf_1 + conf_2)(pos), 29.f))
    errors.push_back("Problems executing a composed configurable functor");

  return errors;
}

int main() {

  mpt::test::collector arfunctors("arfunctors");
  MPT_TEST_UTILS_ADD_TEST(arfunctors, test_simple);
  MPT_TEST_UTILS_ADD_TEST(arfunctors, test_configurable);

  return mpt::test::to_return_code(arfunctors.run());
}
