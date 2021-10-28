#pragma once

#include <Expression/Expression.hpp>

namespace operators {

template <class InfoA, class InfoB>
Expression<InfoB> transform_expression(
    const Expression<InfoA> &input,
    std::function<Expression<InfoB>(const Operator<InfoA> &)> transf) {
  Expression<InfoB> new_exp;
  for (const auto &mul_term : input.expression) {
    if (mul_term.empty()) {
      continue;
    }
    Expression<InfoB> new_terms = transf(mul_term[0]);
    for (unsigned i = 1; i < mul_term.size(); ++i) {
      new_terms = new_terms * transf(mul_term[i]);
    }
    new_exp = new_exp + new_terms;
  }
  return new_exp;
}

template <class InfoA, class InfoB>
Expression<InfoB> only_apply_to_operators(
    std::function<Expression<InfoB>(const Operator<InfoA> &)> transf,
    const Operator<InfoA> &A) {
  if (A.is_number()) {
    return {{{Operator<InfoB>(A.value())}}};
  }
  return transf(A);
}

} // namespace operators
