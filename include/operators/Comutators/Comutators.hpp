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
template <class OperatorInfo, class Comutator>
Expression<OperatorInfo> commute_numbers(
    const Operator<OperatorInfo> &A, const Operator<OperatorInfo> &B,
    Comutator commute) {
  if (either_is_number(A, B)) {
    return zero_commutator<OperatorInfo>();
  }
  return commute(A, B);
}
// Takes a comuator of operators and extends it commute all operators
// that don't have matching state. Should only be applied in situations
// where all non matching operators commute.
template <class OperatorInfo, class Comutator>
Expression<OperatorInfo> commute_non_matching(
      const Operator<OperatorInfo> &A, const Operator<OperatorInfo> &B,
      Comutator commute) {
  if (!A.info().match(B.info())) {
    return zero_commutator<OperatorInfo>();
  }
  return commute(A, B);
}
// Wrap comutator. A function that makes a commutator commute all
// numbers and non matching operators
template <class OperatorInfo, class Comutator>
Expression<OperatorInfo> wrap_commutator(const Operator<OperatorInfo> &A, const Operator<OperatorInfo> &B,
                                         Comutator commute) {
  auto tmp = [&] (const Operator<OperatorInfo> &A, const Operator<OperatorInfo> &B) {
    return commute_non_matching<OperatorInfo>(A, B, commute);
  };
  return commute_numbers<OperatorInfo>(A, B, tmp);
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
