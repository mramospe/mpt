#include "mpt/all.hpp"

int main() {
  static_assert(mpt::has_type_v<float, float>);
  return 0;
}
