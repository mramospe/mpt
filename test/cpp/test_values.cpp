#include "mpt/values.hpp"
#include "test_utils.hpp"

/// Test the "value_at" function
mpt::test::errors test_value_at() {

  mpt::test::errors errors;

  int const a = 0, b = 1;

  if (mpt::value_at<0>(0, 1, 2) != 0)
    errors.push_back("Wrong evaluation from constant expression");

  if (mpt::value_at<1>(a, b) != 1)
    errors.push_back("Wrong evaluation from reference");

  if (mpt::value_at<2>(a, b, 2) != 2)
    errors.push_back("Wrong evaluation from hybrid set of inputs");

  return errors;
}

int main() {

  mpt::test::collector value_at("value accessor");
  MPT_TEST_UTILS_ADD_TEST(value_at, test_value_at);

  return mpt::test::is_success(value_at.run());

  return 0;
}
