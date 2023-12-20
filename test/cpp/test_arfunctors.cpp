#include "mpt/arfunctors/all.hpp"
#include "test_utils.hpp"
#include <cmath>
#include <sstream>
#include <string>

struct position {
  float x, y, z;
};

struct functor_x_t : public mpt::arfunctors::arfunctor {
  template <class Operand> auto operator()(Operand &&op) const { return op.x; }
} constexpr functor_x;

struct functor_y_t : public mpt::arfunctors::arfunctor {
  template <class Operand> auto operator()(Operand &&op) const { return op.y; }
} constexpr functor_y;

struct functor_z_t : public mpt::arfunctors::arfunctor {
  template <class Operand> auto operator()(Operand &&op) const { return op.z; }
} constexpr functor_z;

struct abs_operator {
  template <class Operand> constexpr auto operator()(Operand &&op) const {
    return op > 0 ? op : -op;
  }
};

struct sqrt_operator {
  template <class Operand> constexpr auto operator()(Operand &&op) const {
    return std::sqrt(op);
  }
};

struct in_range_operator {
  template <class LeftBound, class Operand, class RightBound>
  constexpr auto operator()(LeftBound&& lb, Operand &&op, RightBound&& rb) const {
    return op > lb && op < rb;
  }
};

template <class Operand> constexpr auto abs(Operand &&op) {
  return mpt::arfunctors::make_composed_arfunctor<abs_operator>(
      std::forward<Operand>(op));
}

template <class Operand> constexpr auto sqrt(Operand &&op) {
  return mpt::arfunctors::make_composed_arfunctor<sqrt_operator>(
      std::forward<Operand>(op));
}

template <class LeftBound, class Operand, class RightBound>
constexpr auto in_range(LeftBound&& lb, Operand &&op, RightBound&& rb) {
  return mpt::arfunctors::make_composed_arfunctor<in_range_operator>(
      std::forward<LeftBound>(lb), std::forward<Operand>(op), std::forward<RightBound>(rb));
}

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
                      (shift_x < 900.f || !(shift_x < 1.f));

  if (!requirements(pos))
    errors.push_back("Relational operators failed");

  return errors;
}

