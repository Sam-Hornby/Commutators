#include "gtest/gtest.h"
#include "Expression.hpp"
#include "Operator.hpp"
#include "Comutators.hpp"
#include <sstream>

using namespace operators;

TEST(commutator_tests, numbers) {
  auto exp = Operator("A", ordering_value(0), operator_id(0)) * Operator(8);
  exp = exp.sort(commute_none);
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(8 * A)\n");
}

TEST(commutator_tests, commute_two) {
  Operator A("A", ordering_value(1), operator_id(1));
  Operator B("B", ordering_value(-1), operator_id(-1));

  auto exp = A * B;
  exp = exp.sort(commute_none);

  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(B * A) + ([A, B])\n");
}

TEST(commutator_tests, commute_three_1) {
  Operator A("A", ordering_value(1), operator_id(1));
  Operator B("B", ordering_value(1), operator_id(1));
  Operator C("C", ordering_value(-1), operator_id(-1));

  auto exp = A * B * C;
  exp = exp.sort(commute_none);

  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(C * A * B) + ([B, C] * A) + ([A, C] * B) + ([A, [B, C]])\n");
}

TEST(commutator_tests, commute_three_2) {
  Operator A("A", ordering_value(1), operator_id(1));
  Operator B("B", ordering_value(-1), operator_id(-1));
  Operator C("C", ordering_value(-1), operator_id(-1));

  auto exp = A * B * C;
  exp = exp.sort(commute_none);

  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(B * C * A) + (C * [A, B]) + (B * [A, C]) + ([[A, B], C])\n");
}

TEST(commutator_tests, commute_three_number) {
  Operator A("A", ordering_value(1), operator_id(1));
  Operator B("B", ordering_value(-1), operator_id(-1));
  Operator C("C", ordering_value(-1), operator_id(-1));

  auto exp = A * B * Operator(2) * C;
  exp = exp.sort(commute_none);

  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(2 * B * C * A) + (2 * C * [A, B]) + (2 * B * [A, C]) + (2 * [[A, B], C])\n");
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
