#include "mpt/keywords.hpp"
#include <iostream>

struct alpha : mpt::keyword_argument<float> { };
struct beta : mpt::keyword_argument<float> { };
struct delta : mpt::keyword_argument<float> { };

class algorithm : public mpt::keywords_parser<mpt::required_keyword_arguments<alpha>, beta, delta> {
  using base_class = mpt::keywords_parser<mpt::required_keyword_arguments<alpha>, beta, delta>;
  using base_class::base_class;
};

int main() {

  static constexpr auto defaults = std::make_tuple(delta{2.f}, beta{0.f});

  algorithm algo(defaults, alpha{1.f});

  alpha a{1.f};

  algorithm algo2(defaults, a, delta{0.5f});

  std::cout << algo.get<alpha>() << std::endl;

  return 0;
}
