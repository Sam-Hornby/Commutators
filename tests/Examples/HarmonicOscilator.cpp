#include <Expression/Expression.hpp>
#include <Expression/Evaluate.hpp>
#include <Substitutions/Substitutions.hpp>
#include <Expression/Ordering.hpp>
#include <Utils/Utils.hpp>
#include <Expression/Operator.hpp>
#include <Comutators/BosonComutators.hpp>
#include <Expression/Infos/Fock0D.hpp>
#include <exception>
#include <cmath>
#include <Numbers/CompositeNumberExpressions.hpp>
#define BOOST_TEST_MODULE HARMONICOSCILATOR
#include <boost/test/included/unit_test.hpp>

using namespace operators;


Operator<Fock0DInfo> creation_op() {
  return Operator<Fock0DInfo>(ordering_value(0),
                  Fock0DInfo(Type::CREATION_OPERATOR));
}
Operator<Fock0DInfo> anihilation_op() {
  return Operator<Fock0DInfo>(ordering_value(0),
                  Fock0DInfo(Type::ANIHILATION_OPERATOR));
}
//Expression<Fock0DInfo> normalised_n_occupied_state(unsigned n) {
//  Expression<Fock0DInfo> exp;
//  exp.expression.push_back({vacuum_state<Fock0DInfo>()});
//  double normalisation_factor = 1.0 / std::sqrt(tgamma(static_cast<double>(n + 1)));
//  for (unsigned i = 0; i < n; ++i) {
//    exp = creation_op() * exp;
//  }
//  return number<Fock0DInfo>(normalisation_factor) * exp;
//}

template <class Info, class... Args>
Expression<Info> normalised_n_occupied_state(unsigned n, Args... args) {
  return normalised_n_occupied_ops<Info>(n, args...) * vacuum_state<Fock0DInfo>();
}

template <class OperatorInfo>
bool substitutions(typename vector_type<Operator<OperatorInfo>>::iterator start,
                   vector_type<Operator<OperatorInfo>> & exp) {
  return anihilate_vacuum<OperatorInfo>(start, exp) or all_states_orthognal<OperatorInfo>(start, exp);
}

static double round_to_3_dp(double x) {
  return round(x * 1000) / 1000;
}

template <class OperatorInfo>
static bool check_answer(const Expression<OperatorInfo> & exp, const ComplexNumber ans) {
  if (exp.expression.size() != 1) {
    return false;
  }
  if (exp.expression[0].size() != 1) {
    return false;
  }
  if (!exp.expression[0][0].is_number()) {
    return false;
  }
  BOOST_TEST_MESSAGE("real parts " << exp.expression[0][0].value().value.real() << " and "
		     << ans.value.real());
  return round_to_3_dp(exp.expression[0][0].value().value.real()) == round_to_3_dp(ans.value.real()) and
         round_to_3_dp(exp.expression[0][0].value().value.imag()) == round_to_3_dp(ans.value.imag());
}

