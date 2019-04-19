#include "gtest/gtest.h"
#include "Expression.hpp"
#include "Operator.hpp"
#include "Comutators.hpp"
#include <sstream>

using namespace operators;

TEST(commutator_tests, numbers) {
  auto exp = Operator("A", ordering_value(0), operator_info(0)) * Operator(8);
  exp = exp.sort(commute_none);
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(8.000000 * A)\n");
}

TEST(commutator_tests, commute_two) {
  Operator A("A", ordering_value(1), operator_info(1));
  Operator B("B", ordering_value(-1), operator_info(-1));

  auto exp = A * B;
  exp = exp.sort(commute_none);

  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(B * A) + ([A, B])\n");
}

TEST(commutator_tests, commute_three_1) {
  Operator A("A", ordering_value(1), operator_info(1));
  Operator B("B", ordering_value(1), operator_info(1));
  Operator C("C", ordering_value(-1), operator_info(-1));

  auto exp = A * B * C;
  exp = exp.sort(commute_none);

  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(C * A * B) + ([B, C] * A) + ([A, C] * B) + ([A, [B, C]])\n");
}

TEST(commutator_tests, commute_three_2) {
  Operator A("A", ordering_value(1), operator_info(1));
  Operator B("B", ordering_value(-1), operator_info(-1));
  Operator C("C", ordering_value(-1), operator_info(-1));

  auto exp = A * B * C;
  exp = exp.sort(commute_none);

  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(B * C * A) + (C * [A, B]) + (B * [A, C]) + ([[A, B], C])\n");
}

TEST(commutator_tests, commute_three_number) {
  Operator A("A", ordering_value(1), operator_info(1));
  Operator B("B", ordering_value(-1), operator_info(-1));
  Operator C("C", ordering_value(-1), operator_info(-1));

  auto exp = A * B * Operator(2) * C;
  exp = exp.sort(commute_none);

  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(2.000000 * B * C * A) + (2.000000 * C * [A, B]) "
                      "+ (2.000000 * B * [A, C]) + (2.000000 * [[A, B], C])\n");
}

TEST(commutator_tests, commute_four) {
  Operator A("A", ordering_value(-1), operator_info(-1));
  Operator B("B", ordering_value(-1), operator_info(-1));
  Operator C("C", ordering_value(-1), operator_info(-1));
  Operator D("D", ordering_value(-2), operator_info(-2));

  auto exp = A * B * C * D;
  exp = exp.sort(commute_none);

  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(D * A * B * C) + (A * B * [C, D]) + (A * C * [B, D]) + "
                      "(B * C * [A, D]) + (A * [[B, D], C]) + (C * [[A, D], B]) + "
                      "(B * [[A, D], C]) + ([[[A, D], B], C])\n");
}

TEST(commutator_tests, commute_and_addition) {
  Operator A("A", ordering_value(1), operator_info(1));
  Operator B("B", ordering_value(-1), operator_info(-1));
  Operator C("C", ordering_value(2), operator_info(2));
  Operator D("D", ordering_value(-2), operator_info(-2));

  auto exp = (A * B) + (C * D);
  exp = exp.sort(commute_none);

  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(B * A) + ([A, B]) + (D * C) + ([C, D])\n");
}

TEST(anti_commutator_tests, commute_two) {
  Operator A("A", ordering_value(1), operator_info(1));
  Operator B("B", ordering_value(-1), operator_info(-1));

  auto exp = A * B;
  exp = exp.sort(anticommute_none, SortUsing::ANTICOMMUTATORS);

  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(-1.000000 * B * A) + ({A, B})\n");
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
