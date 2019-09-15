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
  Expression<Fock1DInfo> exp = Operator<Fock1DInfo>("A", ordering_value(0), Fock1DInfo(0)) * Operator<Fock1DInfo>(4) *
                    Operator<Fock1DInfo>("B", ordering_value(0), Fock1DInfo(0)) * Operator<Fock1DInfo>(9);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(36.000000 * A * B)\n");
}

TEST(number_tests, operator_and_1) {
  auto exp = Operator<Fock1DInfo>("A", ordering_value(0), Fock1DInfo(0)) * Operator<Fock1DInfo>(1)
             * Operator<Fock1DInfo>("B", ordering_value(0), Fock1DInfo(0));
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
  Expression<Fock1DInfo> exp = Operator<Fock1DInfo>("A", ordering_value(0), Fock1DInfo(0)) * Operator<Fock1DInfo>(0) *
                    Operator<Fock1DInfo>("B", ordering_value(0), Fock1DInfo(0)) * Operator<Fock1DInfo>(9);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "\n");
}

TEST(number_tests, addition_and_multiply) {
  Expression<Fock1DInfo> exp;
  const auto A = Operator<Fock1DInfo>("A", ordering_value(0), Fock1DInfo(0));
  exp = exp + A + (Operator<Fock1DInfo>(0) * A) + (A * Operator<Fock1DInfo>(1) * A) + (A * A * A);
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
  Operator<Fock1DInfo> A("A", ordering_value(0), Fock1DInfo(0));
  Expression<Fock1DInfo> exp = A + A;
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(2.000000 * A)\n");
}

TEST(number_tests, opperator_addition_multiply) {
  Operator<Fock1DInfo> A("A", ordering_value(0), Fock1DInfo(0));
  Expression<Fock1DInfo> exp = (A * Operator<Fock1DInfo>(2)) + A;
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(3.000000 * A)\n");
}

TEST(number_tests, opperator_addition_multiply_2) {
  Operator<Fock1DInfo> A("A", ordering_value(0), Fock1DInfo(0));
  Operator<Fock1DInfo> B("B", ordering_value(0), Fock1DInfo(1));
  Expression<Fock1DInfo> exp = (A * B * Operator<Fock1DInfo>(2)) + (Operator<Fock1DInfo>(6) * A * B);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(8.000000 * A * B)\n");
}

TEST(number_tests, opperator_addition_multiply_3) {
  Operator<Fock1DInfo> A("A", ordering_value(0), Fock1DInfo(0));
  Operator<Fock1DInfo> B("B", ordering_value(0), Fock1DInfo(1));
  Expression<Fock1DInfo> exp = (A * B * Operator<Fock1DInfo>(2)) + (Operator<Fock1DInfo>(6) * B * A);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(2.000000 * A * B) + (6.000000 * B * A)\n");
}

TEST(number_tests, opperator_addition_multiply_4) {
  Operator<Fock1DInfo> A("A", ordering_value(0), Fock1DInfo(0));
  Operator<Fock1DInfo> B("B", ordering_value(0), Fock1DInfo(1));
  Expression<Fock1DInfo> exp = (A * B * Operator<Fock1DInfo>(2))
                    + (A * B * Operator<Fock1DInfo>(0))
                    + (B * A * Operator<Fock1DInfo>(1))
                    + (Operator<Fock1DInfo>(6) * A * B);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(8.000000 * A * B) + (B * A)\n");
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
