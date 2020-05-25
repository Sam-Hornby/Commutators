#include <Expression/Expression.hpp>
#include <Expression/Infos/Fock1D.hpp>
#include <Expression/Simplify.hpp>
#include <Expression/Operator.hpp>
#include <Substitutions/Substitutions.hpp>
#include <Expression/Substitutions.hpp>
#include <Utils/Utils.hpp>
#include <sstream>
#include <Expression/Infos/GenericInfo.hpp>
#include <spdlog/spdlog.h>
#define BOOST_TEST_MODULE Substitutions
#include <boost/test/included/unit_test.hpp>

using namespace operators;

BOOST_AUTO_TEST_CASE(empty) {
  Expression<Fock1DInfo> exp;
  exp = perform_multiplication_substitutions<Fock1DInfo>(exp, all_states_orthognal<Fock1DInfo>);
  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "\n");

  exp.expression = vector_type<vector_type<Operator<Fock1DInfo>>>(3);
  exp = perform_multiplication_substitutions<Fock1DInfo>(exp, all_states_orthognal<Fock1DInfo>);
  std::stringstream ss1;
  exp.print(ss1);
  BOOST_CHECK_EQUAL(ss1.str(), "\n");
}

BOOST_AUTO_TEST_CASE(no_subsi) {
  Operator<GenericInfo> A(ordering_value(-1), GenericInfo("A"));
  Operator<GenericInfo> B(ordering_value(-1), GenericInfo("B"));
  Operator<GenericInfo> C(ordering_value(-1), GenericInfo("C"));
  Operator<GenericInfo> D(ordering_value(-2), GenericInfo("D"));

  Expression<GenericInfo> exp = A * B * C * D;

  exp = perform_multiplication_substitutions<GenericInfo>(exp, no_subs<GenericInfo>);

  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "(A * B * C * D)\n");
}

static bool test_sub(typename vector_type<Operator<GenericInfo>>::iterator start,
                     vector_type<Operator<GenericInfo>> & exp) {
  if (start + 1 == exp.end()) {
    return false;
  }
  if (start->name() == "A" and (start + 1)->name() == "B") {
     *start = Operator<GenericInfo>(ordering_value(-1), GenericInfo("C"));
     set_to_one<GenericInfo>(start + 1, 1);
     return true;
  }
  return false;
}

BOOST_AUTO_TEST_CASE(a_b_equals_c) {
  Operator<GenericInfo> A(ordering_value(-1), GenericInfo("A"));
  Operator<GenericInfo> B(ordering_value(-1), GenericInfo("B"));
  Operator<GenericInfo> C(ordering_value(-1), GenericInfo("C"));
  Operator<GenericInfo> D(ordering_value(-2), GenericInfo("D"));

  Expression<GenericInfo> exp = A * B * C * D;

  exp = perform_multiplication_substitutions<GenericInfo>(exp, test_sub);
  exp = simplify_numbers<GenericInfo>(exp);

  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "(C * C * D)\n");
}

BOOST_AUTO_TEST_CASE(equal_states) {
  auto exp = hermition_conjugate<Fock1DInfo>(vacuum_state<Fock1DInfo>()) * vacuum_state<Fock1DInfo>();
  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "(<0| * |0>)\n");

  exp = perform_multiplication_substitutions<Fock1DInfo>(exp, all_states_orthognal<Fock1DInfo>);

  std::stringstream ss1;
  exp.print(ss1);
  BOOST_CHECK_EQUAL(ss1.str(), "(1.000000 * 1.000000)\n");

  exp = simplify_numbers<Fock1DInfo>(exp);

  std::stringstream ss2;
  exp.print(ss2);
  BOOST_CHECK_EQUAL(ss2.str(), "(1.000000)\n");
}

