#include "mpt/soa.hpp"
#include "test_utils.hpp"

struct x : mpt::field<float> {};
struct y : mpt::field<float> {};
struct z : mpt::field<float> {};

struct position : mpt::composite_field<x, y, z> {};
struct direction : mpt::composite_field<x, y, z> {};
struct scale : mpt::field<int> {};

template <class VectorType> mpt::test::errors test_general() {

  mpt::test::errors errors;

  VectorType av;

  av.reserve(10);
  if (av.size() != 0)
    errors.push_back("Size modified after call to \"reserve\"");

  av.resize(10);
  if (av.size() != 10)
    errors.push_back("Wrong size after call to \"resize\"");

  VectorType bv(10);
  if (bv.size() != 10)
    errors.push_back("Wrong size on construction");

  using value_type = typename VectorType::value_type;

  // normal iteration
  for (auto it = av.begin(); it != av.end(); ++it) {
    *it = value_type{};
    [[maybe_unused]] value_type v = *it;
  }

  // normal const iteration
  for (auto it = av.cbegin(); it != av.cend(); ++it)
    [[maybe_unused]] value_type v = *it;

  // foreach iteration
  for ([[maybe_unused]] auto e : av)
    e = value_type{};

  // test the iterator properties
  auto it = av.begin();
  auto end = av.end();

  if (end - it != int(av.size()))
    errors.push_back("Subtraction of iterators has not been successful");

  it += 4;
  if (end - it != int(av.size() - 4))
    errors.push_back("Subtraction of iterators has not been successful");

  it -= 4;
  if (end - it != int(av.size()))
    errors.push_back("Subtraction of iterators has not been successful");

  return errors;
}

mpt::test::errors test_simple() { return test_general<mpt::soa_vector<x>>(); }

mpt::test::errors test_xyz() {
  return test_general<mpt::soa_vector<x, y, z>>();
}

mpt::test::errors test_position_and_direction() {
  return test_general<mpt::soa_vector<position, direction>>();
}

mpt::test::errors test_position_direction_and_scale() {

  using vector_type = mpt::soa_vector<position, direction, scale>;

  auto errors = test_general<vector_type>();

  // add some more tests for the accessors
  vector_type v(100);

  for (auto e : v) {
    e.get<position, x>() = 0.f;
    e.get<position, y>() = 0.f;
    e.get<position, z>() = 0.f;
    e.get<direction>() = mpt::make_soa_value<direction>(1.f, 1.f, 1.f);
    e.get<scale>() = 1;
  }

  for (auto e : v) {
    if (e.get<scale>() != 1) {
      errors.push_back("Unable to assign values correctly");
      break;
    }
  }

  return errors;
}

int main() {

  mpt::test::collector soa("soa");
  MPT_TEST_UTILS_ADD_TEST(soa, test_simple);
  MPT_TEST_UTILS_ADD_TEST(soa, test_xyz);
  MPT_TEST_UTILS_ADD_TEST(soa, test_position_and_direction);
  MPT_TEST_UTILS_ADD_TEST(soa, test_position_direction_and_scale);

  return mpt::test::to_return_code(soa.run());
}
