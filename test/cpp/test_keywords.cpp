#include "mpt/keywords.hpp"
#include "test_utils.hpp"

struct alpha : mpt::keywords::argument<float> {};
struct beta : mpt::keywords::argument<int> {};
struct delta : mpt::keywords::argument<float> {};

/// Algorithm that tests keyword parsing with no required arguments
class algorithm
    : public mpt::keywords::parser<mpt::keywords::required<>,
                                   mpt::keywords::defaulted<alpha, beta>> {
  using base_class =
      mpt::keywords::parser<mpt::keywords::required<>,
                            mpt::keywords::defaulted<alpha, beta>>;
  using base_class::base_class;
};

/// Algorithm that tests keyword parsing with required arguments
class algorithm_with_required
    : public mpt::keywords::parser<mpt::keywords::required<delta>,
                                   mpt::keywords::defaulted<alpha, beta>> {
  using base_class =
      mpt::keywords::parser<mpt::keywords::required<delta>,
                            mpt::keywords::defaulted<alpha, beta>>;
  using base_class::base_class;
};

// Default values for the arguments
static constexpr auto alpha_default = 0.5f;
static constexpr auto beta_default = 1;
static constexpr auto defaults =
    std::make_tuple(alpha{alpha_default}, beta{beta_default});

/// Test a case with no required arguments
mpt::test::errors test_simple() {

  mpt::test::errors errors;

  algorithm algo_from_const(defaults);

  auto defaults_for_move = defaults;

  algorithm algo_from_move(std::move(defaults_for_move));

  if (!(mpt::test::is_close(algo_from_const.get<alpha>(), alpha_default) &&
        mpt::test::is_close(algo_from_move.get<alpha>(), alpha_default) &&
        mpt::test::is_close(algo_from_const.get<beta>(), beta_default) &&
        mpt::test::is_close(algo_from_move.get<beta>(), beta_default)))
    errors.push_back("Construction of keyword arguments failed");

  return errors;
}

/// Test a case with no required arguments
mpt::test::errors test_with_required() {

  mpt::test::errors errors;

  auto delta_default = 0.1f;

  algorithm_with_required algo_from_const(defaults, delta{delta_default});

  auto defaults_for_move = defaults;

  algorithm_with_required algo_from_move(std::move(defaults_for_move),
                                         delta{delta_default});

  if (!(mpt::test::is_close(algo_from_const.get<alpha>(), alpha_default) &&
        mpt::test::is_close(algo_from_move.get<alpha>(), alpha_default) &&
        mpt::test::is_close(algo_from_const.get<beta>(), beta_default) &&
        mpt::test::is_close(algo_from_move.get<beta>(), beta_default) &&
        mpt::test::is_close(algo_from_const.get<delta>(), delta_default) &&
        mpt::test::is_close(algo_from_move.get<delta>(), delta_default)))
    errors.push_back("Construction of keyword arguments failed");

  return errors;
}

/// Test a case where we override the default arguments
mpt::test::errors test_override() {

  mpt::test::errors errors;

  auto alpha_override = 0.1f;
  auto delta_default = 0.1f;

  algorithm_with_required algo(defaults, delta{delta_default},
                               alpha{alpha_override});

  if (!(mpt::test::is_close(algo.get<beta>(), beta_default) &&
        mpt::test::is_close(algo.get<delta>(), delta_default)))
    errors.push_back("Construction of keyword arguments failed");

  if (!mpt::test::is_close(algo.get<alpha>(), alpha_override))
    errors.push_back("Failed to override arguments");

  return errors;
}

int main() {

  mpt::test::collector keywords("keywords");
  MPT_TEST_UTILS_ADD_TEST(keywords, test_simple);
  MPT_TEST_UTILS_ADD_TEST(keywords, test_with_required);
  MPT_TEST_UTILS_ADD_TEST(keywords, test_override);

  return mpt::test::is_success(keywords.run());
}
