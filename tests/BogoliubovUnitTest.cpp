#include <transforms/BogoliubovTransform.hpp>
#include <transforms/BogoliubovTransformFunctions.hpp>
#include <Utils/Utils.hpp>
#include <Expression/Infos/GenericInfo.hpp>
#define BOOST_TEST_MODULE BogoliubovUnitTests
#include <boost/test/included/unit_test.hpp>

using namespace operators;

const Operator<GenericInfo> c1(ordering_value(0), GenericInfo("c1"));
const Operator<GenericInfo> c2(ordering_value(0), GenericInfo("c2"));
const Operator<GenericInfo> c1_h(ordering_value(0), GenericInfo("c1!"));
const Operator<GenericInfo> c2_h(ordering_value(0), GenericInfo("c2!"));
const auto E = named_number<GenericInfo>('E');
const auto Y = named_number<GenericInfo>('Y');

BOOST_AUTO_TEST_CASE(SuccessfulyTransform_1) {
  // E(c1!c1 + c2!c2) + Y(c1!c2! + c2c1) --> e(b1!b1 + b2!b2) + K
  const auto input = (E * ((c1_h * c1) + (c2_h * c2)))
                   + (Y * ((c1_h * c2_h) + (c2 * c1)));

  const auto f_out =
      bogoliubov_transform<GenericInfo, FermionTestTransform>(input);

  const auto b_out =
      bogoliubov_transform<GenericInfo, BosonTestTransform>(input);

  f_out.print();
  b_out.print();
}

BOOST_AUTO_TEST_CASE(SuccessfulyTransform_2) {
  // need to work out effect of switching 1 and 2 (might only be in
  // evaluating b1 and b2)
  // E(c1!c1 + c2!c2) + Y(c2!c1! + c1c2) --> e(b1!b1 + b2!b2) + K 
  const auto input = (E * ((c1_h * c1) + (c2_h * c2)))
                   + (Y * ((c2_h * c1_h) + (c1 * c2)));

  const auto f_out =
      bogoliubov_transform<GenericInfo, FermionTestTransform>(input);

  const auto b_out =
      bogoliubov_transform<GenericInfo, BosonTestTransform>(input);
}

BOOST_AUTO_TEST_CASE(SimpleInvalidTransforms) {
  // Empty: {} --> {}
  const Expression<GenericInfo> input1;
  const auto out1 = bogoliubov_transform<GenericInfo, FermionTestTransform>(input1);
  BOOST_CHECK(out1.expression.empty());
  // Constant: K --> K
  const Expression<GenericInfo> input2 = {{{E}}};
  const auto out2 = bogoliubov_transform<GenericInfo, FermionTestTransform>(input2);
  BOOST_CHECK_EQUAL(out2.name(), input2.name());

  // Linear: E(c1 + c2) --> E(c1 + c2)
  const auto input3 = E * (c1 + c2);
  const auto out3 = bogoliubov_transform<GenericInfo, FermionTestTransform>(input3);
  BOOST_CHECK_EQUAL(out3.name(), input3.name());
}

BOOST_AUTO_TEST_CASE(InvalidTransform_1) {
  // NonHermitian cross terms: E(c1!c1 + c2!c2) + Y(c1!c2! + c1c2)
  const auto input = (E * ((c1_h * c1) + (c2_h * c2)))
                   + (Y * ((c1_h * c2_h) + (c1 * c2)));
  const auto out = bogoliubov_transform<GenericInfo, FermionTestTransform>(input);
  BOOST_CHECK_EQUAL(out.name(), input.name());
}

BOOST_AUTO_TEST_CASE(InvalidTransform_2) {
  // NonHermitian cross terms: E(c1!c1 + c2!c2) + Y(c2!c1! + c2c1)
  const auto input = (E * ((c1_h * c1) + (c2_h * c2)))
                   + (Y * ((c2_h * c1_h) + (c2 * c1)));
  const auto out = bogoliubov_transform<GenericInfo, FermionTestTransform>(input);
  BOOST_CHECK_EQUAL(out.name(), input.name());
}

