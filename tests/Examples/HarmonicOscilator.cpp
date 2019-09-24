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
  std::string name() const {
    std::string result;
    if (isFockOpType(type)) {
      result += "a_0";
      if (type == Type::CREATION_OPERATOR) {
        result += "!";
      }
      return result;
    } else if (type == Type::STATE_VECTOR) {
      return "|0>";
    } else if (type == Type::HC_STATE_VECTOR) {
      return "<0|";
    } else {
      std::abort();
    }
  }
  bool operator==(Fock0DInfo other) const {
    return type == other.type;
  }

  bool operator!=(Fock0DInfo other) const {
    return not (*this == other);
  }
  bool operator<(Fock0DInfo other) const {
    return type < other.type;
  }
  static Fock0DInfo vacuumState() {
    return Fock0DInfo(Type::STATE_VECTOR);
  }
  bool isVacuumState() const {
    return type == Type::STATE_VECTOR;
  }
  bool isHCVacuumState() const {
    return type == Type::HC_STATE_VECTOR;
  }
  bool match(const Fock0DInfo & other) const {
    if (isFockOpType(type) and isFockOpType(other.type)) {
      return true;
    }
    if (isVectorType(type) and isVectorType(other.type)) {
      return true;
    }
    return false;
  }
};

Operator<Fock0DInfo> creation_op() {
  return Operator<Fock0DInfo>(ordering_value(0),
                  Fock0DInfo(Type::CREATION_OPERATOR));
}
Operator<Fock0DInfo> anihilation_op() {
  return Operator<Fock0DInfo>(ordering_value(0),
                  Fock0DInfo(Type::ANIHILATION_OPERATOR));
}
Expression<Fock0DInfo> normalised_n_occupied_state(unsigned n) {
  Expression<Fock0DInfo> exp;
  exp.expression.push_back({vacuum_state<Fock0DInfo>()});
  double normalisation_factor = 1.0 / std::sqrt(tgamma(static_cast<double>(n + 1)));
  for (unsigned i = 0; i < n; ++i) {
    exp = creation_op() * exp;
  }
  return number<Fock0DInfo>(normalisation_factor) * exp;
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
  auto hamiltonian = (creation_op() * anihilation_op()) + number<Fock0DInfo>(0.5);
  hamiltonian.print();
  // vacuum state expectation value
  Expression<Fock0DInfo> zero_occupied_state = normalised_n_occupied_state(0);
  auto vacuum_expectation = hermition_conjugate<Fock0DInfo>(zero_occupied_state) * hamiltonian * zero_occupied_state;
  vacuum_expectation = normal_order<Fock0DInfo>(vacuum_expectation);
  vacuum_expectation = vacuum_expectation.evaluate(boson_commutator<Fock0DInfo>, substitutions<Fock0DInfo>);
  vacuum_expectation.print();
  ASSERT_TRUE(check_answer(vacuum_expectation, 0.5));

  Expression<Fock0DInfo> single_occupied_state = normalised_n_occupied_state(1);
  auto single_expectation = hermition_conjugate<Fock0DInfo>(single_occupied_state) * hamiltonian * single_occupied_state;
  single_expectation = normal_order<Fock0DInfo>(single_expectation);
  single_expectation = single_expectation.evaluate(boson_commutator<Fock0DInfo>, substitutions<Fock0DInfo>);
  single_expectation.print();
  ASSERT_TRUE(check_answer(single_expectation, 1.5));

  Expression<Fock0DInfo> double_occupied_state = normalised_n_occupied_state(2);
  auto double_expectation = hermition_conjugate<Fock0DInfo>(double_occupied_state) * hamiltonian * double_occupied_state;
  double_expectation = normal_order<Fock0DInfo>(double_expectation);
  double_expectation = double_expectation.evaluate(boson_commutator<Fock0DInfo>, substitutions<Fock0DInfo>);
  double_expectation.print();
  ASSERT_TRUE(check_answer(double_expectation, 2.5));

  Expression<Fock0DInfo> triple_occupied_state = normalised_n_occupied_state(3);
  auto triple_expectation = hermition_conjugate<Fock0DInfo>(triple_occupied_state) * hamiltonian * triple_occupied_state;
  triple_expectation = normal_order<Fock0DInfo>(triple_expectation);
  triple_expectation = triple_expectation.evaluate(boson_commutator<Fock0DInfo>, substitutions<Fock0DInfo>);
  triple_expectation.print();
  ASSERT_TRUE(check_answer(triple_expectation, 3.5));

  Expression<Fock0DInfo> quad_occupied_state = normalised_n_occupied_state(4);
  auto quad_expectation = hermition_conjugate<Fock0DInfo>(quad_occupied_state) * hamiltonian * quad_occupied_state;
  quad_expectation = normal_order<Fock0DInfo>(quad_expectation);
  quad_expectation = quad_expectation.evaluate(boson_commutator<Fock0DInfo>, substitutions<Fock0DInfo>);
  quad_expectation.print();
  ASSERT_TRUE(check_answer(quad_expectation, 4.5));
}

