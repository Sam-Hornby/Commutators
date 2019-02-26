#include "gtest/gtest.h"
#include "Expression.hpp"
#include "Operator.hpp"
#include "Comutators.hpp"
#include <random>
#include <vector>
#include <sstream>
#include <utility>
#include "gtest/internal/gtest-internal.h"
#include "gtest/internal/gtest-param-util.h"
#include "gtest/internal/gtest-param-util-generated.h"
#include "gtest/internal/gtest-port.h"


using namespace operators;

// If all operators commute  then should produce same results as std::sort
// Gerenate tests comparing 2
static void random_sort_test(const unsigned seed, const std::size_t n, const int max, const int min) {
  std::mt19937 randomEngine(seed);
  const int mod = max - min;
  std::vector<int> reference;
  for (unsigned i = 0; i < n; ++i) {
    reference.push_back((randomEngine() % mod) + min);
  }

  Operator A(std::to_string(reference[0]), ordering_value(reference[0]), operator_id(reference[0]));
  Operator B(std::to_string(reference[1]), ordering_value(reference[1]), operator_id(reference[1]));
  Expression exp = A * B;
  for (unsigned i = 2; i < reference.size(); ++i) {
    exp = exp * Operator(std::to_string(reference[i]), ordering_value(reference[i]), operator_id(reference[i]));
  }

  for (unsigned i = 0; i < reference.size(); ++i) {
    ASSERT_EQ(exp.expression[0][i].order.value, reference[i]);
  }

  std::sort(reference.begin(), reference.end());
  const auto new_exp = exp.sort(commute_all);

  for (unsigned i = 0; i < reference.size(); ++i) {
    EXPECT_EQ(new_exp.expression[0][i].order.value, reference[i]);
    EXPECT_EQ(new_exp.expression[0][i].order.value, new_exp.expression[0][i].id.value);
  }
}

TEST(sort_tests, empty) {
  Expression exp;
  auto new_exp = exp.sort(commute_all);
  std::stringstream ss;
  new_exp.print(ss);
  ASSERT_EQ(ss.str(), "\n");
}

TEST(sort_tests, one) {
  Expression exp;
  Operator A("A", ordering_value(0), operator_id(0));
  exp.expression.resize(3);
  exp.expression[0].push_back(A);
  auto new_exp = exp.sort(commute_all);
  std::stringstream ss;
  new_exp.print(ss);
  ASSERT_EQ(ss.str(), "(A)\n");
}

class RandomSortTest : public ::testing::TestWithParam<unsigned> {
  // You can implement all the usual class fixture members here.
};

TEST_P (RandomSortTest, random_sort) {
  const unsigned seed = GetParam();
  const unsigned n = seed;
  random_sort_test(seed, n, 100, -100);
}

INSTANTIATE_TEST_SUITE_P(RandomSortingTests, RandomSortTest, testing::Range(3U, 100U));


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
