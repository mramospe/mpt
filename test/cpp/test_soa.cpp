#include "mpt/soa.hpp"
#include "test_utils.hpp"

struct x : mpt::field<float> {};
struct y : mpt::field<float> {};
struct z : mpt::field<float> {};

struct position : mpt::composite_field<x, y, z> {};
struct direction : mpt::composite_field<x, y, z> {};
struct scale : mpt::field<float> {};

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
  return test_general<mpt::soa_vector<position, direction, scale>>();
}

int main() {

  mpt::test::collector soa("soa");
  MPT_TEST_UTILS_ADD_TEST(soa, test_simple);
  MPT_TEST_UTILS_ADD_TEST(soa, test_xyz);
  MPT_TEST_UTILS_ADD_TEST(soa, test_position_and_direction);
  MPT_TEST_UTILS_ADD_TEST(soa, test_position_direction_and_scale);

  return mpt::test::to_return_code(soa.run());
}
