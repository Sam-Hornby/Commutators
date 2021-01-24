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
const Operator<GenericInfo> c3(ordering_value(0), GenericInfo("c3"));
const Operator<GenericInfo> c4(ordering_value(0), GenericInfo("c4"));
const Operator<GenericInfo> c3_h(ordering_value(0), GenericInfo("c3!"));
const Operator<GenericInfo> c4_h(ordering_value(0), GenericInfo("c4!"));

const auto E = named_number<GenericInfo>('E');
const auto Y = named_number<GenericInfo>('Y');

const auto E_ = NamedNumberExpr::create(NamedNumber('E'));
const auto Y_ = NamedNumberExpr::create(NamedNumber('Y'));

const auto boson_e =
  SquareRootExpr::create(
    SubExpr::create(SquareExpr::create(E_),
                    SquareExpr::create(Y_))
);

const auto boson_K =
  SubExpr::create(boson_e, E_);

const auto fermion_e =
  SquareRootExpr::create(
    AddExpr::create(SquareExpr::create(E_),
                    SquareExpr::create(Y_))
);

const auto fermion_K =
  SubExpr::create(E_, fermion_e);


const auto transformed_f_string = fmt::format("({0} * b1! * b1) + "
                                              "({0} * b2! * b2) + "
                                              "({1})",
                                              fermion_e.name(),
                                              fermion_K.name());

const auto transformed_b_string = fmt::format("({0} * b1! * b1) + "
                                              "({0} * b2! * b2) + "
                                              "({1})",
                                              boson_e.name(),
                                              boson_K.name());

const auto transformed_f_string2 = fmt::format("({0} * b3! * b3) + "
                                              "({0} * b4! * b4) + "
                                              "(2.000000 * {1}) + "
                                              "({0} * b1! * b1) + "
                                              "({0} * b2! * b2)",
                                              fermion_e.name(),
                                              fermion_K.name());

const auto transformed_b_string2 = fmt::format("({0} * b3! * b3) + "
                                              "({0} * b4! * b4) + "
                                              "(2.000000 * {1}) + "
                                              "({0} * b1! * b1) + "
                                              "({0} * b2! * b2)",
                                              boson_e.name(),
                                              boson_K.name());


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

  BOOST_CHECK_EQUAL(f_out.name(), transformed_f_string);
  BOOST_CHECK_EQUAL(b_out.name(), transformed_b_string);
}

BOOST_AUTO_TEST_CASE(SuccessfulyTransform_2) {
  // E(c2!c2 + c1!c1) + Y(c1!c2! + c2c1) --> e(b1!b1 + b2!b2) + K
  const auto input = (E * ((c2_h * c2) + (c1_h * c1)))
                   + (Y * ((c1_h * c2_h) + (c2 * c1)));

  const auto f_out =
      bogoliubov_transform<GenericInfo, FermionTestTransform>(input);

  const auto b_out =
      bogoliubov_transform<GenericInfo, BosonTestTransform>(input);

  BOOST_CHECK_EQUAL(f_out.name(), transformed_f_string);
  BOOST_CHECK_EQUAL(b_out.name(), transformed_b_string);
}

BOOST_AUTO_TEST_CASE(SuccessfulyTransform_3) {
  // E(c2!c2 + c1!c1) + Y(c1!c2! + c2c1) --> e(b1!b1 + b2!b2) + K
  const auto input = (E * ((c2_h * c2) + (c1_h * c1)))
                   + (Y * ((c2 * c1) + (c1_h * c2_h)));

  const auto f_out =
      bogoliubov_transform<GenericInfo, FermionTestTransform>(input);

  const auto b_out =
      bogoliubov_transform<GenericInfo, BosonTestTransform>(input);

  BOOST_CHECK_EQUAL(f_out.name(), transformed_f_string);
  BOOST_CHECK_EQUAL(b_out.name(), transformed_b_string);
}

