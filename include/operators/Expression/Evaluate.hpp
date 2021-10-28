#pragma once

#include "Expression.hpp"
#include "Simplify.hpp"
#include "Sort.hpp"
#include <Expression/Substitutions.hpp>

namespace operators {

//----------------------------------------------------------------------------------------------------------------------
// all at once zone
//----------------------------------------------------------------------------------------------------------------------
template <class OperatorInfo, typename SubstitutionCallable>
static Expression<OperatorInfo> interleaved_sort_sub_simple(
    Expression<OperatorInfo> exp,
    std::function<Expression<OperatorInfo>(const Operator<OperatorInfo> &,
                                           const Operator<OperatorInfo> &)>
        commute,
    SubstitutionCallable subst, const SortUsing s) {

  // the thinking behind this approach is the bubble pass will ensure that the
  // last item is the highest, so in theory if there is one annihilation op this
  // will make it to the end after one bubble pass. Then run substitution step
  // here to remove that term before commuting any further. This stops the
  // sort adding too many unnecessary terms that will slow everything that comes
  // after down
  bool madeChanges = true;
  unsigned count = 0;
  while (madeChanges) {
    spdlog::info("Interleaved bubble loop {}", count++);
    spdlog::info("Size of expression {}", exp.expression.size());
    madeChanges = false;
    std::size_t mul_size = 0;
    for (unsigned i = 0; i < exp.expression.size(); ++i) {
      if (mul_size != exp.expression[i].size()) {
        mul_size = exp.expression[i].size();
        spdlog::debug("Doing size {}", mul_size);
      }
      if (exp.expression[i].empty()) {
        continue;
      }
      madeChanges |= bubble_pass(i, exp.expression, commute, s);
    }
    spdlog::info("Size of expression {}", exp.expression.size());
    exp = perform_multiplication_substitutions<OperatorInfo,
                                               SubstitutionCallable>(
        std::move(exp), subst);
    exp = simplify_multiplications(exp);
    spdlog::info("Size of expression {}", exp.expression.size());
  }
  return exp;
}

template <class OperatorInfo, typename SubstitutionCallable>
Expression<OperatorInfo> interleaved_evaluate(
    const Expression<OperatorInfo> &expr,
    std::function<Expression<OperatorInfo>(const Operator<OperatorInfo> &,
                                           const Operator<OperatorInfo> &)>
        commute,
    SubstitutionCallable subst, const SortUsing s = SortUsing::COMMUTATORS) {

  return interleaved_sort_sub_simple(expr, commute, subst, s);
}

//----------------------------------------------------------------------------------------------------------------------

template <class OperatorInfo>
static void log_expression(const Expression<OperatorInfo> &exp,
                           const std::string &prefix) {
  spdlog::info("After step {}: {}", prefix, exp.print(false));
}

template <class OperatorInfo, typename SubstitutionCallable>
Expression<OperatorInfo>
evaluate(const Expression<OperatorInfo> &input,
         std::function<Expression<OperatorInfo>(const Operator<OperatorInfo> &,
                                                const Operator<OperatorInfo> &)>
             commute,
         SubstitutionCallable subst,
         const SortUsing s = SortUsing::COMMUTATORS) {
  // simplify numbers is much faster than the other functions and can make the
  // the other functions run faster so call it frequently

  spdlog::critical("Start evaluation: Simplift numbers");
  log_expression(input, "Start");
  auto exp = simplify_numbers(input);
  log_expression(exp, "Simplify numbers 1");
  spdlog::critical("Interleaved evaluate");
  exp = interleaved_evaluate(exp, commute, subst, s);
  spdlog::critical("Simplify multiplications");
  exp = simplify_numbers(exp);
  log_expression(exp, "Simplify numbers 2");
  spdlog::critical("End");
  return exp;
}

} // namespace operators
