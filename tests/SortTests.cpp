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

static void random_multiply_and_addition(const unsigned seed) {
  std::mt19937 random_engine(seed);
  const unsigned num_add = (random_engine() % 20) + 2;

  std::vector<std::vector<int>> reference(num_add);
  for (unsigned i = 0; i < reference.size(); ++i) {
    const unsigned num_mult = (random_engine() % 20) + 2;
    for (unsigned j = 0; j < num_mult; ++j) {
      reference[i].push_back((random_engine() % 500));
    }
  }

  Expression exp;
  for (const auto & vec : reference) {
    Expression mul_term =  Operator(std::to_string(vec[0]), ordering_value(vec[0]), operator_id(vec[0]))
                           * Operator(std::to_string(vec[1]), ordering_value(vec[1]), operator_id(vec[1]));
    for (unsigned j = 2; j < vec.size(); ++j) {
      mul_term = mul_term * Operator(std::to_string(vec[j]), ordering_value(vec[j]), operator_id(vec[j]));
    }
    exp = exp + mul_term;
  }

  for (unsigned i = 0; i < reference.size(); ++i) {
    for (unsigned j = 0; j < reference[i].size(); ++j) {
      ASSERT_EQ(exp.expression[i][j].order.value, reference[i][j]);
    }
  }

  for (auto & vec : reference) {
    std::sort(vec.begin(), vec.end());
  }
  const auto new_exp = exp.sort(commute_all);

  for (unsigned i = 0; i < reference.size(); ++i) {
    for (unsigned j = 0; j < reference[i].size(); ++j) {
      EXPECT_EQ(new_exp.expression[i][j].order.value, reference[i][j]);
      EXPECT_EQ(new_exp.expression[i][j].order.value, new_exp.expression[i][j].id.value);
    }
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

TEST(sort_tests, number) {
  auto exp = Operator("A", ordering_value(0), operator_id(0)) * Operator(3);
  exp = exp.sort(commute_all);
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(3 * A)\n");
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

class AdditionAndMultiplyRandomSort : public ::testing::TestWithParam<unsigned> {
  // You can implement all the usual class fixture members here.
};

TEST_P(AdditionAndMultiplyRandomSort, random_add_sort) {
  const unsigned m = GetParam();
  const unsigned seed = m + 1000; // don't use same seed as random sort tests
  random_multiply_and_addition(seed);
}

INSTANTIATE_TEST_SUITE_P(AdditionAndMultiplyTests, AdditionAndMultiplyRandomSort, testing::Range(0U, 30U));

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
