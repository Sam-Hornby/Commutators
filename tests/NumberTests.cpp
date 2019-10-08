#include "Expression.hpp"
#include "Operator.hpp"
#include "Comutators.hpp"
#include <sstream>
#define BOOST_TEST_MODULE Number
#include <boost/test/included/unit_test.hpp>

using namespace operators;


BOOST_AUTO_TEST_CASE(empty) {
  Expression<Fock1DInfo> exp;
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "\n");
}

BOOST_AUTO_TEST_CASE(single) {
  Expression<Fock1DInfo> exp;
  exp = exp + Operator<Fock1DInfo>(5);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "(5.000000)\n");
}

BOOST_AUTO_TEST_CASE(two_numbers) {
  Expression<Fock1DInfo> exp = Operator<Fock1DInfo>(5) * Operator<Fock1DInfo>(7);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "(35.000000)\n");
}


BOOST_AUTO_TEST_CASE(operators_and_numbers) {
  Expression<GenericInfo> exp = Operator<GenericInfo>(ordering_value(0), GenericInfo("A")) * Operator<GenericInfo>(4) *
                    Operator<GenericInfo>(ordering_value(0), GenericInfo("B")) * Operator<GenericInfo>(9);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "(36.000000 * A * B)\n");
}

BOOST_AUTO_TEST_CASE(operator_and_1) {
  auto exp = Operator<GenericInfo>(ordering_value(0), GenericInfo("A")) * Operator<GenericInfo>(1)
             * Operator<GenericInfo>(ordering_value(0), GenericInfo("B"));
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "(A * B)\n");
}

BOOST_AUTO_TEST_CASE(single_one) {
  Expression<Fock1DInfo> exp;
  exp = exp + Operator<Fock1DInfo>(1);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "(1.000000)\n");
}

BOOST_AUTO_TEST_CASE(zero) {
  Expression<GenericInfo> exp = Operator<GenericInfo>(ordering_value(0), GenericInfo("A")) * Operator<GenericInfo>(0) *
                    Operator<GenericInfo>(ordering_value(0), GenericInfo("B")) * Operator<GenericInfo>(9);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "\n");
}

BOOST_AUTO_TEST_CASE(addition_and_multiply) {
  Expression<GenericInfo> exp;
  const auto A = Operator<GenericInfo>(ordering_value(0), GenericInfo("A"));
  exp = exp + A + (Operator<GenericInfo>(0) * A) + (A * Operator<GenericInfo>(1) * A) + (A * A * A);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "(A) + (A * A) + (A * A * A)\n");
}

BOOST_AUTO_TEST_CASE(addition) {
  Expression<Fock1DInfo> exp = Operator<Fock1DInfo>(6) + Operator<Fock1DInfo>(7);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "(13.000000)\n");
}

BOOST_AUTO_TEST_CASE(opperator_addition) {
  Operator<GenericInfo> A(ordering_value(0), GenericInfo("A"));
  Expression<GenericInfo> exp = A + A;
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "(2.000000 * A)\n");
}

BOOST_AUTO_TEST_CASE(opperator_addition_multiply) {
  Operator<GenericInfo> A(ordering_value(0), GenericInfo("A"));
  Expression<GenericInfo> exp = (A * Operator<GenericInfo>(2)) + A;
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "(3.000000 * A)\n");
}

BOOST_AUTO_TEST_CASE(opperator_addition_multiply_2) {
  Operator<GenericInfo> A(ordering_value(0), GenericInfo("A"));
  Operator<GenericInfo> B(ordering_value(0), GenericInfo("B"));
  Expression<GenericInfo> exp = (A * B * Operator<GenericInfo>(2)) + (Operator<GenericInfo>(6) * A * B);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "(8.000000 * A * B)\n");
}

BOOST_AUTO_TEST_CASE(opperator_addition_multiply_3) {
  Operator<GenericInfo> A(ordering_value(0), GenericInfo("A"));
  Operator<GenericInfo> B(ordering_value(0), GenericInfo("B"));
  Expression<GenericInfo> exp = (A * B * Operator<GenericInfo>(2)) + (Operator<GenericInfo>(6) * B * A);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "(2.000000 * A * B) + (6.000000 * B * A)\n");
}

BOOST_AUTO_TEST_CASE(opperator_addition_multiply_4) {
  Operator<GenericInfo> A(ordering_value(0), GenericInfo("A"));
  Operator<GenericInfo> B(ordering_value(0), GenericInfo("B"));
  Expression<GenericInfo> exp = (A * B * Operator<GenericInfo>(2))
                    + (A * B * Operator<GenericInfo>(0))
                    + (B * A * Operator<GenericInfo>(1))
                    + (Operator<GenericInfo>(6) * A * B);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "(8.000000 * A * B) + (B * A)\n");
}

BOOST_AUTO_TEST_CASE(ones) {
  Operator<GenericInfo> A(ordering_value(0), GenericInfo("A"));
  Expression<GenericInfo> exp = A + number<GenericInfo>(1.0);
  exp = exp.simplify_numbers();
  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "(A) + (1.000000)\n");
}

BOOST_AUTO_TEST_CASE(imaginary_numbers) {
  Operator<GenericInfo> A(ImaginaryNumber(2.0) * 4.0);
  Expression<GenericInfo> exp = {{{A}}};
  std::stringstream ss;
  exp.print(ss);
  BOOST_CHECK_EQUAL(ss.str(), "(8.000000 * i)\n");

  auto B = ImaginaryNumber(2.0) * ImaginaryNumber(5.0);
  BOOST_CHECK_EQUAL(B, -10.0);
  auto C = ImaginaryNumber(-1) * ImaginaryNumber(3);
  BOOST_CHECK_EQUAL(C, 3);

  ComplexNumber D = ComplexNumber(2) * ImaginaryNumber(4.0);
  BOOST_CHECK_EQUAL(D.real_part, 0);
  BOOST_CHECK_EQUAL(D.imaginary_part, 8.0);
  BOOST_CHECK_EQUAL(D.name(), "8.000000 * i");

  D = ImaginaryNumber(5) * ComplexNumber(0, 1);
  BOOST_CHECK_EQUAL(D.real_part, -5.0);
  BOOST_CHECK_EQUAL(D.imaginary_part, 0);
  BOOST_CHECK_EQUAL(D.name(), "-5.000000");

  D = ComplexNumber(1, 1) * ComplexNumber(2, 3);
  BOOST_CHECK_EQUAL(D.real_part, -1.0);
  BOOST_CHECK_EQUAL(D.imaginary_part, 5.0);
  BOOST_CHECK_EQUAL(D.name(), "(-1.000000 + 5.000000i)");

  D = ComplexNumber(3, 4) + ComplexNumber(7, 8);
  BOOST_CHECK_EQUAL(D.real_part, 10.0);
  BOOST_CHECK_EQUAL(D.imaginary_part, 12.0);
  BOOST_CHECK_EQUAL(D.name(), "(10.000000 + 12.000000i)");

}
