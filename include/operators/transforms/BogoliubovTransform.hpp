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

// c_0! = (coeff_a * b_0!) + (coeff_b * b_1)
// c_1! = (coeff_a * b_1!) +/- (coeff_b * b_0)

template <class InfoA>
bool isSecondOp(const Operator<InfoA> & op) {
  // Will give a default but suspect for most custom classes, an overload
  // for that template instance will be needed.  Same for the create function
  std::abort();
}

template <>
bool isSecondOp(const Operator<Fock1DInfo> & op) {
  return op.info().x_coordinate < 0;
}

template <class InfoA>
Operator<InfoA> createBogOp(Operator<InfoA> op, const bool swap) {
  std::abort();
  // TODO work out nice way for this
}

template <>
Operator<Fock1DInfo> createBogOp(Operator<Fock1DInfo> op, const bool swap) {
  assert(isFockOpType(op.info().type));
  assert(op.info().x_coordinate != 0);
  if (op.info().symbol != 'b') {
    op.info().symbol = 'b';
  } else {
    op.info().symbol = 'c';
  }
  if (swap) {
    op.info().x_coordinate = (op.info().x_coordinate *= -1);
    op.info().type = op.info().type == Type::CREATION_OPERATOR ?
                                         Type::ANIHILATION_OPERATOR : Type::CREATION_OPERATOR;
  }
  return op;
}

template <class InfoA>
Expression<InfoA>
bogoliubov_transform(const Expression<InfoA> & input,
                     const OperatorType type) {
  // maybe better off finding coefficents first then substituing
  auto coeff_a = named_number<InfoA>('u');
  auto coeff_b = named_number<InfoA>('v');
  auto result = transform_expression<InfoA, InfoA>(input, [&] (const Operator<InfoA> & op) {
    if (op.is_number() or isVectorType(op.info().type)) {
      return Expression<InfoA>({{op}});
    }
    if (not isFockOpType(op.info().type)) {
      throw std::logic_error("Unexpected type");
    }
    const auto sign_term = [&] () {
      if (type == OperatorType::FERMION and isSecondOp<InfoA>(op)) {
        return number<InfoA>(-1);
      }
      return number<InfoA>(1);
    }();
    return (coeff_a * createBogOp<InfoA>(op, false)) +
             (sign_term * coeff_b * createBogOp<InfoA>(op, true));
  });

  result = result.simplify_numbers();
  // std::sort on the operators, then try and factorise
  return result;
}

//template <class InfoA>
//Expression<InfoA>
//bogoliubov_transform(const Expression<InfoA> & input,
//                    const OperatorType) {
//  return input;
//}


}
