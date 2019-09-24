#include "gtest/gtest.h"
#include "Expression.hpp"
#include "Operator.hpp"
#include "Comutators.hpp"
#include <sstream>

using namespace operators;


TEST(number_tests, empty) {
  Expression<Fock1DInfo> exp;
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "\n");
}

TEST(number_tests, single) {
  Expression<Fock1DInfo> exp;
  exp = exp + Operator<Fock1DInfo>(5);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(5.000000)\n");
}

TEST(number_tests, two_numbers) {
  Expression<Fock1DInfo> exp = Operator<Fock1DInfo>(5) * Operator<Fock1DInfo>(7);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(35.000000)\n");
}


TEST(number_tests, operators_and_numbers) {
  Expression<GenericInfo> exp = Operator<GenericInfo>(ordering_value(0), GenericInfo("A")) * Operator<GenericInfo>(4) *
                    Operator<GenericInfo>(ordering_value(0), GenericInfo("B")) * Operator<GenericInfo>(9);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(36.000000 * A * B)\n");
}

TEST(number_tests, operator_and_1) {
  auto exp = Operator<GenericInfo>(ordering_value(0), GenericInfo("A")) * Operator<GenericInfo>(1)
             * Operator<GenericInfo>(ordering_value(0), GenericInfo("B"));
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(A * B)\n");
}

TEST(number_tests, single_one) {
  Expression<Fock1DInfo> exp;
  exp = exp + Operator<Fock1DInfo>(1);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(1.000000)\n");
}

TEST(number_tests, zero) {
  Expression<GenericInfo> exp = Operator<GenericInfo>(ordering_value(0), GenericInfo("A")) * Operator<GenericInfo>(0) *
                    Operator<GenericInfo>(ordering_value(0), GenericInfo("B")) * Operator<GenericInfo>(9);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "\n");
}

TEST(number_tests, addition_and_multiply) {
  Expression<GenericInfo> exp;
  const auto A = Operator<GenericInfo>(ordering_value(0), GenericInfo("A"));
  exp = exp + A + (Operator<GenericInfo>(0) * A) + (A * Operator<GenericInfo>(1) * A) + (A * A * A);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(A) + (A * A) + (A * A * A)\n");
}

TEST(number_tests, addition) {
  Expression<Fock1DInfo> exp = Operator<Fock1DInfo>(6) + Operator<Fock1DInfo>(7);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(13.000000)\n");
}

TEST(number_tests, opperator_addition) {
  Operator<GenericInfo> A(ordering_value(0), GenericInfo("A"));
  Expression<GenericInfo> exp = A + A;
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(2.000000 * A)\n");
}

TEST(number_tests, opperator_addition_multiply) {
  Operator<GenericInfo> A(ordering_value(0), GenericInfo("A"));
  Expression<GenericInfo> exp = (A * Operator<GenericInfo>(2)) + A;
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(3.000000 * A)\n");
}

TEST(number_tests, opperator_addition_multiply_2) {
  Operator<GenericInfo> A(ordering_value(0), GenericInfo("A"));
  Operator<GenericInfo> B(ordering_value(0), GenericInfo("B"));
  Expression<GenericInfo> exp = (A * B * Operator<GenericInfo>(2)) + (Operator<GenericInfo>(6) * A * B);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(8.000000 * A * B)\n");
}

TEST(number_tests, opperator_addition_multiply_3) {
  Operator<GenericInfo> A(ordering_value(0), GenericInfo("A"));
  Operator<GenericInfo> B(ordering_value(0), GenericInfo("B"));
  Expression<GenericInfo> exp = (A * B * Operator<GenericInfo>(2)) + (Operator<GenericInfo>(6) * B * A);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(2.000000 * A * B) + (6.000000 * B * A)\n");
}

TEST(number_tests, opperator_addition_multiply_4) {
  Operator<GenericInfo> A(ordering_value(0), GenericInfo("A"));
  Operator<GenericInfo> B(ordering_value(0), GenericInfo("B"));
  Expression<GenericInfo> exp = (A * B * Operator<GenericInfo>(2))
                    + (A * B * Operator<GenericInfo>(0))
                    + (B * A * Operator<GenericInfo>(1))
                    + (Operator<GenericInfo>(6) * A * B);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(8.000000 * A * B) + (B * A)\n");
}

TEST(number_tests, ones) {
  Operator<GenericInfo> A(ordering_value(0), GenericInfo("A"));
  Expression<GenericInfo> exp = A + number<GenericInfo>(1.0);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(A) + (1.000000)\n");
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
