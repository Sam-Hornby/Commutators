#include "gtest/gtest.h"
#include "Expression.hpp"
#include "Operator.hpp"
#include <sstream>

using namespace operators;

TEST (printing_tests, basic) {
  // the test
  Operator A("A", ordering_value(0), operator_id(0));
  Operator B("B", ordering_value(0), operator_id(1));

  //const auto exp = A + B;
  Expression exp;
  exp.expression.resize(2);
  exp.expression[0].push_back(A);
  exp.expression[1].push_back(B);
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(A) + (B)\n");
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
