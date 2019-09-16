#include "gtest/gtest.h"
#include "Expression.hpp"
#include "Substitutions.hpp"
#include "Ordering.hpp"
#include "Utils.hpp"
#include "Operator.hpp"
#include "Comutators.hpp"
#include <exception>
#include <cmath>

using namespace operators;

struct Fock0DInfo {
  // struct where only degree of freedom is energy value and only vacuum state represented
  // other states representedwith fock operators
  Type type = Type::UNSPECIFIED;
  Fock0DInfo() = default;
  Fock0DInfo(Type type) : type(type) {}
  static Fock0DInfo vacuumState() {
    return Fock0DInfo(Type::STATE_VECTOR);
  }
  bool isVacuumState() {
    return type == Type::STATE_VECTOR;
  }
  bool isHCVacuumState() {
    return type == Type::HC_STATE_VECTOR;
  }
  bool match(const Fock1DInfo & other) {
    if (isFockOpType(type) and isFockOpType(other.type)) {
      return true;
    }
    if (isVectorType(type) and isVectorType(other.type)) {
      return true;
    }
    return false;
  }
};

template <class OperatorInfo>
Expression<OperatorInfo> boson_commutator_(const Operator<OperatorInfo> & A, const Operator<OperatorInfo> & B) {
  if (is_anihilation_op<OperatorInfo>(A) and is_creation_op<OperatorInfo>(B) and A.match(B)) {
    return Expression<OperatorInfo>({{number<OperatorInfo>(1)}});
  }
  throw std::logic_error("Shouldn't be commuting anything else");
}

template <class OperatorInfo>
Expression<OperatorInfo> boson_commutator(const Operator<OperatorInfo> & A, const Operator<OperatorInfo> & B) {
  return commute_numbers<OperatorInfo>(A, B, boson_commutator_<OperatorInfo>);
}

template <class OperatorInfo>
bool substitutions(typename std::vector<Operator<OperatorInfo>>::iterator start,
                   std::vector<Operator<OperatorInfo>> & exp) {
  return anihilate_vacuum<OperatorInfo>(start, exp) or all_states_orthognal<OperatorInfo>(start, exp);
}

static double round_to_3_dp(double x) {
  return round(x * 1000) / 1000;
}

template <class OperatorInfo>
static bool check_answer(const Expression<OperatorInfo> & exp, const double ans) {
  if (exp.expression.size() != 1) {
    return false;
  }
  if (exp.expression[0].size() != 1) {
    return false;
  }
  if (!exp.expression[0][0].is_number()) {
    return false;
  }

  return round_to_3_dp(exp.expression[0][0].value.get()) == round_to_3_dp(ans);
}

TEST(Examples, HarmonicOscilator) {
  auto hamiltonian = (creation_op(0) * anihilation_op(0)) + number(0.5);
  hamiltonian.print();
  // vacuum state expectation value
  auto zero_occupied_state = normalised_n_occupied_state(0);
  auto vacuum_expectation = hermition_conjugate(zero_occupied_state) * hamiltonian * zero_occupied_state;
  vacuum_expectation = normal_order(vacuum_expectation);
  vacuum_expectation = vacuum_expectation.evaluate(boson_commutator, substitutions);
  vacuum_expectation.print();
  ASSERT_TRUE(check_answer(vacuum_expectation, 0.5));

  auto single_occupied_state = normalised_n_occupied_state(1);
  auto single_expectation = hermition_conjugate(single_occupied_state) * hamiltonian * single_occupied_state;
  single_expectation = normal_order(single_expectation);
  single_expectation = single_expectation.evaluate(boson_commutator, substitutions);
  single_expectation.print();
  ASSERT_TRUE(check_answer(single_expectation, 1.5));

  auto double_occupied_state = normalised_n_occupied_state(2);
  auto double_expectation = hermition_conjugate(double_occupied_state) * hamiltonian * double_occupied_state;
  double_expectation = normal_order(double_expectation);
  double_expectation = double_expectation.evaluate(boson_commutator, substitutions);
  double_expectation.print();
  ASSERT_TRUE(check_answer(double_expectation, 2.5));

  auto triple_occupied_state = normalised_n_occupied_state(3);
  auto triple_expectation = hermition_conjugate(triple_occupied_state) * hamiltonian * triple_occupied_state;
  triple_expectation = normal_order(triple_expectation);
  triple_expectation = triple_expectation.evaluate(boson_commutator, substitutions);
  triple_expectation.print();
  ASSERT_TRUE(check_answer(triple_expectation, 3.5));

  auto quad_occupied_state = normalised_n_occupied_state(4);
  auto quad_expectation = hermition_conjugate(quad_occupied_state) * hamiltonian * quad_occupied_state;
  quad_expectation = normal_order(quad_expectation);
  quad_expectation = quad_expectation.evaluate(boson_commutator, substitutions);
  quad_expectation.print();
  ASSERT_TRUE(check_answer(quad_expectation, 4.5));
}

TEST(Examples, HarmonicOscilator_2D) {
  // hamiltonian = w0 * (a0! * a0 + 0.5) + w1 * (a1! * a1 + 0.5)
  // states are |x, y> represents particles in state x wrt to first
  // 1d oscilator and y wrt to second
  // As can get away with it in this case will use |x, y> = |x> |y>
  // Use info value to denote which dimension operator/state is referring to
  auto hamiltonian = (number(7.0) * ((creation_op(0) * anihilation_op(0)) + number(0.5)))
                   + (number(5.0) * ((creation_op(1) * anihilation_op(1)) + number(0.5)));
  hamiltonian.print();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
