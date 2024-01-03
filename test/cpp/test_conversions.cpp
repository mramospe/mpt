#include "mpt/numstr.hpp"
#include "test_utils.hpp"

mpt::test::errors test_integral() {

    mpt::test::errors errors;



    return errors;
}

mpt::test::errors test_floating_point() {

    mpt::test::errors errors;

    return errors;
}

template<class Reference>
auto parsed_number_is_not_of_type(std::string_view str) {
    return std::visit([](auto const& e) -> bool {
        return !std::is_same_v<std::decay_t<decltype(e)>, Reference>;
    }, mpt::arithmetic_value_from_str(str));
}

mpt::test::errors test_general_infering_type() {

    mpt::test::errors errors;

    if ( parsed_number_is_not_of_type<int>("13") )
        errors.push_back("Wrong integral type; expected \"int\"");

    if ( parsed_number_is_not_of_type<long int>("13l") )
        errors.push_back("Wrong integral type; expected \"long int\"");

    if ( parsed_number_is_not_of_type<long long int>("13ll") )
        errors.push_back("Wrong integral type; expected \"long long int\"");

    if ( parsed_number_is_not_of_type<unsigned int>("13u") )
        errors.push_back("Wrong integral type; expected \"unsigned int\"");

    if ( parsed_number_is_not_of_type<unsigned long int>("13ul") )
        errors.push_back("Wrong integral type; expected \"unsigned long int\"");

    if ( parsed_number_is_not_of_type<unsigned long long int>("13ull") )
        errors.push_back("Wrong integral type; expected \"unsigned long long int\"");

    return errors;
}

int main() {

  mpt::test::collector basic("basic");
  MPT_TEST_UTILS_ADD_TEST(basic, test_integral);
  MPT_TEST_UTILS_ADD_TEST(basic, test_floating_point);

  mpt::test::collector general("general");
  MPT_TEST_UTILS_ADD_TEST(general, test_general_infering_type);

  return mpt::test::to_return_code(basic.run(), general.run());
}