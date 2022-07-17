#pragma once

#include "Comutators.hpp"

namespace operators {

// Spin comutator
// ------------------------------------------------------------

struct AxisAndSign {
  char axis;
  bool negative;
};

const std::unordered_map<std::string, AxisAndSign> spin_relations = {
  {"xy", {'z', false}},
  {"yx", {'z', true}},
  {"yz", {'x', false}},
  {"zy", {'x', true}},
  {"zx", {'y', false}},
  {"xz", {'y', true}},
};

template <class OperatorInfo>
Expression<OperatorInfo> spin_comutator_(const Operator<OperatorInfo> &A,
                                         const Operator<OperatorInfo> &B) {
  if ((A.type != Type::SPIN_OPERATORS) or (B.type != Type::SPIN_OPERATORS)) {
    throw std::logic_error("Need to be commuting spin operators to call this commutator");
  }
  if (A.symbol == B.symbol) {
    return zero_commutator<OperatorInfo>();
  }
  Operator<OperatorInfo> result = A;
  const auto output_info = spin_relations.at(
        std::to_string(A.symbol) + std::to_string(B.symbol));
  result.symbol = output_info.axis;
  return im_number<OperatorInfo>(output_info.negative ? -1.0 : 1.0)
            * result;
}

template <class OperatorInfo>
Expression<OperatorInfo> spin_comutator(const Operator<OperatorInfo> &A,
                                        const Operator<OperatorInfo> &B) {
  return wrap_commutator<OperatorInfo>(A, B, spin_comutator_<OperatorInfo>);
}

}