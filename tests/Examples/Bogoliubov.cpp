#include "transforms/BogoliubovTransform.hpp"
#include <Expression/struct_ids.hpp>
#include <Comutators/Comutators.hpp>
#include <Utils/Utils.hpp>
#define BOOST_TEST_MODULE Bogoliubov
#include <boost/test/included/unit_test.hpp>

using namespace operators;

BOOST_AUTO_TEST_CASE(simple_2_case_bosons) {
  /*auto H = (named_number<Fock1DInfo>('e') *
           ((creation_op(1) * anihilation_op(1)) + (creation_op(-1) * anihilation_op(-1))))
           + (named_number<Fock1DInfo>('y') *
           ((creation_op(1) * creation_op(-1)) + (anihilation_op(-1) * anihilation_op(-1))));
  BOOST_TEST_MESSAGE(H.print(true));

  auto BH = bogoliubov_transform(H, OperatorType::BOSON);

  BOOST_TEST_MESSAGE(BH.print(false));
  */
  // After bogoliubov transforms answer should be
  // E = root(e**2 - y**2)
  // H = E(b_1! * b_1 + b_-1! * b_-1) - e + E
  //BOOST_CHECK_EQUAL(H.print(false),
  //                  "");
}

BOOST_AUTO_TEST_CASE(simple_2_case_fermions) {
  /*auto H = (named_number<Fock1DInfo>('e') *
           ((creation_op(1) * anihilation_op(1)) + (creation_op(-1) * anihilation_op(-1))))
           + (named_number<Fock1DInfo>('y') *
           ((creation_op(1) * creation_op(-1)) + (anihilation_op(-1) * anihilation_op(-1))));
  BOOST_TEST_MESSAGE(H.print(true));

  auto BH = bogoliubov_transform(H, OperatorType::FERMION);

  BOOST_TEST_MESSAGE(BH.print(false));
  */
  // After bogoliubov transforms answer should be
  // E = root(e**2 + y**2)
  // H = E(b_1! * b_1 + b_-1! * b_-1) + e - E
  //BOOST_CHECK_EQUAL(H.print(false),
  //                  "");
}

//BOOST_AUTO_TEST_CASE(Weakly_interacting_bose_gas) {}
