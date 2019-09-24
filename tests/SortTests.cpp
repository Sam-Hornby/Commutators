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
static void random_sort_test(const unsigned seed, const std::size_t n, const int max, const int min, const bool inter) {
  std::mt19937 randomEngine(seed);
  const int mod = max - min;
  std::vector<int> reference;
  for (unsigned i = 0; i < n; ++i) {
    reference.push_back((randomEngine() % mod) + min);
  }

  Operator<GenericInfo> A(ordering_value(reference[0]), GenericInfo(std::to_string(reference[0])));
  Operator<GenericInfo> B(ordering_value(reference[1]), GenericInfo(std::to_string(reference[1])));
  Expression<GenericInfo> exp = A * B;
  for (unsigned i = 2; i < reference.size(); ++i) {
    exp = exp * Operator<GenericInfo>(ordering_value(reference[i]), GenericInfo(std::to_string(reference[i])));
  }

  for (unsigned i = 0; i < reference.size(); ++i) {
    ASSERT_EQ(exp.expression[0][i].order.value, reference[i]);
  }

  std::sort(reference.begin(), reference.end());
  const Expression<GenericInfo> new_exp = [&] () {
    if (inter) {
      return exp.interleaved_evaluate(commute_all<GenericInfo>,
                                          [](typename std::vector<Operator<GenericInfo>>::iterator,
                                                    std::vector<Operator<GenericInfo>> &) {return false;});
    } else {
      return exp.sort(commute_all<GenericInfo>);
    }
  } ();

  for (unsigned i = 0; i < reference.size(); ++i) {
    EXPECT_EQ(new_exp.expression[0][i].order.value, reference[i]);
    EXPECT_EQ(std::to_string(new_exp.expression[0][i].order.value), new_exp.expression[0][i].info.name());
  }
}

