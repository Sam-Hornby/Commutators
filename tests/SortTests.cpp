#include <Expression/Expression.hpp>
#include <Expression/Operator.hpp>
#include <Comutators/Comutators.hpp>
#include <random>
#include <vector>
#include <sstream>
#include <utility>
#define BOOST_TEST_MODULE Sort
#include <boost/test/included/unit_test.hpp>



using namespace operators;

// If all operators commute  then should produce same results as std::sort
// Gerenate tests comparing 2
static void random_sort_test(const unsigned seed, const std::size_t n, const int max, const int min, const bool inter) {
  spdlog::set_level(spdlog::level::critical);
  std::mt19937 randomEngine(seed);
  const int mod = max - min;
  vector_type<int> reference;
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
    BOOST_CHECK_EQUAL(exp.expression[0][i].order.value, reference[i]);
  }

  std::sort(reference.begin(), reference.end());
  const Expression<GenericInfo> new_exp = [&] () {
    if (inter) {
      return exp.interleaved_evaluate(commute_all<GenericInfo>,
                                          [](typename vector_type<Operator<GenericInfo>>::iterator,
                                                    vector_type<Operator<GenericInfo>> &) {return false;});
    } else {
      return exp.sort(commute_all<GenericInfo>);
    }
  } ();

  for (unsigned i = 0; i < reference.size(); ++i) {
    BOOST_CHECK_EQUAL(new_exp.expression[0][i].order.value, reference[i]);
    BOOST_CHECK_EQUAL(std::to_string(new_exp.expression[0][i].order.value), new_exp.expression[0][i].name());
  }
}

static void random_multiply_and_addition(const unsigned seed, const bool inter) {
  spdlog::set_level(spdlog::level::critical);
  std::mt19937 random_engine(seed);
  const unsigned num_add = (random_engine() % 20) + 2;

  vector_type<vector_type<int>> reference(num_add);
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
      BOOST_CHECK_EQUAL(exp.expression[i][j].order.value, reference[i][j]);
    }
  }

  for (auto & vec : reference) {
    std::sort(vec.begin(), vec.end());
  }
  const auto new_exp = [&] () {
    if (inter) {
      return exp.interleaved_evaluate(commute_all<GenericInfo>,
                                          [](typename vector_type<Operator<GenericInfo>>::iterator,
                                                    vector_type<Operator<GenericInfo>> &) {return false;});
    } else {
      return exp.sort(commute_all<GenericInfo>);
    }
  } ();
  for (unsigned i = 0; i < reference.size(); ++i) {
    for (unsigned j = 0; j < reference[i].size(); ++j) {
      BOOST_CHECK_EQUAL(new_exp.expression[i][j].order.value, reference[i][j]);
      BOOST_CHECK_EQUAL(std::to_string(new_exp.expression[i][j].order.value), new_exp.expression[i][j].name());
    }
  }
}

BOOST_AUTO_TEST_CASE(empty) {
  spdlog::set_level(spdlog::level::critical);
  Expression<Fock1DInfo> exp;
  auto new_exp = exp.sort(commute_all<Fock1DInfo>);
  std::stringstream ss;
  new_exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "\n");
}

BOOST_AUTO_TEST_CASE(one) {
  Expression<GenericInfo> exp;
  Operator<GenericInfo> A(ordering_value(0), GenericInfo("A"));
  exp.expression.resize(3);
  exp.expression[0].push_back(A);
  auto new_exp = exp.sort(commute_all<GenericInfo>);
  std::stringstream ss;
  new_exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "(A)\n");
}

BOOST_AUTO_TEST_CASE(numberss) {
  auto exp = Operator<GenericInfo>(ordering_value(0), GenericInfo("A"))
           * named_number<GenericInfo>('z')
           * Operator<GenericInfo>(3);
  exp = exp.sort(commute_all<GenericInfo>);
  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "(z * 3.000000 * A)\n");
}

// interleaver_evaluate with empty substitutions tests
BOOST_AUTO_TEST_CASE(inter_empty) {
  Expression<Fock1DInfo> exp;
  auto new_exp = exp.interleaved_evaluate(commute_all<Fock1DInfo>,
                                          [](typename vector_type<Operator<Fock1DInfo>>::iterator,
                                                    vector_type<Operator<Fock1DInfo>> &) {return false;});
  std::stringstream ss;
  new_exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "\n");
}

BOOST_AUTO_TEST_CASE(inter_one) {
  Expression<GenericInfo> exp;
  Operator<GenericInfo> A(ordering_value(0), GenericInfo("A"));
  exp.expression.resize(3);
  exp.expression[0].push_back(A);
  auto new_exp = exp.interleaved_evaluate(commute_all<GenericInfo>,
                                          [](typename vector_type<Operator<GenericInfo>>::iterator,
                                                    vector_type<Operator<GenericInfo>> &) {return false;});
  std::stringstream ss;
  new_exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "(A)\n");
}

BOOST_AUTO_TEST_CASE(inter_number) {
  auto exp = Operator<GenericInfo>(ordering_value(0), GenericInfo("A")) * Operator<GenericInfo>(3);
  exp = exp.interleaved_evaluate(commute_all<GenericInfo>,
                                          [](typename vector_type<Operator<GenericInfo>>::iterator,
                                                    vector_type<Operator<GenericInfo>> &) {return false;});
  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "(3.000000 * A)\n");
}

BOOST_AUTO_TEST_CASE(random_sort) {
  for (unsigned seed = 3U; seed < 100U; ++seed) {
    const unsigned n = seed;
    random_sort_test(seed, n, 100, -100, false);
    BOOST_TEST_MESSAGE("Finished seed " << seed);
  }
}

BOOST_AUTO_TEST_CASE(inter_random_sort) {
  for (unsigned seed = 3U; seed < 100U; ++seed) {
    const unsigned n = seed;
    random_sort_test(seed, n, 100, -100, true);
    BOOST_TEST_MESSAGE("Finished seed " << seed);
  }
}

BOOST_AUTO_TEST_CASE(random_add_sort) {
  for (unsigned m = 0; m < 30U; ++m) {
    const unsigned seed = m + 1000; // don't use same seed as random sort tests
    random_multiply_and_addition(seed, false);
    BOOST_TEST_MESSAGE("Finished m " << m);
  }
}

BOOST_AUTO_TEST_CASE(inter_random_add_sort) {
  for (unsigned m = 0; m < 30U; ++m) {
    const unsigned seed = m + 1000; // don't use same seed as random sort tests
    random_multiply_and_addition(seed, true);
    BOOST_TEST_MESSAGE("Finished m " << m);
  }
}
