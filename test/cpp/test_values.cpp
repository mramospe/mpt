#include "mpt/values.hpp"
#include <iostream>

int main() {

  int const a = 0, b = 1;

  std::cout << mpt::value_at<0>(0, 1, 2) << std::endl;
  std::cout << mpt::value_at<1>(a, b) << std::endl;

  return 0;
}