BOOST_AUTO_TEST_CASE(InvalidTransform_3) {
  // NonHermitian Diag terms: E(c1!c1! + c2!c2) + Y(c1!c2! + c2c1)
  const auto input = (E * ((c1_h * c1_h) + (c2_h * c2)))
                   + (Y * ((c1_h * c2_h) + (c2 * c1)));
  const auto out = bogoliubov_transform<GenericInfo, FermionTestTransform>(input);
  BOOST_CHECK_EQUAL(out.name(), input.name());
}

BOOST_AUTO_TEST_CASE(InvalidTransform_4) {
  // Extra c2: E(c1!c2 + c2!c2) + Y(c1!c2! + c2c1)
  const auto input = (E * ((c1_h * c2) + (c2_h * c2)))
                   + (Y * ((c1_h * c2_h) + (c2 * c1)));
  const auto out = bogoliubov_transform<GenericInfo, FermionTestTransform>(input);
  BOOST_CHECK_EQUAL(out.name(), input.name());
}

BOOST_AUTO_TEST_CASE(InvalidTransform_5) {
  // no diag terms: E(c1!c2 + c2!c1) + Y(c1!c2! + c2c1)
  const auto input = (E * ((c1_h * c2) + (c2_h * c1)))
                   + (Y * ((c1_h * c2_h) + (c2 * c1)));
  const auto out = bogoliubov_transform<GenericInfo, FermionTestTransform>(input);
  BOOST_CHECK_EQUAL(out.name(), input.name());
}

BOOST_AUTO_TEST_CASE(InvalidTransform_6) {
  // no cross terms E(c1!c1 + c2!c2) + Y
  const auto input = (E * ((c1_h * c1) + (c2_h * c2)))
                   + Y;
  const auto out = bogoliubov_transform<GenericInfo, FermionTestTransform>(input);
  BOOST_CHECK_EQUAL(out.name(), input.name());
}

BOOST_AUTO_TEST_CASE(InvalidTransform_7) {
  // no diag terms: E + Y(c1!c2! + c2c1)
  const auto input = E
                   + (Y * ((c1_h * c2_h) + (c2 * c1)));
  const auto out = bogoliubov_transform<GenericInfo, FermionTestTransform>(input);
  BOOST_CHECK_EQUAL(out.name(), input.name());
}

BOOST_AUTO_TEST_CASE(NonQuadraticTransform) {
  // E(c1!c1!c1c1 + c2!c2!c2c2) + Y(c1!c1!c2!c2! + c2c2c1c1)
  const auto input1 = (E * ((c1_h*c1_h*c1*c1) + (c2_h*c2_h*c2*c2)))
                   + (Y * ((c1_h*c1_h*c2_h*c2_h) + (c2*c2*c1*c1)));
  const auto out1 = bogoliubov_transform<GenericInfo, FermionTestTransform>(input1);
  BOOST_CHECK_EQUAL(out1.name(), input1.name());

  // E(c1!c1c1 + c2!c2c2) + Y(c1!c2 + c2c1)
  const auto input2 = (E * ((c1_h * c1 * c1) + (c2_h * c2 * c2)))
                    + (Y * ((c1_h * c2_h) + (c2 * c1)));
  const auto out2 = bogoliubov_transform<GenericInfo, FermionTestTransform>(input2);
  BOOST_CHECK_EQUAL(out2.name(), input2.name());

  // this one actually can be transformed but will add functionality later
  // c1 * (E(c1!c1 + c2!c2) + Y(c1!c2 + c2c1))
  const auto input3 = c1 * ((E * ((c1_h * c1) + (c2_h * c2)))
                    + (Y * ((c1_h * c2_h) + (c2 * c1))));
  const auto out3 = bogoliubov_transform<GenericInfo, FermionTestTransform>(input3);
  BOOST_CHECK_EQUAL(out3.name(), input3.name());
}