TEST(Examples, HarmonicOscilator_2D) {
  // hamiltonian = w0 * (a0! * a0 + 0.5) + w1 * (a1! * a1 + 0.5)
  // states are |x, y> represents particles in state x wrt to first
  // 1d oscilator and y wrt to second
  // Use the x coordinate in the operator info to tell which dimension the operators belong. As only having vacuum state
  // all vaccum states will have zero as they belong to both
  auto hamiltonian = (number<Fock1DInfo>(7.0) * ((creation_op(0) * anihilation_op(0)) + number<Fock1DInfo>(0.5)))
                   + (number<Fock1DInfo>(5.0) * ((creation_op(1) * anihilation_op(1)) + number<Fock1DInfo>(0.5)));
  hamiltonian.print();
  // vacuum state expectation value
  Expression<Fock1DInfo> zero_occupied_state = normalised_n_occupied_state(0, 0);
  auto vacuum_expectation = hermition_conjugate<Fock1DInfo>(zero_occupied_state) * hamiltonian * zero_occupied_state;
  vacuum_expectation = normal_order<Fock1DInfo>(vacuum_expectation);
  vacuum_expectation = vacuum_expectation.evaluate(boson_commutator<Fock1DInfo>, substitutions<Fock1DInfo>);
  vacuum_expectation.print();
  ASSERT_TRUE(check_answer(vacuum_expectation, 6.0));

  // single occupied state in first HO
  Expression<Fock1DInfo> single_occupied_state = normalised_n_occupied_state(1, 0);
  auto expectation = hermition_conjugate<Fock1DInfo>(single_occupied_state) * hamiltonian * single_occupied_state;
  expectation = normal_order<Fock1DInfo>(expectation);
  expectation = expectation.evaluate(boson_commutator<Fock1DInfo>, substitutions<Fock1DInfo>);
  expectation.print();
  ASSERT_TRUE(check_answer(expectation, 13.0));

  // single occupied state in second HO
  single_occupied_state = normalised_n_occupied_state(1, 1);
  expectation = hermition_conjugate<Fock1DInfo>(single_occupied_state) * hamiltonian * single_occupied_state;
  expectation = normal_order<Fock1DInfo>(expectation);
  expectation = expectation.evaluate(boson_commutator<Fock1DInfo>, substitutions<Fock1DInfo>);
  expectation.print();
  ASSERT_TRUE(check_answer(expectation, 11.0));

  // both H0s occupied
  single_occupied_state = normalised_n_occupied_ops(3, 0) *
                          normalised_n_occupied_ops(6, 1) *
                          vacuum_state<Fock1DInfo>();
  expectation = hermition_conjugate<Fock1DInfo>(single_occupied_state) * hamiltonian * single_occupied_state;
  expectation = normal_order<Fock1DInfo>(expectation);
  expectation = expectation.evaluate(boson_commutator<Fock1DInfo>, substitutions<Fock1DInfo>);
  expectation.print();
  ASSERT_TRUE(check_answer(expectation, 57.0));

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
