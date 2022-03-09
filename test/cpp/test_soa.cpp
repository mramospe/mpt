#include "mpt/soa.hpp"
#include "test_utils.hpp"

struct x : mpt::field<float> {};
struct y : mpt::field<float> {};
struct z : mpt::field<float> {};

struct position : mpt::composite_field<x, y, z> {};
struct direction : mpt::composite_field<x, y, z> {};
struct scale : mpt::field<int> {};

static_assert(mpt::is_basic_field_v<x>, "Basic field is not detected as such");
static_assert(!mpt::is_basic_field_v<position>,
              "Composite field is detected as a basic field");
static_assert(!mpt::is_composite_field_v<x>,
              "Basic field is detected as a composite field");
static_assert(mpt::is_composite_field_v<position>,
              "Composite field is not detected as such");

template <class Container> mpt::test::errors test_iterations(Container &av) {

  mpt::test::errors errors;

  using value_type = typename Container::value_type;

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

  auto iteration_errors = test_iterations(av);

  errors.reserve(errors.size() + iteration_errors.size());

  std::copy(iteration_errors.cbegin(), iteration_errors.cend(),
            std::back_insert_iterator(errors));

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
    e.get<position>().get<x>() = 0.f;
    e.get<position>().get<y>() = 0.f;
    e.get<position>().get<z>() = 0.f;
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

mpt::test::errors test_zip() {

  mpt::soa_vector<position, scale> ps(100);
  mpt::soa_vector<direction> d(100);

  auto zip = mpt::make_soa_zip(ps, d);

  auto errors = test_iterations(zip);

  return errors;
}

int main() {

  mpt::test::collector soa_vector("soa_vector");
  MPT_TEST_UTILS_ADD_TEST(soa_vector, test_simple);
  MPT_TEST_UTILS_ADD_TEST(soa_vector, test_xyz);
  MPT_TEST_UTILS_ADD_TEST(soa_vector, test_position_and_direction);
  MPT_TEST_UTILS_ADD_TEST(soa_vector, test_position_direction_and_scale);

  mpt::test::collector soa_zip("soa_zip");
  MPT_TEST_UTILS_ADD_TEST(soa_zip, test_zip);

  return mpt::test::to_return_code(soa_vector.run()) ||
         mpt::test::to_return_code(soa_zip.run());
}
