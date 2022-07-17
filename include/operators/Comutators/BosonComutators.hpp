#pragma once
#include "Comutators.hpp"

namespace operators {

// Boson comutator
// ---------------------------------------------------------------------------------------------------------------------
template <class OperatorInfo>
Expression<OperatorInfo> boson_commutator_(const Operator<OperatorInfo> &A,
                                           const Operator<OperatorInfo> &B) {
  if (is_anihilation_op<OperatorInfo>(A) and is_creation_op<OperatorInfo>(B)) {
    assert((A.info().match(B.info()));
    return Expression<OperatorInfo>({{number<OperatorInfo>(1)}});
  }
  throw std::logic_error("Shouldn't be commuting anything else");
}

template <class OperatorInfo>
Expression<OperatorInfo> boson_commutator(const Operator<OperatorInfo> &A,
                                          const Operator<OperatorInfo> &B) {
  return wrap_commutator<OperatorInfo>(A, B, boson_commutator_<OperatorInfo>);
}
// ---------------------------------------------------------------------------------------------------------------------

} // namespace operators
