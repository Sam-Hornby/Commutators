#include <Expression/Expression.hpp>
#include <NestedExpressions/Sum.hpp>
#include <sstream>
#include <iomanip>
#include <Expression/struct_ids.hpp>
#include <Expression/Infos/Fock1D.hpp>
#include <Expression/Infos/GenericInfo.hpp>
#define BOOST_TEST_MODULE Print
#include <boost/test/included/unit_test.hpp>


using namespace operators;

BOOST_AUTO_TEST_CASE(basic_addition) {
  Operator<GenericInfo> A(ordering_value(0), GenericInfo("A"));
  Operator<GenericInfo> B(ordering_value(0), GenericInfo("B"));

  const auto exp = A + B;
  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "(A) + (B)\n");
}

BOOST_AUTO_TEST_CASE(empty) {
  Expression<Fock1DInfo> exp;
  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "\n");

  exp.expression.resize(3);
  std::stringstream ss2;
  exp.print(ss2);
  BOOST_CHECK_EQUAL(ss2.str(), "\n");

  exp.expression[0].push_back(Operator<Fock1DInfo>(5));
  std::stringstream ss3;
  exp.print(ss3);
  BOOST_CHECK_EQUAL(ss3.str(), "(5.000000)\n");
}

BOOST_AUTO_TEST_CASE(basic_multiplication) {
  Operator<GenericInfo> A(ordering_value(0), GenericInfo("A"));
  Operator<GenericInfo> B(ordering_value(0), GenericInfo("B"));

  const auto exp = A * B;
  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "A * B\n");
}

BOOST_AUTO_TEST_CASE(add_and_multiply) {
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
  BOOST_CHECK_EQUAL(ss.str(), "(A * B) + (C * D) + (A * C) + (B * D)\n");
}

BOOST_AUTO_TEST_CASE(multiply_expressions) {
  Operator<GenericInfo> A(ordering_value(0), GenericInfo("A"));
  Operator<GenericInfo> B(ordering_value(0), GenericInfo("B"));
  Operator<GenericInfo> C(ordering_value(0), GenericInfo("C"));
  Operator<GenericInfo> D(ordering_value(0), GenericInfo("D"));
  Operator<GenericInfo> E(ordering_value(0), GenericInfo("E"));
  Operator<GenericInfo> F(ordering_value(0), GenericInfo("F"));

  const auto exp = (A + B + C) * ((D * F) + E);

  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "(A * D * F) + (B * D * F) + (C * D * F) + (A * E) + (B * E) + (C * E)\n");
}

BOOST_AUTO_TEST_CASE(imaginary_numbers) {
  Expression<GenericInfo> exp = {{{Operator<GenericInfo>(ImaginaryNumber(1.0))}}};
  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "1.000000 * i\n");

  exp = im_number<GenericInfo>(3.0) * number<GenericInfo>(2.0) *
              Operator<GenericInfo>(ordering_value(0), GenericInfo("A"));
  std::stringstream ss2;
  exp.print(ss2);
  BOOST_CHECK_EQUAL(ss2.str(), "3.000000 * i * 2.000000 * A\n");
}

BOOST_AUTO_TEST_CASE(named_numbers) {
  Expression<GenericInfo> exp = {{{Operator<GenericInfo>(NamedNumber('t'))}}};
  BOOST_CHECK_EQUAL(exp.print(true), "t\n");
  exp = Operator<GenericInfo>(NamedNumber('t'))
        * number<GenericInfo>(2)
        * Operator<GenericInfo>(ordering_value(0), GenericInfo("A"));
  BOOST_CHECK_EQUAL(exp.print(false), "t * 2.000000 * A");
      
}

BOOST_AUTO_TEST_CASE(SumPrint) {
  Expression<GenericInfo> exp;
  exp = Operator<GenericInfo>(NamedNumber('t'))
        * number<GenericInfo>(2)
        * Operator<GenericInfo>(ordering_value(0), GenericInfo("A"));
  exp = exp + number<GenericInfo>(10);
  Product<GenericInfo> prod(exp);
  Sum<GenericInfo> sum(std::move(exp));
  BOOST_TEST_MESSAGE(sum.print());
  BOOST_TEST_MESSAGE(prod.print());
  BOOST_CHECK_EQUAL(sum.print(), "\u03A3[(t * 2.000000 * A) + (10.000000)]");
  BOOST_CHECK_EQUAL(prod.print(), "\u03A0[(t * 2.000000 * A) + (10.000000)]");
}
