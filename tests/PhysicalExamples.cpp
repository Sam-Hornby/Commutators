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

Expression boson_commutator_(const Operator & A, const Operator & B) {
  if (is_anihilation_op(A) and is_creation_op(B)) {
    return Expression({{number(1)}});
  }
  throw std::logic_error("Shouldn't be commuting anything else");
}

Expression boson_commutator(const Operator & A, const Operator & B) {
  return commute_numbers(A, B, boson_commutator_);
}

bool substitutions(std::vector<Operator>::iterator start, std::vector<Operator> & exp) {
  return anihilate_vacuum(start, exp) or all_states_orthognal(start, exp);
}

static double round_to_3_dp(double x) {
  return round(x * 1000) / 1000;
}

static bool check_answer(const Expression & exp, const double ans) {
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
  // todo use floats for numbers so can have half
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

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
