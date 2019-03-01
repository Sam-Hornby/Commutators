#ifndef _commutators_hpp_
#define _commutators_hpp_

#include "Expression.hpp"

// file of helpful comutator functions

namespace operators {

// returns zero, if 2 operators commute return this
inline Expression zero_commutator() {
  return Expression(std::vector<std::vector<Operator>>(1,
                                                       std::vector<Operator>(1, Operator(0))
                                                      ));
}
// If the commutator of 2 operators is a number return this
inline Expression numeric_commutator(const int n) {
  return Expression(std::vector<std::vector<Operator>>(1,
                                                       std::vector<Operator>(1, Operator(n))
                                                      ));
}
// if the commutator of 2 operatos is another operator return this
inline Expression operator_commutator(std::string name, const ordering_value order, const operator_info info) {
  return Expression(std::vector<std::vector<Operator>>(1,
                                                       std::vector<Operator>(1, Operator(name, order, info))
                                                      ));
}

bool either_is_number(const Operator & A, const Operator & B) {
  return A.is_number() or B.is_number();
}
// Takes a comutator of operators and extends it to commute all numbers
// As simplify of expressions assumes all numbers commute best to wrap all comutators with this function
Expression commute_numbers(const Operator & A, const Operator & B,
                           std::function<Expression(const Operator &, const Operator &)> commute) {
  if (either_is_number(A, B)) {
    return zero_commutator();
  }
  return commute(A, B);
}
// every operator is comutative
Expression commute_all(const Operator &, const Operator &) {
  return zero_commutator();
}

Expression commute_none_(const Operator & A, const Operator & B) {
  std::string com = "[" + A.name + ", " + B.name + "]";
  return operator_commutator(std::move(com), ordering_value(0), operator_info(0));
}

Expression commute_none(const Operator & A, const Operator & B) {
  return commute_numbers(A, B, commute_none_);
}



} // End namespace operators

#endif // header guard
