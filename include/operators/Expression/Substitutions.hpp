#pragma once
#include "Expression.hpp"

namespace operators {
//----------------------------------------------------------------------------------------------------------------------
// perform substitutions zone
//----------------------------------------------------------------------------------------------------------------------

template <class OperatorInfo, typename F>
static bool bubble_subs(vector_type<Operator<OperatorInfo>> &exp, F subst) {
  bool madeSub = false;
  for (auto it = exp.begin(); it != exp.end(); ++it) {
    madeSub |= subst(it, exp);
  }
  return madeSub;
}

template <class OperatorInfo, typename F>
static vector_type<Operator<OperatorInfo>>
perform_mul_subs(vector_type<Operator<OperatorInfo>> &term, F subst) {
  spdlog::trace("perform_mul_subs begin");
  vector_type<Operator<OperatorInfo>> exp = std::move(term);
  while (bubble_subs(exp, subst))
    ;
  spdlog::trace("perform_mul_subs end");
  return exp;
}

template <class OperatorInfo, typename F>
Expression<OperatorInfo>
perform_multiplication_substitutions(Expression<OperatorInfo> expr, F subst) {
  spdlog::debug("performMultiplicationSubstitutions begin");
  Expression<OperatorInfo> final_exp(
      vector_type<vector_type<Operator<OperatorInfo>>>(expr.expression.size()));
  for (unsigned i = 0; i < expr.expression.size(); ++i) {
    final_exp.expression[i] = perform_mul_subs(expr.expression[i], subst);
  }
  spdlog::debug("performMultiplicationSubstitutions end");
  return final_exp;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace operators