class configurable_functor : public mpt::arfunctors::arfunctor {

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

mpt::test::errors test_math() {

  mpt::test::errors errors;

  position pos = {1.f, 2.f, 3.f};

  if (!mpt::test::is_close(sqrt(functor_x + functor_z)(pos), 2.f))
    errors.push_back("Unable to calculate the square root");

  if (!in_range(-2.f, functor_x, +2.f)(pos))
    errors.push_back("Unable to determine if a quantity is in the given range");

  if (!in_range(functor_x, functor_y, functor_z)(pos))
    errors.push_back("Unable to determine if a quantity is in a range defined by functors");

  return errors;
}

mpt::test::errors test_runtime() {

  mpt::test::errors errors;

  auto fx = mpt::arfunctors::make_runtime_arfunctor<float(position const &)>(
      functor_x);
  auto fy = mpt::arfunctors::make_runtime_arfunctor<float(position const &)>(
      functor_y);
  auto fz = functor_z;

  position pos = {1.f, 2.f, 3.f};

  if (!mpt::test::is_close((fx + fy + fz + 4.f)(pos), 10.f))
    errors.push_back("Unable to calculate the sum of positions at runtime");

  return errors;
}

mpt::test::errors test_runtime_math() {

  mpt::test::errors errors;

  auto fx = mpt::arfunctors::make_runtime_arfunctor<float(position const &)>(
      functor_x);
  auto fy = functor_y;
  auto fz = functor_z;

  position pos = {1.f, 2.f, 3.f};

  if (!mpt::test::is_close(sqrt(fx + fz)(pos), 2.f))
    errors.push_back("Unable to calculate the square root from a compile-time "
                     "and a run-time functor");

  if (!mpt::test::is_close(sqrt(fx * fx)(pos), 1.f))
    errors.push_back(
        "Unable to calculate the square root from two run-time functors");

  if (!in_range(-2.f, fx, +2.f)(pos))
    errors.push_back("Unable to determine if a quantity is in the given range");

  if (!in_range(fx, fy, fz)(pos))
    errors.push_back("Unable to determine if a quantity is in a range defined by functors");

  return errors;
}

template <class Functor> std::string to_string(Functor const &functor) {
  std::stringstream ss;
  ss << functor;
  return ss.str();
}

mpt::test::errors test_parser() {

  mpt::test::errors errors;

  auto fx = mpt::arfunctors::make_runtime_arfunctor<float(position const &)>(
      functor_x);
  auto fy = mpt::arfunctors::make_runtime_arfunctor<float(position const &)>(
      functor_y);
  auto fz = mpt::arfunctors::make_runtime_arfunctor<float(position const &)>(functor_z);

  auto magt = mpt::arfunctors::make_runtime_arfunctor<float(position const &)>(sqrt(fx * fx + fy * fy));

  auto mag = mpt::arfunctors::make_runtime_arfunctor<float(position const &)>(sqrt(fx * fx + fy * fy + fz * fz));

  using signatures = mpt::arfunctors::signatures<float(position const&), int(position const&)>;

  mpt::arfunctors::functor_map<signatures> functor_map = {{"x", fx}, {"y", fy}, {"z", fz}, {"magt", magt}, {"mag", mag}};
  mpt::arfunctors::function_map<signatures> function_map;

  auto parser = mpt::arfunctors::make_parser(functor_map, function_map);

  return errors;
}

/*
mpt::test::errors test_string() {

  mpt::test::errors errors;

  auto compare = [&errors](auto &&functor, std::string reference) {
    auto ct_result = to_string(functor);
    if (ct_result != reference)
      errors.push_back("Wrong string obtained (compile-time): \"" + ct_result +
                       "\" (reference: " + reference + "\")");

    auto rt_result = to_string(
        mpt::make_runtime_arfunctor<float(position const &)>(functor));
    if (rt_result != reference)
      errors.push_back("Wrong string obtained (run-time): \"" + rt_result +
                       "\" (reference: " + reference + "\")");
  };

  auto rho = sqrt(functor_x * functor_x + functor_y * functor_y);
  compare(rho, "sqrt(x * x + y * y)");

  auto logical =
      rho < 2 && abs(functor_z) - 1 > 5 && !(functor_x < 5 || functor_y < 5);
  compare(logical,
          "sqrt(x * x + y * y) < 2 && abs(z) - 1 > 5 && !(x < 5 || y < 5)");

  auto bitwise = (rho < 2) & (abs(functor_z) - 1 > 5) &
                 ~((functor_x < 5) | (functor_y < 5));
  compare(
      bitwise,
      "(sqrt(x * x + y * y) < 2) & (abs(z) - 1 > 5) & ~((x < 5) | (y < 5))");

  return errors;
}
*/
int main() {

  mpt::test::collector arfunctors("arfunctors");
  MPT_TEST_UTILS_ADD_TEST(arfunctors, test_simple);
  MPT_TEST_UTILS_ADD_TEST(arfunctors, test_configurable);
  MPT_TEST_UTILS_ADD_TEST(arfunctors, test_math);

  mpt::test::collector runtime_arfunctors("runtime arfunctors");
  MPT_TEST_UTILS_ADD_TEST(runtime_arfunctors, test_runtime);
  MPT_TEST_UTILS_ADD_TEST(runtime_arfunctors, test_runtime_math);
  MPT_TEST_UTILS_ADD_TEST(runtime_arfunctors, test_parser);
  //MPT_TEST_UTILS_ADD_TEST(runtime_arfunctors, test_string);

  return mpt::test::to_return_code(arfunctors.run(), runtime_arfunctors.run());
}
