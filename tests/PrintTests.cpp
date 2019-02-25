#include "gtest/gtest.h"
#include "Expression.hpp"
#include "Operator.hpp"
#include <sstream>

using namespace operators;

TEST (printing_tests, basic_addition) {
  Operator A("A", ordering_value(0), operator_id(0));
  Operator B("B", ordering_value(0), operator_id(1));

  const auto exp = A + B;
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(A) + (B)\n");
}

TEST(printing_tests, empty) {
  Expression exp;
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "\n");

  exp.expression.resize(3);
  std::stringstream ss2;
  exp.print(ss2);
  ASSERT_EQ(ss2.str(), "\n");

  exp.expression[0].push_back(Operator(5));
  std::stringstream ss3;
  exp.print(ss3);
  ASSERT_EQ(ss3.str(), "(5)\n");
}

TEST(printing_tests, basic_multiplication) {
  Operator A("A", ordering_value(0), operator_id(0));
  Operator B("B", ordering_value(0), operator_id(1));

  const auto exp = A * B;
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(A * B)\n");
}

TEST(printing_tests, add_and_multiply) {
  Operator A("A", ordering_value(0), operator_id(0));
  Operator B("B", ordering_value(0), operator_id(1));
  Operator C("C", ordering_value(0), operator_id(2));
  Operator D("D", ordering_value(0), operator_id(3));

  const auto exp1 = A * B;
  const auto exp2 = C * D;
  const auto exp3 = A * C;
  const auto exp4 = B * D;

  const auto exp = exp1 + exp2 + exp3 + exp4;

  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(A * B) + (C * D) + (A * C) + (B * D)\n");
}

TEST(printing_tests, multiply_expressions) {
  Operator A("A", ordering_value(0), operator_id(0));
  Operator B("B", ordering_value(0), operator_id(1));
  Operator C("C", ordering_value(0), operator_id(2));
  Operator D("D", ordering_value(0), operator_id(3));
  Operator E("E", ordering_value(0), operator_id(4));

  const auto exp = (A + B + C) * (D + E);

  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(A * D) + (B * D) + (C * D) + (A * E) + (B * E) + (C * E)\n");
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
