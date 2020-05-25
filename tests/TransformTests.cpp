#include "transforms/Transform.hpp"
#include <Expression/struct_ids.hpp>
//#include <Comutators/Comutators.hpp>
#include <Expression/Infos/GenericInfo.hpp>
#define BOOST_TEST_MODULE Transforms
#include <boost/test/included/unit_test.hpp>


using namespace operators;

Expression<GenericInfo> test_transform(const Operator<GenericInfo> & A) {
  if (A.is_number()) {
    return {{{A}}};
  }
  if (A.info().op_name == "a") {
    return Operator<GenericInfo>(ordering_value(0), GenericInfo("A")) +
           Operator<GenericInfo>(ordering_value(0), GenericInfo("B"));
  }
  if (A.info().op_name == "c") {
    return Operator<GenericInfo>(ordering_value(0), GenericInfo("C")) +
           Operator<GenericInfo>(ordering_value(0), GenericInfo("D"));
  }
  if (A.info().op_name == "e") {
    return {{{Operator<GenericInfo>(ordering_value(0), GenericInfo("E"))}}};
  }
  if (A.info().op_name == "f") {
    return Operator<GenericInfo>(ordering_value(0), GenericInfo("F")) *
           Operator<GenericInfo>(ordering_value(0), GenericInfo("G"));
  }
  if (A.info().op_name == "h") {
    return (Operator<GenericInfo>(ordering_value(0), GenericInfo("H")) *
           Operator<GenericInfo>(ordering_value(0), GenericInfo("I"))) +
           Operator<GenericInfo>(ordering_value(0), GenericInfo("J"));
  }
  return {{{A}}};
}


BOOST_AUTO_TEST_CASE(empty) {
  Expression<GenericInfo> exp;
  exp = transform_expression<GenericInfo, GenericInfo>(exp, test_transform);
  BOOST_CHECK_EQUAL(exp.expression.size(), 0);
}


BOOST_AUTO_TEST_CASE(onetoone) {
  Operator<GenericInfo> e = Operator<GenericInfo>(0, GenericInfo("e"));
  Operator<GenericInfo> other = Operator<GenericInfo>(0, GenericInfo("other"));
  auto new_exp = transform_expression<GenericInfo, GenericInfo>(e * other, test_transform);
  BOOST_CHECK_EQUAL(new_exp.print(true), "(E * other)\n");
  new_exp = transform_expression<GenericInfo, GenericInfo>(other * e, test_transform);
  BOOST_CHECK_EQUAL(new_exp.print(true), "(other * E)\n");
  new_exp = transform_expression<GenericInfo, GenericInfo>(e + other, test_transform);
  BOOST_CHECK_EQUAL(new_exp.print(true), "(E) + (other)\n");
  new_exp = transform_expression<GenericInfo, GenericInfo>(other + e, test_transform);
  BOOST_CHECK_EQUAL(new_exp.print(true), "(other) + (E)\n");
}

BOOST_AUTO_TEST_CASE(multiple_terms) {
  Operator<GenericInfo> a = Operator<GenericInfo>(0, GenericInfo("a"));
  Operator<GenericInfo> c = Operator<GenericInfo>(0, GenericInfo("c"));
  Operator<GenericInfo> e = Operator<GenericInfo>(0, GenericInfo("e"));
  Operator<GenericInfo> f = Operator<GenericInfo>(0, GenericInfo("f"));
  Operator<GenericInfo> h = Operator<GenericInfo>(0, GenericInfo("h"));
  auto new_exp = transform_expression<GenericInfo, GenericInfo>(e * a, test_transform);
  BOOST_CHECK_EQUAL(new_exp.print(true), "(E * A) + (E * B)\n");
  new_exp = transform_expression<GenericInfo, GenericInfo>(a * c, test_transform);
  BOOST_CHECK_EQUAL(new_exp.print(true), "(A * C) + (B * C) + (A * D) + (B * D)\n");
  new_exp = transform_expression<GenericInfo, GenericInfo>(f * a, test_transform);
  BOOST_CHECK_EQUAL(new_exp.print(true), "(F * G * A) + (F * G * B)\n");
  new_exp = transform_expression<GenericInfo, GenericInfo>(a * f, test_transform);
  BOOST_CHECK_EQUAL(new_exp.print(true), "(A * F * G) + (B * F * G)\n");
  new_exp = transform_expression<GenericInfo, GenericInfo>(a + f, test_transform);
  BOOST_CHECK_EQUAL(new_exp.print(true), "(A) + (B) + (F * G)\n");

  new_exp = transform_expression<GenericInfo, GenericInfo>(h * a + (h * f), test_transform);
  BOOST_CHECK_EQUAL(new_exp.print(true), "(H * I * A) + (J * A) + "
                                 "(H * I * B) + (J * B) + "
                                 "(H * I * F * G) + (J * F * G)\n");
}
