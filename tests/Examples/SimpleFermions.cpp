#include "Expression.hpp"
#include "Substitutions.hpp"
#include "Ordering.hpp"
#include "Utils.hpp"
#include "Operator.hpp"
#include "Comutators.hpp"
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
  return round_to_3_dp(exp.expression[0][0].value().real_part) == round_to_3_dp(ans.real_part) and
         round_to_3_dp(exp.expression[0][0].value().imaginary_part) == round_to_3_dp(ans.imaginary_part);
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
  exp1 = exp1.evaluate(boson_commutator<Fock1DInfo>, fermion_dual_occupation<Fock1DInfo>, SortUsing::ANTICOMMUTATORS);
  exp2 = exp2.evaluate(boson_commutator<Fock1DInfo>, fermion_dual_occupation<Fock1DInfo>, SortUsing::ANTICOMMUTATORS);
  exp3 = exp3.evaluate(boson_commutator<Fock1DInfo>, fermion_dual_occupation<Fock1DInfo>, SortUsing::ANTICOMMUTATORS);
  exp4 = exp4.evaluate(boson_commutator<Fock1DInfo>, fermion_dual_occupation<Fock1DInfo>, SortUsing::ANTICOMMUTATORS);
  exp1.print();
  exp2.print();
  exp3.print();
  exp4.print();
}
