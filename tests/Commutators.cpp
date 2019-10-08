#include "Expression.hpp"
#include "Operator.hpp"
#include "Comutators.hpp"
#include <sstream>
#define BOOST_TEST_MODULE Comutator
#include <boost/test/included/unit_test.hpp>

using namespace operators;

BOOST_AUTO_TEST_CASE(numbers) {
  auto exp = Operator<GenericInfo>(ordering_value(0), GenericInfo("A")) * Operator<GenericInfo>(8);
  exp = exp.sort(commute_none);
  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "(8.000000 * A)\n");
}

BOOST_AUTO_TEST_CASE(commute_two) {
  Operator<GenericInfo> A(ordering_value(1), GenericInfo("A"));
  Operator<GenericInfo> B(ordering_value(-1), GenericInfo("B"));

  auto exp = A * B;
  exp = exp.sort(commute_none);

  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "(B * A) + ([A, B])\n");
}

BOOST_AUTO_TEST_CASE(commute_three_1) {
  Operator<GenericInfo> A(ordering_value(1), GenericInfo("A"));
  Operator<GenericInfo> B(ordering_value(1), GenericInfo("B"));
  Operator<GenericInfo> C(ordering_value(-1), GenericInfo("C"));

  auto exp = A * B * C;
  exp = exp.sort(commute_none);

  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "(C * A * B) + ([B, C] * A) + ([A, C] * B) + ([A, [B, C]])\n");
}

BOOST_AUTO_TEST_CASE(commute_three_2) {
  Operator<GenericInfo> A(ordering_value(1), GenericInfo("A"));
  Operator<GenericInfo> B(ordering_value(-1), GenericInfo("B"));
  Operator<GenericInfo> C(ordering_value(-1), GenericInfo("C"));

  auto exp = A * B * C;
  exp = exp.sort(commute_none);

  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "(B * C * A) + (C * [A, B]) + (B * [A, C]) + ([[A, B], C])\n");
}

BOOST_AUTO_TEST_CASE(commute_three_number) {
  Operator<GenericInfo> A(ordering_value(1), GenericInfo("A"));
  Operator<GenericInfo> B(ordering_value(-1), GenericInfo("B"));
  Operator<GenericInfo> C(ordering_value(-1), GenericInfo("C"));

  auto exp = A * B * Operator<GenericInfo>(2) * C;
  exp = exp.sort(commute_none);

  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "(2.000000 * B * C * A) + (2.000000 * C * [A, B]) "
                      "+ (2.000000 * B * [A, C]) + (2.000000 * [[A, B], C])\n");
}

BOOST_AUTO_TEST_CASE(commute_four) {
  Operator<GenericInfo> A(ordering_value(-1), GenericInfo("A"));
  Operator<GenericInfo> B(ordering_value(-1), GenericInfo("B"));
  Operator<GenericInfo> C(ordering_value(-1), GenericInfo("C"));
  Operator<GenericInfo> D(ordering_value(-2), GenericInfo("D"));

  auto exp = A * B * C * D;
  exp = exp.sort(commute_none);

  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "(D * A * B * C) + (A * B * [C, D]) + (A * C * [B, D]) + "
                      "(B * C * [A, D]) + (A * [[B, D], C]) + (C * [[A, D], B]) + "
                      "(B * [[A, D], C]) + ([[[A, D], B], C])\n");
}

BOOST_AUTO_TEST_CASE(commute_and_addition) {
  Operator<GenericInfo> A(ordering_value(1), GenericInfo("A"));
  Operator<GenericInfo> B(ordering_value(-1), GenericInfo("B"));
  Operator<GenericInfo> C(ordering_value(2), GenericInfo("C"));
  Operator<GenericInfo> D(ordering_value(-2), GenericInfo("D"));

  auto exp = (A * B) + (C * D);
  exp = exp.sort(commute_none);

  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "(B * A) + ([A, B]) + (D * C) + ([C, D])\n");
}

BOOST_AUTO_TEST_CASE(commute_2) {
  Operator<GenericInfo> A(ordering_value(1), GenericInfo("A"));
  Operator<GenericInfo> B(ordering_value(-1), GenericInfo("B"));

  auto exp = A * B;
  exp = exp.sort(anticommute_none, SortUsing::ANTICOMMUTATORS);

  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "(-1.000000 * B * A) + ({A, B})\n");
}
