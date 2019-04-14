#include "gtest/gtest.h"
#include "Expression.hpp"
#include "Substitutions.hpp"
#include "Ordering.hpp"
#include "Utils.hpp"
#include "Operator.hpp"
#include "Comutators.hpp"
#include <exception>

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

TEST(Examples, HarmonicOscilator) {
  // todo use floats for numbers so can have half
  auto hamiltonian = (creation_op(0) * anihilation_op(0)) + number(2);
  // vacuum state expectation value
  auto vacuum_expectation = hermition_conjugate(vacuum_state()) * hamiltonian * vacuum_state();
  vacuum_expectation = normal_order(vacuum_expectation);
  vacuum_expectation = vacuum_expectation.evaluate(boson_commutator, substitutions);
  vacuum_expectation.print();

  auto single_occupied_state = creation_op(0) * vacuum_state();
  // todo overload hc to do expressions
  auto single_expectation = hermition_conjugate(single_occupied_state) * hamiltonian * single_occupied_state;
  single_expectation = normal_order(single_expectation);
  single_expectation = single_expectation.evaluate(boson_commutator, substitutions);
  single_expectation.print();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
