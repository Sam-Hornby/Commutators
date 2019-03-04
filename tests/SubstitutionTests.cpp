#include "gtest/gtest.h"
#include "Expression.hpp"
#include "Operator.hpp"
#include "Substitutions.hpp"
#include "Utils.hpp"
#include <sstream>

using namespace operators;

TEST(substitution_tests, empty) {
  Expression exp;
  exp = exp.performMultiplicationSubstitutions(all_states_orthognal);
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "\n");
}

TEST(substitution_tests, no_subs) {
  Operator A("A", ordering_value(-1), operator_info(-1));
  Operator B("B", ordering_value(-1), operator_info(-1));
  Operator C("C", ordering_value(-1), operator_info(-1));
  Operator D("D", ordering_value(-2), operator_info(-2));

  Expression exp = A * B * C * D;

  exp = exp.performMultiplicationSubstitutions(no_subs);

  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(A * B * C * D)\n");
}

TEST(substitution_tests, equal_states) {
  auto exp = hermition_conjugate(vacuum_state()) * vacuum_state();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(|0>_dag * |0>)\n");

  exp = exp.performMultiplicationSubstitutions(all_states_orthognal);

  std::stringstream ss1;
  exp.print(ss1);
  ASSERT_EQ(ss1.str(), "(1 * 1)\n");

  exp = exp.simplify_numbers();

  std::stringstream ss2;
  exp.print(ss2);
  ASSERT_EQ(ss2.str(), "(1)\n");
}

TEST(substitution_tests, unequal_states) {
  auto exp = Operator("<1|", ordering_value(0), operator_info(1, Type::HC_STATE_VECTOR)) * vacuum_state();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(<1| * |0>)\n");

  exp = exp.performMultiplicationSubstitutions(all_states_orthognal);
  exp = exp.simplify_numbers();
  std::stringstream ss1;
  exp.print(ss1);
  ASSERT_EQ(ss1.str(), "\n");
}

TEST(substitution_tests, op_and_state) {
  Operator A("A", ordering_value(-1), operator_info(-1));

  auto exp1 = A * vacuum_state();
  exp1 = exp1.performMultiplicationSubstitutions(all_states_orthognal);
  std::stringstream ss;
  exp1.print(ss);
  ASSERT_EQ(ss.str(), "(A * |0>)\n");

  auto exp2 = hermition_conjugate(vacuum_state()) * A;
  exp2 = exp2.performMultiplicationSubstitutions(all_states_orthognal);

  std::stringstream ss1;
  exp2.print(ss1);
  ASSERT_EQ(ss1.str(), "(|0>_dag * A)\n");
}

TEST(substitution_tests, destroy_vacuum) {
  auto exp = anihilation_op(0) * vacuum_state();
  exp = exp.performMultiplicationSubstitutions(anihilate_vacuum);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "\n");

  exp = hermition_conjugate(vacuum_state()) * creation_op(0);
  exp = exp.performMultiplicationSubstitutions(anihilate_vacuum);
  exp = exp.simplify_numbers();
  std::stringstream ss1;
  exp.print(ss1);
  ASSERT_EQ(ss1.str(), "\n");
}
