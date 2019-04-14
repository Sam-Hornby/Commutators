#ifndef _op_utils_hpp_
#define _op_utils_hpp_

#include "Operator.hpp"
#include "struct_ids.hpp"
#include "Expression.hpp"

namespace operators {

inline Operator vacuum_state() {
  return Operator("|0>", ordering_value(0), operator_info(0, Type::STATE_VECTOR));
}

inline Operator creation_op(const int info_value) {
  return Operator("a!_" + std::to_string(info_value), ordering_value(0),
                  operator_info(info_value, Type::CREATION_OPERATOR));
}

inline Operator anihilation_op(const int info_value) {
  return Operator("a_" + std::to_string(info_value), ordering_value(0),
                  operator_info(info_value, Type::ANIHILATION_OPERATOR));
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

Expression hermition_conjugate(const Expression & exp) {
  Expression new_exp;
  new_exp.expression.resize(exp.expression.size());
  for (unsigned i = 0; i < new_exp.expression.size(); ++i) {
    for (int j = exp.expression[i].size() - 1; j >= 0; --j) {
      new_exp.expression[i].push_back(hermition_conjugate(exp.expression[i][j]));
    }
  }
  return new_exp;
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

bool is_unspecified_op(const Operator & A) {
  return A.info.type == Type::UNSPECIFIED;
}

} // end namspace



#endif
