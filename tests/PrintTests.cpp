#include "gtest/gtest.h"
#include "Expression.hpp"
#include <sstream>
#include <iomanip>
#include "struct_ids.hpp"
#include "Comutators.hpp"

using namespace operators;

TEST (printing_tests, basic_addition) {
  Operator<GenericInfo> A(ordering_value(0), GenericInfo("A"));
  Operator<GenericInfo> B(ordering_value(0), GenericInfo("B"));

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
  Operator<GenericInfo> A(ordering_value(0), GenericInfo("A"));
  Operator<GenericInfo> B(ordering_value(0), GenericInfo("B"));

  const auto exp = A * B;
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(A * B)\n");
}

TEST(printing_tests, add_and_multiply) {
  Operator<GenericInfo> A(ordering_value(0), GenericInfo("A"));
  Operator<GenericInfo> B(ordering_value(0), GenericInfo("B"));
  Operator<GenericInfo> C(ordering_value(0), GenericInfo("C"));
  Operator<GenericInfo> D(ordering_value(0), GenericInfo("D"));

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
  Operator<GenericInfo> A(ordering_value(0), GenericInfo("A"));
  Operator<GenericInfo> B(ordering_value(0), GenericInfo("B"));
  Operator<GenericInfo> C(ordering_value(0), GenericInfo("C"));
  Operator<GenericInfo> D(ordering_value(0), GenericInfo("D"));
  Operator<GenericInfo> E(ordering_value(0), GenericInfo("E"));
  Operator<GenericInfo> F(ordering_value(0), GenericInfo("F"));

  const auto exp = (A + B + C) * ((D * F) + E);

  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(A * D * F) + (B * D * F) + (C * D * F) + (A * E) + (B * E) + (C * E)\n");
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
