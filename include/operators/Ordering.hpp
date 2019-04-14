#ifndef _ordering_hpp_
#define _ordering_hpp_

#include "Operator.hpp"
#include "Expression.hpp"
#include "Utils.hpp"

namespace operators {

Operator order_op(Operator op) {
  if (is_state_vector(op)) {
    op.order = ordering_value(200);
  } else if (is_hc_state_vector(op)) {
    op.order = ordering_value(-200);
  } else if (is_anihilation_op(op)) {
    op.order = ordering_value(100);
  } else if (is_creation_op(op)) {
    op.order = ordering_value(-100);
  }
  return op;
}

Expression normal_order(const Expression & exp) {
  auto ordered = exp;
  for (auto & mul_term : ordered.expression) {
    for (auto & op : mul_term) {
      if (op.is_number()) {
        continue;
      }
      op = order_op(op);
    }
  }
  return ordered;
}

} // end namespace


#endif
