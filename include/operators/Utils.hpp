#ifndef _op_utils_hpp_
#define _op_utils_hpp_

#include "Operator.hpp"
#include "struct_ids.hpp"

namespace operators {

inline Operator vacuum_state() {
  return Operator("|0>", ordering_value(0), operator_info(0, Type::STATE_VECTOR));
}

Operator hermition_conjugate(const Operator & op) {
  auto type = op.info.type;
  if (type == Type::STATE_VECTOR) {
    type = Type::HC_STATE_VECTOR;
  } else if (type == Type::HC_STATE_VECTOR) {
    type = Type::STATE_VECTOR;
  } else if (type == Type::CREATION_OPERATOR) {
    type = Type::ANIHILATION_OPERATOR;
  } else if (type == Type::ANIHILATION_OPERATOR) {
    type = Type::CREATION_OPERATOR;
  }
  return Operator(op.name + "_dag", op.order, operator_info(op.info.value, type));
}

bool is_state_vector(const Operator & A) {
  return A.info.type == Type::STATE_VECTOR;
}

bool is_hc_state_vector(const Operator & A) {
  return A.info.type == Type::HC_STATE_VECTOR;
}

bool is_creation_op(const Operator & A) {
  return A.info.type == Type::CREATION_OPERATOR;
}

bool is_anihilation_op(const Operator & A) {
  return A.info.type == Type::ANIHILATION_OPERATOR;
}

bool is_unspecified(const Operator & A) {
  return A.info.type == Type::UNSPECIFIED;
}

} // end namspace



#endif
