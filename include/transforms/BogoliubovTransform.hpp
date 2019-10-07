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
  // Will give a default but suspect for most custom classes, an overload
  // for that template instance will be needed.  Same for the create function
  std::abort();
}

template <>
bool isSecondOp(const Operator<Fock1DInfo> & op) {
  assert(op.info().x_coordinate < 2);
  return op.info().x_coordinate == 1;
}

template <class InfoA>
Operator<InfoA> createBogOp(Operator<InfoA> op, const bool swap) {
  std::abort();
  // TODO work out nice way for this
}

template <>
Operator<Fock1DInfo> createBogOp(Operator<Fock1DInfo> op, const bool swap) {
  assert(op.info().x_coordinate < 2);
  assert(isFockOpType(op.info().type));
  if (op.info().symbol != 'd') {
    op.info().symbol = 'd';
  } else {
    op.info().symbol = 'c';
  }
  if (swap) {
    op.info().x_coordinate = (op.info().x_coordinate xor 1);
    op.info().type = op.info().type == Type::CREATION_OPERATOR ?
                                         Type::ANIHILATION_OPERATOR : Type::CREATION_OPERATOR;
  }
  return op;
}

template <class InfoA>
Expression<InfoA>
bogoliubov_transform(const Expression<InfoA> & input,
                     const ComplexNumber coeff_a,
                     ComplexNumber coeff_b,
                     const OperatorType type) {
  return transform_expression<InfoA, InfoA>([&] (const Operator<InfoA> & op) {
    if (op.is_number() or isVectorType(op.info().type) {
      return {{{op}}};
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
