#include "gtest/gtest.h"
#include "Expression.hpp"
#include <sstream>
#include <iomanip>
#include "struct_ids.hpp"

using namespace operators;

TEST (printing_tests, basic_addition) {
  Operator<Fock1DInfo> A("A", ordering_value(0), Fock1DInfo(0));
  Operator<Fock1DInfo> B("B", ordering_value(0), Fock1DInfo(1));

  const auto exp = A + B;
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(A) + (B)\n");
}

TEST(printing_tests, empty) {
  Expression<Fock1DInfo> exp;
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "\n");

  exp.expression.resize(3);
  std::stringstream ss2;
  exp.print(ss2);
  ASSERT_EQ(ss2.str(), "\n");

  exp.expression[0].push_back(Operator<Fock1DInfo>(5));
  std::stringstream ss3;
  exp.print(ss3);
  ASSERT_EQ(ss3.str(), "(5.000000)\n");
}

TEST(printing_tests, basic_multiplication) {
  Operator<Fock1DInfo> A("A", ordering_value(0), Fock1DInfo(0));
  Operator<Fock1DInfo> B("B", ordering_value(0), Fock1DInfo(1));

  const auto exp = A * B;
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(A * B)\n");
}

TEST(printing_tests, add_and_multiply) {
  Operator<Fock1DInfo> A("A", ordering_value(0), Fock1DInfo(0));
  Operator<Fock1DInfo> B("B", ordering_value(0), Fock1DInfo(1));
  Operator<Fock1DInfo> C("C", ordering_value(0), Fock1DInfo(2));
  Operator<Fock1DInfo> D("D", ordering_value(0), Fock1DInfo(3));

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
  Operator<Fock1DInfo> A("A", ordering_value(0), Fock1DInfo(0));
  Operator<Fock1DInfo> B("B", ordering_value(0), Fock1DInfo(1));
  Operator<Fock1DInfo> C("C", ordering_value(0), Fock1DInfo(2));
  Operator<Fock1DInfo> D("D", ordering_value(0), Fock1DInfo(3));
  Operator<Fock1DInfo> E("E", ordering_value(0), Fock1DInfo(4));
  Operator<Fock1DInfo> F("F", ordering_value(0), Fock1DInfo(5));

  const auto exp = (A + B + C) * ((D * F) + E);

  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(A * D * F) + (B * D * F) + (C * D * F) + (A * E) + (B * E) + (C * E)\n");
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
