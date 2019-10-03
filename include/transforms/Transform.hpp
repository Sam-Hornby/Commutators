#pragma once

#include "../Expression.hpp"

namespace operators {

template <class InfoA, class InfoB>
Expression<InfoB> transform_expression(const Expression<InfoA> & input,
                                       std::function<Expression<InfoB>(const Operator<InfoA> &)> transf) {
  Expression<InfoB> new_exp;
  for (const auto & mul_term : input) {
    Expression<InfoB> new_terms;
    for (const auto & op : mul_term) {
      new_terms = new_terms * transf(op);
    }
    new_exp = new_exp + new_terms;
  }
  return new_exp;
}

template <class InfoA, class InfoB>
Expression<InfoB>
only_apply_to_operators(std::function<Expression<InfoB>(const Operator<InfoA> &)> transf,
                        const Operator<InfoA> & A) {
  if (A.is_number()) {
    return {{{Operator(A.value())}}};
  }
  return transf(A);
}

}
