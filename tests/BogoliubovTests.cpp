#include "transforms/BogoliubovTransform.hpp"
#include "transforms/BogoliubovTransformFunctions.hpp"
#include <Utils/Utils.hpp>
#define BOOST_TEST_MODULE BogoliubovTests
#include <boost/test/included/unit_test.hpp>

using namespace operators;
const auto trans = BosonDefaultTransforms<Fock1DInfo>();

BOOST_AUTO_TEST_CASE(Simple) {
  Expression<Fock1DInfo> secondTerm0 = creation_op(0) * creation_op(1);
  Expression<Fock1DInfo> secondTerm1 = creation_op(1) * anihilation_op(1);
  Expression<Fock1DInfo> otherTerm0 = number<Fock1DInfo>(10) + number<Fock1DInfo>(10);
  Expression<Fock1DInfo> otherTerm1 = {{{anihilation_op(1)}}};
  Expression<Fock1DInfo> otherTerm2 = creation_op(1) * anihilation_op(1) * anihilation_op(1);
  auto input = secondTerm0 + secondTerm1 + otherTerm0 + otherTerm1 + otherTerm2;
  auto ans = bogoliubov_transform<Fock1DInfo>(input, trans);
}


const Expression<Fock1DInfo> valid_diagonol_terms = (anihilation_op(0) * creation_op(0)) +
                                              (anihilation_op(1) * creation_op(1));
const Expression<Fock1DInfo> valid_cross_terms = (anihilation_op(0) * anihilation_op(1)) +
                                           (creation_op(1) * creation_op(0));

const Expression<Fock1DInfo> constant1 = number<Fock1DInfo>(5.0) * number<Fock1DInfo>(1.0);
const Expression<Fock1DInfo> constant2 =
          Operator<Fock1DInfo>(ComplexNumber(2.0, 3.0)) * number<Fock1DInfo>(1.0);
const Expression<Fock1DInfo> constant3 =
          Operator<Fock1DInfo>(NamedNumber('k')) * Operator<Fock1DInfo>(ComplexNumber(2.0, 3.0));


template <class T>
static unsigned get_total(const vector_type<Operator<T>> &A) {
  unsigned total = 0;
  for (const auto &op : A) {
    if (op.is_number()) {continue;}
    auto &info = op.info();
    total += ((static_cast<unsigned>(info.type) * 100) + info.x_coordinate);
  }
  return total;
}

template <class T>
static void makeup_fixed_order(Expression<T> &A) {
  std::sort(A.expression.begin(), A.expression.end(),
            [] (const vector_type<Operator<T>> &A, const vector_type<Operator<T>> &B) {
    return get_total<T>(A) < get_total<T>(B);
  });
}


BOOST_AUTO_TEST_CASE(NoTransform1) {
  // Term of tpye E(C0!C0 + C1!C1) + Y(C0!C0! + C1C1)
  const auto invalid_cross_terms =
        (anihilation_op(0) * anihilation_op(0)) + (creation_op(1) * creation_op(1));
  auto expr = (constant1 * valid_diagonol_terms) + (constant1 * invalid_cross_terms);
  expr = simplify_numbers(expr);
  auto ans = bogoliubov_transform<Fock1DInfo>(expr, trans);
  makeup_fixed_order(expr);
  makeup_fixed_order(ans);
  BOOST_CHECK_EQUAL(expr, ans);
}

BOOST_AUTO_TEST_CASE(NoTransform2) {
  const auto invalid_diagonol_terms =
        (anihilation_op(0) * creation_op(1)) + (anihilation_op(1) * creation_op(0));
  auto expr = (constant1 * invalid_diagonol_terms) + (constant1 * valid_cross_terms);
  expr = simplify_numbers(expr);
  auto ans = bogoliubov_transform<Fock1DInfo>(expr, trans);
  makeup_fixed_order(expr);
  makeup_fixed_order(ans);
  BOOST_CHECK_EQUAL(expr, ans);
}