BOOST_AUTO_TEST_CASE(HarmonicOscilator) {
  auto hamiltonian = (creation_op() * anihilation_op()) + number<Fock0DInfo>(0.5);
  BOOST_TEST_MESSAGE(hamiltonian.print(false));
  // vacuum state expectation value
  Expression<Fock0DInfo> zero_occupied_state = normalised_n_occupied_state<Fock0DInfo>(0);
  auto vacuum_expectation = hermition_conjugate<Fock0DInfo>(zero_occupied_state) * hamiltonian * zero_occupied_state;
  vacuum_expectation = normal_order<Fock0DInfo>(vacuum_expectation);
  vacuum_expectation = evaluate<Fock0DInfo>(vacuum_expectation, boson_commutator<Fock0DInfo>, substitutions<Fock0DInfo>);
  BOOST_TEST_MESSAGE(vacuum_expectation.print(false));
  BOOST_CHECK(check_answer(vacuum_expectation, 0.5));

  Expression<Fock0DInfo> single_occupied_state = normalised_n_occupied_state<Fock0DInfo>(1);
  auto single_expectation = hermition_conjugate<Fock0DInfo>(single_occupied_state) * hamiltonian * single_occupied_state;
  single_expectation = normal_order<Fock0DInfo>(single_expectation);
  single_expectation = evaluate<Fock0DInfo>(single_expectation, boson_commutator<Fock0DInfo>, substitutions<Fock0DInfo>);
  BOOST_TEST_MESSAGE(single_expectation.print(false));
  BOOST_CHECK(check_answer(single_expectation, 1.5));

  Expression<Fock0DInfo> double_occupied_state = normalised_n_occupied_state<Fock0DInfo>(2);
  auto double_expectation = hermition_conjugate<Fock0DInfo>(double_occupied_state) * hamiltonian * double_occupied_state;
  double_expectation = normal_order<Fock0DInfo>(double_expectation);
  double_expectation = evaluate<Fock0DInfo>(double_expectation, boson_commutator<Fock0DInfo>, substitutions<Fock0DInfo>);
  BOOST_TEST_MESSAGE(double_expectation.print(false));
  BOOST_CHECK(check_answer(double_expectation, 2.5));

  Expression<Fock0DInfo> triple_occupied_state = normalised_n_occupied_state<Fock0DInfo>(3);
  auto triple_expectation = hermition_conjugate<Fock0DInfo>(triple_occupied_state) * hamiltonian * triple_occupied_state;
  triple_expectation = normal_order<Fock0DInfo>(triple_expectation);
  triple_expectation = evaluate<Fock0DInfo>(triple_expectation, boson_commutator<Fock0DInfo>, substitutions<Fock0DInfo>);
  BOOST_TEST_MESSAGE(triple_expectation.print(false));
  BOOST_CHECK(check_answer(triple_expectation, 3.5));

  Expression<Fock0DInfo> quad_occupied_state = normalised_n_occupied_state<Fock0DInfo>(4);
  auto quad_expectation = hermition_conjugate<Fock0DInfo>(quad_occupied_state) * hamiltonian * quad_occupied_state;
  quad_expectation = normal_order<Fock0DInfo>(quad_expectation);
  quad_expectation = evaluate<Fock0DInfo>(quad_expectation, boson_commutator<Fock0DInfo>, substitutions<Fock0DInfo>);
  BOOST_TEST_MESSAGE(quad_expectation.print(false));
  BOOST_CHECK(check_answer(quad_expectation, 4.5));
}

BOOST_AUTO_TEST_CASE(HarmonicOscilator_2D) {
  // hamiltonian = w0 * (a0! * a0 + 0.5) + w1 * (a1! * a1 + 0.5)
  // states are |x, y> represents particles in state x wrt to first
  // 1d oscilator and y wrt to second
  // Use the x coordinate in the operator info to tell which dimension the operators belong. As only having vacuum state
  // all vaccum states will have zero as they belong to both
  auto hamiltonian = (number<Fock1DInfo>(7.0) * ((creation_op(0) * anihilation_op(0)) + number<Fock1DInfo>(0.5)))
                   + (number<Fock1DInfo>(5.0) * ((creation_op(1) * anihilation_op(1)) + number<Fock1DInfo>(0.5)));
  BOOST_TEST_MESSAGE(hamiltonian.print(false));
  // vacuum state expectation value
  Expression<Fock1DInfo> zero_occupied_state = normalised_n_occupied_state<Fock1DInfo>(0, 0);
  auto vacuum_expectation = hermition_conjugate<Fock1DInfo>(zero_occupied_state) * hamiltonian * zero_occupied_state;
  vacuum_expectation = normal_order<Fock1DInfo>(vacuum_expectation);
  vacuum_expectation = evaluate<Fock1DInfo>(vacuum_expectation, boson_commutator<Fock1DInfo>, substitutions<Fock1DInfo>);
  BOOST_TEST_MESSAGE(vacuum_expectation.print(false));
  BOOST_CHECK(check_answer(vacuum_expectation, 6.0));

  // single occupied state in first HO
  Expression<Fock1DInfo> single_occupied_state = normalised_n_occupied_state<Fock1DInfo>(1, 0);
  auto expectation = hermition_conjugate<Fock1DInfo>(single_occupied_state) * hamiltonian * single_occupied_state;
  expectation = normal_order<Fock1DInfo>(expectation);
  expectation = evaluate<Fock1DInfo>(expectation, boson_commutator<Fock1DInfo>, substitutions<Fock1DInfo>);
  BOOST_TEST_MESSAGE(expectation.print(false));
  BOOST_CHECK(check_answer(expectation, 13.0));

  // single occupied state in second HO
  single_occupied_state = normalised_n_occupied_state<Fock1DInfo>(1, 1);
  expectation = hermition_conjugate<Fock1DInfo>(single_occupied_state) * hamiltonian * single_occupied_state;
  expectation = normal_order<Fock1DInfo>(expectation);
  expectation = evaluate<Fock1DInfo>(expectation, boson_commutator<Fock1DInfo>, substitutions<Fock1DInfo>);
  BOOST_TEST_MESSAGE(expectation.print(false));
  BOOST_CHECK(check_answer(expectation, 11.0));

  // both H0s occupied
  single_occupied_state = normalised_n_occupied_ops<Fock1DInfo>(5, 0) *
                          normalised_n_occupied_ops<Fock1DInfo>(6, 1) *
                          vacuum_state<Fock1DInfo>();
  expectation = hermition_conjugate<Fock1DInfo>(single_occupied_state) * hamiltonian * single_occupied_state;
  expectation = normal_order<Fock1DInfo>(expectation);
  expectation = evaluate<Fock1DInfo>(expectation, boson_commutator<Fock1DInfo>, substitutions<Fock1DInfo>);
  BOOST_TEST_MESSAGE(expectation.print(false));
  BOOST_CHECK(check_answer(expectation, 71.0));

}


