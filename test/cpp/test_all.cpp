#include "mpt/all.hpp"
#include "test_utils.hpp"

using namespace mpt;

// Test that we can include all the headers without problems
int main() {
  mpt::test::collector all("all");
  return mpt::test::to_return_code(all.run());
}
