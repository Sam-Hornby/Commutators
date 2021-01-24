#include <Expression/Expression.hpp>
#include <Expression/Infos/Fock1D.hpp>
#include <Substitutions/Substitutions.hpp>
#include <Expression/Evaluate.hpp>
#include <Expression/Ordering.hpp>
#include <Utils/Utils.hpp>
#include <Expression/Operator.hpp>
#include <Comutators/BosonComutators.hpp>
#include <exception>
#include <cmath>
#define BOOST_TEST_MODULE SimpleFermions
#include <boost/test/included/unit_test.hpp>

using namespace operators;

static double round_to_3_dp(double x) {
  return round(x * 1000) / 1000;
}

template <class OperatorInfo>
static bool check_answer(const Expression<OperatorInfo> & exp, const ComplexNumber ans) {
  if (exp.expression.size() != 1) {
    return false;
  }
  if (exp.expression[0].size() != 1) {
    return false;
  }
  if (!exp.expression[0][0].is_number()) {
    return false;
  }
  return round_to_3_dp(exp.expression[0][0].value().value.real()) == round_to_3_dp(ans.value.real()) and
         round_to_3_dp(exp.expression[0][0].value().value.imag()) == round_to_3_dp(ans.value.imag());
}

BOOST_AUTO_TEST_CASE(simple) {
  Expression<Fock1DInfo> exp1 = creation_op(0) * anihilation_op(0) * creation_op(0);
  Expression<Fock1DInfo> exp2 = creation_op(0) * anihilation_op(0) * anihilation_op(0);
  Expression<Fock1DInfo> exp3 = creation_op(0) * creation_op(0) * anihilation_op(0);
  Expression<Fock1DInfo> exp4 = anihilation_op(0) * creation_op(0);
  exp1 = normal_order<Fock1DInfo>(exp1);
  exp2 = normal_order<Fock1DInfo>(exp2);
  exp3 = normal_order<Fock1DInfo>(exp3);
  exp4 = normal_order<Fock1DInfo>(exp4);
  exp1 = evaluate<Fock1DInfo>(exp1, boson_commutator<Fock1DInfo>, fermion_dual_occupation<Fock1DInfo>, SortUsing::ANTICOMMUTATORS);
  exp2 = evaluate<Fock1DInfo>(exp2, boson_commutator<Fock1DInfo>, fermion_dual_occupation<Fock1DInfo>, SortUsing::ANTICOMMUTATORS);
  exp3 = evaluate<Fock1DInfo>(exp3, boson_commutator<Fock1DInfo>, fermion_dual_occupation<Fock1DInfo>, SortUsing::ANTICOMMUTATORS);
  exp4 = evaluate<Fock1DInfo>(exp4, boson_commutator<Fock1DInfo>, fermion_dual_occupation<Fock1DInfo>, SortUsing::ANTICOMMUTATORS);
  exp1.print();
  exp2.print();
  exp3.print();
  exp4.print();
}
