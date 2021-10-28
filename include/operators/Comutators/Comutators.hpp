#pragma once

#include <Expression/Expression.hpp>
#include <Utils/Utils.hpp>

// file of helpful comutator functions

namespace operators {

// returns zero, if 2 operators commute return this
template <class OperatorInfo>
inline Expression<OperatorInfo> zero_commutator() {
  return Expression<OperatorInfo>(
      vector_type<vector_type<Operator<OperatorInfo>>>(
          1,
          vector_type<Operator<OperatorInfo>>(1, Operator<OperatorInfo>(0))));
}
// If the commutator of 2 operators is a number return this
template <class OperatorInfo>
inline Expression<OperatorInfo> numeric_commutator(const double n) {
  return Expression<OperatorInfo>(
      vector_type<vector_type<Operator<OperatorInfo>>>(
          1,
          vector_type<Operator<OperatorInfo>>(1, Operator<OperatorInfo>(n))));
}
// if the commutator of 2 operatos is another operator return this
template <class OperatorInfo>
inline Expression<OperatorInfo> operator_commutator(const ordering_value order,
                                                    const OperatorInfo info) {
  return Expression<OperatorInfo>(
      vector_type<vector_type<Operator<OperatorInfo>>>(
          1, vector_type<Operator<OperatorInfo>>(
                 1, Operator<OperatorInfo>(order, info))));
}

template <class OperatorInfo>
bool either_is_number(const Operator<OperatorInfo> &A,
                      const Operator<OperatorInfo> &B) {
  return A.is_number() or B.is_number();
}
// Takes a comutator of operators and extends it to commute all numbers
// As simplify of expressions assumes all numbers commute best to wrap all
// comutators with this function
template <class OperatorInfo>
Expression<OperatorInfo> commute_numbers(
    const Operator<OperatorInfo> &A, const Operator<OperatorInfo> &B,
    std::function<Expression<OperatorInfo>(const Operator<OperatorInfo> &,
                                           const Operator<OperatorInfo> &)>
        commute) {
  if (either_is_number(A, B)) {
    return zero_commutator<OperatorInfo>();
  }
  return commute(A, B);
}

// Commute all
// ---------------------------------------------------------------------------------------------------------------------
// every operator is comutative
template <class OperatorInfo>
Expression<OperatorInfo> commute_all(const Operator<OperatorInfo> &,
                                     const Operator<OperatorInfo> &) {
  return zero_commutator<OperatorInfo>();
}

} // End namespace operators
