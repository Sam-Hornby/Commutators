#include "gtest/gtest.h"
#include "Expression.hpp"
#include "Operator.hpp"
#include "Substitutions.hpp"
#include "Utils.hpp"
#include <sstream>
#include <Comutators.hpp>

using namespace operators;

TEST(substitution_tests, empty) {
  Expression<Fock1DInfo> exp;
  exp = exp.performMultiplicationSubstitutions(all_states_orthognal<Fock1DInfo>);
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "\n");

  exp.expression = std::vector<std::vector<Operator<Fock1DInfo>>>(3);
  exp = exp.performMultiplicationSubstitutions(all_states_orthognal<Fock1DInfo>);
  std::stringstream ss1;
  exp.print(ss1);
  ASSERT_EQ(ss1.str(), "\n");
}

TEST(substitution_tests, no_subs) {
  Operator<GenericInfo> A(ordering_value(-1), GenericInfo("A"));
  Operator<GenericInfo> B(ordering_value(-1), GenericInfo("B"));
  Operator<GenericInfo> C(ordering_value(-1), GenericInfo("C"));
  Operator<GenericInfo> D(ordering_value(-2), GenericInfo("D"));

  Expression<GenericInfo> exp = A * B * C * D;

  exp = exp.performMultiplicationSubstitutions(no_subs<GenericInfo>);

  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(A * B * C * D)\n");
}

static bool test_sub(typename std::vector<Operator<GenericInfo>>::iterator start,
                     std::vector<Operator<GenericInfo>> & exp) {
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

TEST(substitution_tests, a_b_equals_c) {
  Operator<GenericInfo> A(ordering_value(-1), GenericInfo("A"));
  Operator<GenericInfo> B(ordering_value(-1), GenericInfo("B"));
  Operator<GenericInfo> C(ordering_value(-1), GenericInfo("C"));
  Operator<GenericInfo> D(ordering_value(-2), GenericInfo("D"));

  Expression<GenericInfo> exp = A * B * C * D;

  exp = exp.performMultiplicationSubstitutions(test_sub);
  exp = exp.simplify_numbers();

  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(C * C * D)\n");
}

TEST(substitution_tests, equal_states) {
  auto exp = hermition_conjugate<Fock1DInfo>(vacuum_state<Fock1DInfo>()) * vacuum_state<Fock1DInfo>();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(<0| * |0>)\n");

  exp = exp.performMultiplicationSubstitutions(all_states_orthognal<Fock1DInfo>);

  std::stringstream ss1;
  exp.print(ss1);
  ASSERT_EQ(ss1.str(), "(1.000000 * 1.000000)\n");

  exp = exp.simplify_numbers();

  std::stringstream ss2;
  exp.print(ss2);
  ASSERT_EQ(ss2.str(), "(1.000000)\n");
}

TEST(substitution_tests, unequal_states) {
  auto exp = Operator<Fock1DInfo>(ordering_value(0), Fock1DInfo(1, Type::HC_STATE_VECTOR))
             * vacuum_state<Fock1DInfo>();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(<1,0| * |0>)\n");

  exp = exp.performMultiplicationSubstitutions(all_states_orthognal<Fock1DInfo>);
  exp = exp.simplify_numbers();
  std::stringstream ss1;
  exp.print(ss1);
  ASSERT_EQ(ss1.str(), "\n");
}

TEST(substitution_tests, op_and_state) {
  Operator<Fock1DInfo> A(ordering_value(-1), Fock1DInfo(0, Type::ANIHILATION_OPERATOR));

  auto exp1 = A * vacuum_state<Fock1DInfo>();
  exp1 = exp1.performMultiplicationSubstitutions(all_states_orthognal<Fock1DInfo>);
  std::stringstream ss;
  exp1.print(ss);
  ASSERT_EQ(ss.str(), "(a_0 * |0>)\n");

  auto exp2 = hermition_conjugate(vacuum_state<Fock1DInfo>()) * A;
  exp2 = exp2.performMultiplicationSubstitutions(all_states_orthognal<Fock1DInfo>);

  std::stringstream ss1;
  exp2.print(ss1);
  ASSERT_EQ(ss1.str(), "(<0| * a_0)\n");
}

TEST(substitution_tests, destroy_vacuum) {
  auto exp = anihilation_op(0) * vacuum_state<Fock1DInfo>();
  exp = exp.performMultiplicationSubstitutions(anihilate_vacuum<Fock1DInfo>);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "\n");

  exp = hermition_conjugate(vacuum_state<Fock1DInfo>()) * creation_op(0) * vacuum_state<Fock1DInfo>();
  exp = exp.performMultiplicationSubstitutions(anihilate_vacuum<Fock1DInfo>);
  exp = exp.simplify_numbers();
  std::stringstream ss1;
  exp.print(ss1);
  ASSERT_EQ(ss1.str(), "\n");

  exp = creation_op(1) * vacuum_state<Fock1DInfo>();
  exp = exp.performMultiplicationSubstitutions(anihilate_vacuum<Fock1DInfo>);
  exp = exp.simplify_numbers();
  std::stringstream ss2;
  exp.print(ss2);
  ASSERT_EQ(ss2.str(), "(a_1! * |0>)\n");

  exp = hermition_conjugate(vacuum_state<Fock1DInfo>()) * anihilation_op(1);
  exp = exp.performMultiplicationSubstitutions(anihilate_vacuum<Fock1DInfo>);
  exp = exp.simplify_numbers();
  std::stringstream ss3;
  exp.print(ss3);
  ASSERT_EQ(ss3.str(), "(<0| * a_1)\n");
}

TEST(substitution_tests, destroy_fermions) {
  auto exp = anihilation_op(1) * anihilation_op(1) * creation_op(1) * vacuum_state<Fock1DInfo>();
  exp = exp.performMultiplicationSubstitutions(fermion_dual_occupation<Fock1DInfo>);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "\n");

  exp = anihilation_op(1) * anihilation_op(2) * creation_op(1) * vacuum_state<Fock1DInfo>();
  exp = exp.performMultiplicationSubstitutions(fermion_dual_occupation<Fock1DInfo>);
  exp = exp.simplify_numbers();
  std::stringstream ss1;
  exp.print(ss1);
  ASSERT_EQ(ss1.str(), "(a_1 * a_2 * a_1! * |0>)\n");
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
