#include "gtest/gtest.h"
#include "Expression.hpp"
#include "Operator.hpp"
#include "Comutators.hpp"
#include <sstream>

using namespace operators;


TEST(number_tests, empty) {
  Expression exp;
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "\n");
}

TEST(number_tests, single) {
  Expression exp;
  exp = exp + Operator(5);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(5)\n");
}

TEST(number_tests, two_numbers) {
  Expression exp = Operator(5) * Operator(7);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(35)\n");
}


TEST(number_tests, operators_and_numbers) {
  Expression exp = Operator("A", ordering_value(0), operator_info(0)) * Operator(4) *
                    Operator("B", ordering_value(0), operator_info(0)) * Operator(9);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(36 * A * B)\n");
}

TEST(number_tests, operator_and_1) {
  auto exp = Operator("A", ordering_value(0), operator_info(0)) * Operator(1)
             * Operator("B", ordering_value(0), operator_info(0));
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(A * B)\n");
}

TEST(number_tests, single_one) {
  Expression exp;
  exp = exp + Operator(1);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(1)\n");
}

TEST(number_tests, zero) {
  Expression exp = Operator("A", ordering_value(0), operator_info(0)) * Operator(0) *
                    Operator("B", ordering_value(0), operator_info(0)) * Operator(9);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "\n");
}

TEST(number_tests, addition_and_multiply) {
  Expression exp;
  const auto A = Operator("A", ordering_value(0), operator_info(0));
  exp = exp + A + (Operator(0) * A) + (A * Operator(1) * A) + (A * A * A);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(A) + (A * A) + (A * A * A)\n");
}

TEST(number_tests, addition) {
  Expression exp = Operator(6) + Operator(7);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(13)\n");
}

TEST(number_tests, opperator_addition) {
  Operator A("A", ordering_value(0), operator_info(0));
  Expression exp = A + A;
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(2 * A)\n");
}

TEST(number_tests, opperator_addition_multiply) {
  Operator A("A", ordering_value(0), operator_info(0));
  Expression exp = (A * Operator(2)) + A;
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(3 * A)\n");
}

TEST(number_tests, opperator_addition_multiply_2) {
  Operator A("A", ordering_value(0), operator_info(0));
  Operator B("B", ordering_value(0), operator_info(1));
  Expression exp = (A * B * Operator(2)) + (Operator(6) * A * B);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(8 * A * B)\n");
}

TEST(number_tests, opperator_addition_multiply_3) {
  Operator A("A", ordering_value(0), operator_info(0));
  Operator B("B", ordering_value(0), operator_info(1));
  Expression exp = (A * B * Operator(2)) + (Operator(6) * B * A);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(2 * A * B) + (6 * B * A)\n");
}

TEST(number_tests, opperator_addition_multiply_4) {
  Operator A("A", ordering_value(0), operator_info(0));
  Operator B("B", ordering_value(0), operator_info(1));
  Expression exp = (A * B * Operator(2))
                    + (A * B * Operator(0))
                    + (B * A * Operator(1))
                    + (Operator(6) * A * B);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(8 * A * B) + (B * A)\n");
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
