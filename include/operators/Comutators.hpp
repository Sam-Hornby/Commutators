#ifndef _commutators_hpp_
#define _commutators_hpp_

#include "Expression.hpp"
#include "Utils.hpp"

// file of helpful comutator functions

namespace operators {

// returns zero, if 2 operators commute return this
template <class OperatorInfo>
inline Expression<OperatorInfo> zero_commutator() {
  return Expression<OperatorInfo>(std::vector<std::vector<Operator<OperatorInfo>>>(1,
                                                       std::vector<Operator<OperatorInfo>>(1, Operator<OperatorInfo>(0))
                                                      ));
}
// If the commutator of 2 operators is a number return this
template <class OperatorInfo>
inline Expression<OperatorInfo> numeric_commutator(const double n) {
  return Expression<OperatorInfo>(std::vector<std::vector<Operator<OperatorInfo>>>(1,
                                                       std::vector<Operator<OperatorInfo>>(1, Operator<OperatorInfo>(n))
                                                      ));
}
// if the commutator of 2 operatos is another operator return this
template <class OperatorInfo>
inline Expression<OperatorInfo>
operator_commutator(std::string name, const ordering_value order, const OperatorInfo info) {
  return Expression<OperatorInfo>(std::vector<std::vector<Operator<OperatorInfo>>>(1,
                                  std::vector<Operator<OperatorInfo>>(1, Operator<OperatorInfo>(name, order, info))));
}

template <class OperatorInfo>
bool either_is_number(const Operator<OperatorInfo> & A, const Operator<OperatorInfo> & B) {
  return A.is_number() or B.is_number();
}
// Takes a comutator of operators and extends it to commute all numbers
// As simplify of expressions assumes all numbers commute best to wrap all comutators with this function
template <class OperatorInfo>
Expression<OperatorInfo>
commute_numbers(const Operator<OperatorInfo> & A, const Operator<OperatorInfo> & B,
                std::function<Expression<OperatorInfo>(const Operator<OperatorInfo> &,
                                                       const Operator<OperatorInfo> &)> commute) {
  if (either_is_number(A, B)) {
    return zero_commutator<OperatorInfo>();
  }
  return commute(A, B);
}

// Commute all
// ---------------------------------------------------------------------------------------------------------------------
// every operator is comutative
template <class OperatorInfo>
Expression<OperatorInfo> commute_all(const Operator<OperatorInfo> &, const Operator<OperatorInfo> &) {
  return zero_commutator<OperatorInfo>();
}

// Commute none
// ---------------------------------------------------------------------------------------------------------------------
template <class OperatorInfo>
Expression<OperatorInfo> commute_none_(const Operator<OperatorInfo> & A, const Operator<OperatorInfo> & B) {
  std::string com = "[" + A.name + ", " + B.name + "]";
  return operator_commutator<OperatorInfo>(std::move(com), ordering_value(0), OperatorInfo());
}

template <class OperatorInfo>
Expression<OperatorInfo> commute_none(const Operator<OperatorInfo> & A, const Operator<OperatorInfo> & B) {
  return commute_numbers<OperatorInfo>(A, B, commute_none_<Fock1DInfo>);
}

// Anti commute none
// ---------------------------------------------------------------------------------------------------------------------
template <class OperatorInfo>
Expression<OperatorInfo> anticommute_none_(const Operator<OperatorInfo> & A, const Operator<OperatorInfo> & B) {
  std::string com = "{" + A.name + ", " + B.name + "}";
  return operator_commutator<OperatorInfo>(std::move(com), ordering_value(0), OperatorInfo(0));
}

template <class OperatorInfo>
Expression<OperatorInfo> anticommute_none(const Operator<OperatorInfo> & A, const Operator<OperatorInfo> & B) {
  return commute_numbers<OperatorInfo>(A, B, anticommute_none_<Fock1DInfo>);
}
// ---------------------------------------------------------------------------------------------------------------------


// Boson comutator
// ---------------------------------------------------------------------------------------------------------------------
template <class OperatorInfo>
Expression<OperatorInfo> boson_commutator_(const Operator<OperatorInfo> & A, const Operator<OperatorInfo> & B) {
  if (is_anihilation_op<OperatorInfo>(A) and is_creation_op<OperatorInfo>(B)) {
    if (A.info.match(B.info)) {
      return Expression<OperatorInfo>({{number<OperatorInfo>(1)}});
    } else {
      return zero_commutator<OperatorInfo>();
    }
  }
  throw std::logic_error("Shouldn't be commuting anything else");
}

template <class OperatorInfo>
Expression<OperatorInfo> boson_commutator(const Operator<OperatorInfo> & A, const Operator<OperatorInfo> & B) {
  return commute_numbers<OperatorInfo>(A, B, boson_commutator_<OperatorInfo>);
}
// ---------------------------------------------------------------------------------------------------------------------



} // End namespace operators

#endif // header guard
