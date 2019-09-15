#ifndef _ordering_hpp_
#define _ordering_hpp_

#include "Operator.hpp"
#include "Expression.hpp"
#include "Utils.hpp"

namespace operators {

template <class OperatorInfo>
Operator<OperatorInfo> order_op(Operator<OperatorInfo> op) {
  if (is_state_vector<OperatorInfo>(op)) {
    op.order = ordering_value(200);
  } else if (is_hc_state_vector<OperatorInfo>(op)) {
    op.order = ordering_value(-200);
  } else if (is_anihilation_op<OperatorInfo>(op)) {
    op.order = ordering_value(100);
  } else if (is_creation_op<OperatorInfo>(op)) {
    op.order = ordering_value(-100);
  }
  return op;
}

template <class OperatorInfo>
Expression<OperatorInfo> normal_order(const Expression<OperatorInfo> & exp) {
  auto ordered = exp;
  for (auto & mul_term : ordered.expression) {
    for (auto & op : mul_term) {
      if (op.is_number()) {
        continue;
      }
      op = order_op<OperatorInfo>(op);
    }
  }
  return ordered;
}

} // end namespace


#endif