BOOST_AUTO_TEST_CASE(SuccessfulyTransform_4) {
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

BOOST_AUTO_TEST_CASE(SuccessfulyTransform_5) {
  // E(c1!c1 + c2!c2) + Y(c1!c2! + c2c1) --> e(b1!b1 + b2!b2) + K
  const auto input = (E * ((c1_h * c1) + (c2_h * c2)))
                   + (Y * ((c1_h * c2_h) + (c2 * c1)))
                   + (E * ((c3_h * c3) + (c4_h * c4)))
                   + (Y * ((c3_h * c4_h) + (c4 * c3)));

  const auto f_out =
      bogoliubov_transform<GenericInfo, FermionTestTransform>(input);

  const auto b_out =
      bogoliubov_transform<GenericInfo, BosonTestTransform>(input);

  f_out.print();
  b_out.print();

  BOOST_CHECK_EQUAL(f_out.name(),
                    fmt::format("{}",
                                transformed_f_string2));
  BOOST_CHECK_EQUAL(b_out.name(),
                    fmt::format("{}",
                                transformed_b_string2));
}

BOOST_AUTO_TEST_CASE(SuccessfulyTransform_6) {
  // 4(c1!c1 + c2!c2) + 3(c1!c2! + c2c1) --> e(b1!b1 + b2!b2) + K
  const auto input = (Operator<GenericInfo>(4.0) * ((c1_h * c1) + (c2_h * c2)))
                   + (Operator<GenericInfo>(3.0) * ((c1_h * c2_h) + (c2 * c1)));

  const auto f_out =
      bogoliubov_transform<GenericInfo, FermionTestTransform>(input);

  const auto b_out =
      bogoliubov_transform<GenericInfo, BosonTestTransform>(input);

  double ef_const = std::sqrt((4.0*4.0) + (3.0*3.0));
  double eb_const = std::sqrt((4.0*4.0) - (3.0 * 3.0));
  double kf_const = 4.0 - ef_const;
  double kb_const = eb_const - 4.0;

  BOOST_CHECK_EQUAL(f_out.name(),
                    fmt::format("({0} * b1! * b1) + ({0} * b2! * b2) + ({1})",
                            std::to_string(ef_const),
                            std::to_string(kf_const)));
  BOOST_CHECK_EQUAL(b_out.name(),
                    fmt::format("({0} * b1! * b1) + ({0} * b2! * b2) + ({1})",
                                 std::to_string(eb_const),
                                 std::to_string(kb_const)));
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

BOOST_AUTO_TEST_CASE(InvalidTransform_8) {
  // no terms:
  Expression<GenericInfo> input;
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
  // c3 * (E(c1!c1 + c2!c2) + Y(c1!c2 + c2c1))
  const auto input3 = c3 * ((E * ((c1_h * c1) + (c2_h * c2)))
                    + (Y * ((c1_h * c2_h) + (c2 * c1))));
  const auto out3 = bogoliubov_transform<GenericInfo, FermionTestTransform>(input3);
  BOOST_CHECK_EQUAL(out3.name(), input3.name());

  // c3 + (E(c1!c1 + c2!c2) + Y(c1!c2 + c2c1))
  const auto input4 = c3 + ((E * ((c1_h * c1) + (c2_h * c2)))
                    + (Y * ((c1_h * c2_h) + (c2 * c1))));
  const auto out4 = bogoliubov_transform<GenericInfo, BosonTestTransform>(input4);
  BOOST_CHECK_EQUAL(out4.name(), fmt::format("(c3) + {}", transformed_b_string));
  
  // Y + (E(c1!c1 + c2!c2) + Y(c1!c2 + c2c1))
  const auto input5 = Y + ((E * ((c1_h * c1) + (c2_h * c2)))
                    + (Y * ((c1_h * c2_h) + (c2 * c1))));
  const auto out5 = bogoliubov_transform<GenericInfo, BosonTestTransform>(input5);
  BOOST_CHECK_EQUAL(out5.name(), fmt::format("(Y) + {}", transformed_b_string));

}
