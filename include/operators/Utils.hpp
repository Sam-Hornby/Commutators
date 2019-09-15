#ifndef _op_utils_hpp_
#define _op_utils_hpp_

#include "Operator.hpp"
#include "struct_ids.hpp"
#include "Expression.hpp"
#include <cmath>
#include <math.h>

namespace operators {

inline Operator vacuum_state() {
  return Operator("|0>", ordering_value(0), operator_info(0, Type::STATE_VECTOR));
}

inline Operator creation_op(const int info_value) {
  return Operator("a_" + std::to_string(info_value) + "!", ordering_value(0),
                  operator_info(info_value, Type::CREATION_OPERATOR));
}

inline Operator anihilation_op(const int info_value) {
  return Operator("a_" + std::to_string(info_value), ordering_value(0),
                  operator_info(info_value, Type::ANIHILATION_OPERATOR));
}

inline Expression normalised_n_occupied_state(unsigned n, unsigned creation_op_index = 0) {
  Expression exp;
  exp.expression.push_back({vacuum_state()});
  double normalisation_factor = 1.0 / std::sqrt(tgamma(static_cast<double>(n + 1)));
  for (unsigned i = 0; i < n; ++i) {
    exp = creation_op(creation_op_index) * exp;
  }
  return number(normalisation_factor) * exp;
}


Operator hermition_conjugate(const Operator & op) {
  if (op.is_number()) {
    // yet to support complex numbers
    return op;
  }
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
  if (op.info.type == Type::CREATION_OPERATOR) {
    if (op.name[op.name.size() - 1] == '!') {
      std::string new_name(op.name, 0, op.name.size() - 1);
      return Operator(std::move(new_name), op.order, operator_info(op.info.value, type));
    }
  }
  if (op.info.type == Type::STATE_VECTOR) {
    if (op.name[0] == '|' and op.name[op.name.size() - 1] == '>') {
      std::string new_name = op.name;
      new_name[0] = '<';
      new_name[new_name.size() - 1] = '|';
      return Operator(std::move(new_name), op.order, operator_info(op.info.value, type));
    }
  }
  if (op.info.type == Type::HC_STATE_VECTOR) {
    if (op.name[0] == '<' and op.name[op.name.size() - 1] == '|') {
      std::string new_name = op.name;
      new_name[0] = '|';
      new_name[new_name.size() - 1] = '>';
      return Operator(std::move(new_name), op.order, operator_info(op.info.value, type));
    }
  }

  return Operator(op.name + "!", op.order, operator_info(op.info.value, type));
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