BOOST_AUTO_TEST_CASE(HarmonicOscilator_2D_named) {
  // hamiltonian = w0 * (a0! * a0 + 0.5) + w1 * (a1! * a1 + 0.5)
  // states are |x, y> represents particles in state x wrt to first
  // 1d oscilator and y wrt to second
  // Use the x coordinate in the operator info to tell which dimension the operators belong. As only having vacuum state
  // all vaccum states will have zero as they belong to both
  auto hamiltonian = (named_number<Fock1DInfo>('w') * ((creation_op(0) * anihilation_op(0)) + number<Fock1DInfo>(0.5)))
                   + (named_number<Fock1DInfo>('u') * ((creation_op(1) * anihilation_op(1)) + number<Fock1DInfo>(0.5)));
  BOOST_TEST_MESSAGE(hamiltonian.print(false));

  // both H0s occupied
  auto state = normalised_n_occupied_ops<Fock1DInfo>(3, 0) *
               normalised_n_occupied_ops<Fock1DInfo>(4, 1) *
               vacuum_state<Fock1DInfo>();
  auto expectation = hermition_conjugate<Fock1DInfo>(state) * hamiltonian * state;
  expectation = normal_order<Fock1DInfo>(expectation);
  expectation = evaluate<Fock1DInfo>(expectation, boson_commutator<Fock1DInfo>, substitutions<Fock1DInfo>);
  BOOST_CHECK_EQUAL(expectation.print(false), "(3.500000 * w) + (4.500000 * u)");
}


BOOST_AUTO_TEST_CASE(HarmonicOscilator_2D_composite) {
  // hamiltonian = w0 * (a0! * a0 + 0.5) + w1 * (a1! * a1 + 0.5)
  // states are |x, y> represents particles in state x wrt to first
  // 1d oscilator and y wrt to second
  // Use the x coordinate in the operator info to tell which dimension the operators belong. As only having vacuum state
  // all vaccum states will have zero as they belong to both
  auto h_ = NamedNumberExpr::create(NamedNumber('h'));
  auto w_ = NamedNumberExpr::create(NamedNumber('w'));
  auto u_ = NamedNumberExpr::create(NamedNumber('u'));
  
  auto hamiltonian = (Operator<Fock1DInfo>(h_)
                             * ((creation_op(0) * anihilation_op(0)) + number<Fock1DInfo>(0.5))
                             * Operator<Fock1DInfo>(w_))
                   + (Operator<Fock1DInfo>(h_)
                             * ((creation_op(1) * anihilation_op(1)) + number<Fock1DInfo>(0.5))
                             * Operator<Fock1DInfo>(u_));
  BOOST_TEST_MESSAGE(hamiltonian.print(false));

  // both H0s occupied
  auto state = normalised_n_occupied_ops<Fock1DInfo>(3, 0) *
               normalised_n_occupied_ops<Fock1DInfo>(4, 1) *
               vacuum_state<Fock1DInfo>();
  auto expectation = hermition_conjugate<Fock1DInfo>(state) * hamiltonian * state;
  expectation = normal_order<Fock1DInfo>(expectation);
  expectation = evaluate<Fock1DInfo>(expectation, boson_commutator<Fock1DInfo>, substitutions<Fock1DInfo>);
  BOOST_CHECK_EQUAL(expectation.print(false), "(3.500000 * h * w) + (4.500000 * h * u)");
}
