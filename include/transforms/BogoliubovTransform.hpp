// To perform the bogoliubov transform
#pragma once

#include "Expression.hpp"
#include "ImaginaryNumber.hpp"
#include "Transform.hpp"

namespace operators {

enum class OperatorType {
  BOSON,
  FERMION,
};

// c_0! = (coeff_a * d_0!) + (coeff_b * d_1)
// c_1! = (coeff_a * d_1!) +/- (coeff_b * d_0)

template <class InfoA>
bool isSecondOp(const Operator<InfoA> & op) [
  std::abort();
  // TODO work out be way for this
]

template <class InfoA>
Operator<InfoA> createBogOp(const Operator<InfoA> & op, const bool swap) {
  std::abort();
  // TODO work out nice way for this
}

template <class InfoA>
Expression<InfoA>
bogoliubov_transform(const Expression<InfoA> & input,
                     const ComplexNumber coeff_a,
                     ComplexNumber coeff_b,
                     const OperatorType type) {
  return transform_expression<InfoA, InfoA>([&] (const Operator<InfoA> & op) {
    if (op.is_number() or isVectorType(op.info().type) {
      return {{{op)}}};
    }
    if (not isFockOpType(op.info().type)) {
      throw std::logic_error("Unexpected type");
    }
    if (type == OperatorType::FERMIONS and isSecondOp<InfoA>(op)) {
      coeff_b = coeff_b * -1;
    }
    return (coeff_a * createBogOp<InfoA>(op, false)) + (coeff_b * createBogOp<InfoA>(op, true));
  });
}


}
