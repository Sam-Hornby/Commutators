#include "transforms/BogoliubovTransform.hpp"
#include "Utils.hpp"
#define BOOST_TEST_MODULE BogoliubovTests
#include <boost/test/included/unit_test.hpp>

using namespace operators;

BOOST_AUTO_TEST_CASE(Simple) {
  Expression<Fock1DInfo> secondTerm0 = creation_op(0) * creation_op(1);
  Expression<Fock1DInfo> secondTerm1 = creation_op(1) * anihilation_op(1);
  Expression<Fock1DInfo> otherTerm0 = number<Fock1DInfo>(10) + number<Fock1DInfo>(10);
  Expression<Fock1DInfo> otherTerm1 = {{{anihilation_op(1)}}};
  Expression<Fock1DInfo> otherTerm2 = creation_op(1) * anihilation_op(1) * anihilation_op(1);
  auto input = secondTerm0 + secondTerm1 + otherTerm0 + otherTerm1 + otherTerm2;
  auto ans = bogoliubov_transform(input);
}