static void random_multiply_and_addition(const unsigned seed, const bool inter) {
  std::mt19937 random_engine(seed);
  const unsigned num_add = (random_engine() % 20) + 2;

  std::vector<std::vector<int>> reference(num_add);
  for (unsigned i = 0; i < reference.size(); ++i) {
    const unsigned num_mult = (random_engine() % 20) + 2;
    for (unsigned j = 0; j < num_mult; ++j) {
      reference[i].push_back((random_engine() % 500));
    }
  }

  Expression<GenericInfo> exp;
  for (const auto & vec : reference) {
    Expression<GenericInfo> mul_term =  Operator<GenericInfo>(ordering_value(vec[0]), GenericInfo(std::to_string(vec[0])))
                           * Operator<GenericInfo>(ordering_value(vec[1]), GenericInfo(std::to_string(vec[1])));
    for (unsigned j = 2; j < vec.size(); ++j) {
      mul_term = mul_term * Operator<GenericInfo>(ordering_value(vec[j]), GenericInfo(std::to_string(vec[j])));
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
  const auto new_exp = [&] () {
    if (inter) {
      return exp.interleaved_evaluate(commute_all<GenericInfo>,
                                          [](typename std::vector<Operator<GenericInfo>>::iterator,
                                                    std::vector<Operator<GenericInfo>> &) {return false;});
    } else {
      return exp.sort(commute_all<GenericInfo>);
    }
  } ();
  for (unsigned i = 0; i < reference.size(); ++i) {
    for (unsigned j = 0; j < reference[i].size(); ++j) {
      EXPECT_EQ(new_exp.expression[i][j].order.value, reference[i][j]);
      EXPECT_EQ(std::to_string(new_exp.expression[i][j].order.value), new_exp.expression[i][j].info.name());
    }
  }
}

TEST(sort_tests, empty) {
  Expression<Fock1DInfo> exp;
  auto new_exp = exp.sort(commute_all<Fock1DInfo>);
  std::stringstream ss;
  new_exp.print(ss);
  ASSERT_EQ(ss.str(), "\n");
}

TEST(sort_tests, one) {
  Expression<GenericInfo> exp;
  Operator<GenericInfo> A(ordering_value(0), GenericInfo("A"));
  exp.expression.resize(3);
  exp.expression[0].push_back(A);
  auto new_exp = exp.sort(commute_all<GenericInfo>);
  std::stringstream ss;
  new_exp.print(ss);
  ASSERT_EQ(ss.str(), "(A)\n");
}

TEST(sort_tests, number) {
  auto exp = Operator<GenericInfo>(ordering_value(0), GenericInfo("A")) * Operator<GenericInfo>(3);
  exp = exp.sort(commute_all<GenericInfo>);
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(3.000000 * A)\n");
}

// interleaver_evaluate with empty substitutions tests
TEST(sort_tests, inter_empty) {
  Expression<Fock1DInfo> exp;
  auto new_exp = exp.interleaved_evaluate(commute_all<Fock1DInfo>,
                                          [](typename std::vector<Operator<Fock1DInfo>>::iterator,
                                                    std::vector<Operator<Fock1DInfo>> &) {return false;});
  std::stringstream ss;
  new_exp.print(ss);
  ASSERT_EQ(ss.str(), "\n");
}

TEST(sort_tests, inter_one) {
  Expression<GenericInfo> exp;
  Operator<GenericInfo> A(ordering_value(0), GenericInfo("A"));
  exp.expression.resize(3);
  exp.expression[0].push_back(A);
  auto new_exp = exp.interleaved_evaluate(commute_all<GenericInfo>,
                                          [](typename std::vector<Operator<GenericInfo>>::iterator,
                                                    std::vector<Operator<GenericInfo>> &) {return false;});
  std::stringstream ss;
  new_exp.print(ss);
  ASSERT_EQ(ss.str(), "(A)\n");
}

TEST(sort_tests, inter_number) {
  auto exp = Operator<GenericInfo>(ordering_value(0), GenericInfo("A")) * Operator<GenericInfo>(3);
  exp = exp.interleaved_evaluate(commute_all<GenericInfo>,
                                          [](typename std::vector<Operator<GenericInfo>>::iterator,
                                                    std::vector<Operator<GenericInfo>> &) {return false;});
  std::stringstream ss;
  exp.print(ss);
  ASSERT_EQ(ss.str(), "(3.000000 * A)\n");
}

class RandomSortTest : public ::testing::TestWithParam<unsigned> {
  // You can implement all the usual class fixture members here.
};

class InterRandomSortTest : public ::testing::TestWithParam<unsigned> {
  // You can implement all the usual class fixture members here.
};

TEST_P (RandomSortTest, random_sort) {
  const unsigned seed = GetParam();
  const unsigned n = seed;
  random_sort_test(seed, n, 100, -100, false);
}

TEST_P (InterRandomSortTest, inter_random_sort) {
  const unsigned seed = GetParam();
  const unsigned n = seed;
  random_sort_test(seed, n, 100, -100, true);
}

INSTANTIATE_TEST_SUITE_P(RandomSortingTests, RandomSortTest, testing::Range(3U, 100U));
INSTANTIATE_TEST_SUITE_P(InterRandomSortingTests, InterRandomSortTest, testing::Range(3U, 100U));

class AdditionAndMultiplyRandomSort : public ::testing::TestWithParam<unsigned> {
  // You can implement all the usual class fixture members here.
};

TEST_P(AdditionAndMultiplyRandomSort, random_add_sort) {
  const unsigned m = GetParam();
  const unsigned seed = m + 1000; // don't use same seed as random sort tests
  random_multiply_and_addition(seed, false);
}

TEST_P(AdditionAndMultiplyRandomSort, inter_random_add_sort) {
  const unsigned m = GetParam();
  const unsigned seed = m + 1000; // don't use same seed as random sort tests
  random_multiply_and_addition(seed, true);
}

INSTANTIATE_TEST_SUITE_P(AdditionAndMultiplyTests, AdditionAndMultiplyRandomSort, testing::Range(0U, 30U));
INSTANTIATE_TEST_SUITE_P(InterAdditionAndMultiplyTests, AdditionAndMultiplyRandomSort, testing::Range(0U, 30U));

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
