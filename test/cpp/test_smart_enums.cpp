#include "mpt/smart_enum.hpp"
#include <iostream>

MPT_SMART_ENUM(simple, simple_properties, int, simple_unknown, first, second);

int main() {

  std::cout << mpt::smart_enum::properties_t<simple>::size << std::endl;
  std::cout << mpt::smart_enum::properties_t<simple>::names[0] << std::endl;
  std::cout << mpt::smart_enum::properties_t<simple>::names[1] << std::endl;

  return 0;
}