BOOST_AUTO_TEST_CASE(unequal_states) {
  auto exp = Operator<Fock1DInfo>(ordering_value(0), Fock1DInfo(1, Type::HC_STATE_VECTOR))
             * vacuum_state<Fock1DInfo>();
  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "(<1,0| * |0>)\n");

  exp = perform_multiplication_substitutions<Fock1DInfo>(exp, all_states_orthognal<Fock1DInfo>);
  exp = simplify_numbers<Fock1DInfo>(exp);
  std::stringstream ss1;
  exp.print(ss1);
  BOOST_CHECK_EQUAL(ss1.str(), "\n");
}

BOOST_AUTO_TEST_CASE(op_and_state) {
  Operator<Fock1DInfo> A(ordering_value(-1), Fock1DInfo(0, Type::ANIHILATION_OPERATOR));

  auto exp1 = A * vacuum_state<Fock1DInfo>();
  exp1 = perform_multiplication_substitutions<Fock1DInfo>(exp1, all_states_orthognal<Fock1DInfo>);
  std::stringstream ss;
  exp1.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "(a_0 * |0>)\n");

  auto exp2 = hermition_conjugate(vacuum_state<Fock1DInfo>()) * A;
  exp2 = perform_multiplication_substitutions<Fock1DInfo>(exp2, all_states_orthognal<Fock1DInfo>);

  std::stringstream ss1;
  exp2.print(ss1);
  BOOST_CHECK_EQUAL(ss1.str(), "(<0| * a_0)\n");
}

BOOST_AUTO_TEST_CASE(destroy_vacuum) {
  auto exp = anihilation_op(0) * vacuum_state<Fock1DInfo>();
  exp = perform_multiplication_substitutions<Fock1DInfo>(exp, anihilate_vacuum<Fock1DInfo>);
  exp = simplify_numbers<Fock1DInfo>(exp);
  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "\n");

  exp = hermition_conjugate(vacuum_state<Fock1DInfo>()) * creation_op(0) * vacuum_state<Fock1DInfo>();
  exp = perform_multiplication_substitutions<Fock1DInfo>(exp, anihilate_vacuum<Fock1DInfo>);
  exp = simplify_numbers<Fock1DInfo>(exp);
  std::stringstream ss1;
  exp.print(ss1);
  BOOST_CHECK_EQUAL(ss1.str(), "\n");

  exp = creation_op(1) * vacuum_state<Fock1DInfo>();
  exp = perform_multiplication_substitutions<Fock1DInfo>(exp, anihilate_vacuum<Fock1DInfo>);
  exp = simplify_numbers<Fock1DInfo>(exp);
  std::stringstream ss2;
  exp.print(ss2);
  BOOST_CHECK_EQUAL(ss2.str(), "(a_1! * |0>)\n");

  exp = hermition_conjugate(vacuum_state<Fock1DInfo>()) * anihilation_op(1);
  exp = perform_multiplication_substitutions<Fock1DInfo>(exp, anihilate_vacuum<Fock1DInfo>);
  exp = simplify_numbers<Fock1DInfo>(exp);
  std::stringstream ss3;
  exp.print(ss3);
  BOOST_CHECK_EQUAL(ss3.str(), "(<0| * a_1)\n");
}

BOOST_AUTO_TEST_CASE(destroy_fermions) {
  auto exp = anihilation_op(1) * anihilation_op(1) * creation_op(1) * vacuum_state<Fock1DInfo>();
  exp = perform_multiplication_substitutions<Fock1DInfo>(exp, fermion_dual_occupation<Fock1DInfo>);
  exp = simplify_numbers<Fock1DInfo>(exp);
  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "\n");

  exp = anihilation_op(1) * anihilation_op(2) * creation_op(1) * vacuum_state<Fock1DInfo>();
  exp = perform_multiplication_substitutions<Fock1DInfo>(exp, fermion_dual_occupation<Fock1DInfo>);
  exp = simplify_numbers<Fock1DInfo>(exp);
  std::stringstream ss1;
  exp.print(ss1);
  BOOST_CHECK_EQUAL(ss1.str(), "(a_1 * a_2 * a_1! * |0>)\n");
}
