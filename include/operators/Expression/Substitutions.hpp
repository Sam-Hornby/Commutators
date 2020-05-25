#pragma once
#include "Expression.hpp"


namespace operators {
//----------------------------------------------------------------------------------------------------------------------
// perform substitutions zone
//----------------------------------------------------------------------------------------------------------------------

template <class OperatorInfo>
static bool bubble_subs(vector_type<Operator<OperatorInfo>> &exp,
                       std::function<bool(typename vector_type<Operator<OperatorInfo>>::iterator,
                                          vector_type<Operator<OperatorInfo>> &)> subst) {
  bool madeSub = false;
  for (auto it = exp.begin(); it != exp.end(); ++it) {
    if (subst(it, exp)) {
      madeSub = true;
    }
  }
  return madeSub;
}

template <class OperatorInfo>
static vector_type<Operator<OperatorInfo>>
perform_mul_subs(const vector_type<Operator<OperatorInfo>> & term,
              std::function<bool(typename vector_type<Operator<OperatorInfo>>::iterator,
                                 vector_type<Operator<OperatorInfo>> &)> subst) {
  spdlog::trace("perform_mul_subs begin");
  vector_type<Operator<OperatorInfo>> exp = term;
  while(bubble_subs(exp, subst));
  spdlog::trace("perform_mul_subs end");
  return exp;
}

template <class OperatorInfo>
Expression<OperatorInfo> perform_multiplication_substitutions(const Expression<OperatorInfo> &expr,
                  std::function<bool(typename vector_type<Operator<OperatorInfo>>::iterator,
                                     vector_type<Operator<OperatorInfo>> &)> subst) {
  spdlog::debug("performMultiplicationSubstitutions begin");
  Expression<OperatorInfo> final_exp(vector_type<vector_type<Operator<OperatorInfo>>>(expr.expression.size()));
  for (unsigned i = 0; i < expr.expression.size(); ++i) {
    final_exp.expression[i] = perform_mul_subs(expr.expression[i], subst);
  }
  spdlog::debug("performMultiplicationSubstitutions end");
  return final_exp;
}


//----------------------------------------------------------------------------------------------------------------------
}
