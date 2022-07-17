#pragma once
#include "Expression.hpp"

namespace operators {

//----------------------------------------------------------------------------------------------------------------------
// sorting zone
//----------------------------------------------------------------------------------------------------------------------

template <class OperatorInfo>
static void add_terms_from_comutator(
    vector_type<vector_type<Operator<OperatorInfo>>> &sorted_terms,
    const Expression<OperatorInfo> &com, const std::size_t term_index,
    const unsigned i) {
  for (const auto &mul_term : com.expression) {
    if (std::any_of(mul_term.begin(), mul_term.end(),
                    [&](const Operator<OperatorInfo> &op) {
                      return op.is_evaluated_number() and
                             op.value() == ComplexNumber(0);
                    })) {
      continue;
    }
    const bool all_numbers = std::all_of(
        mul_term.begin(), mul_term.end(),
        [&](const Operator<OperatorInfo> &op) { return op.is_number(); });
    const bool all_one = std::all_of(mul_term.begin(), mul_term.end(),
                                     [&](const Operator<OperatorInfo> &op) {
                                       return op.is_evaluated_number() and
                                              op.value() == ComplexNumber(1.0);
                                     });
    assert(sorted_terms[term_index].size() >
           1); // Shouldn;t be commuting single  term
    const bool no_other_terms = sorted_terms[term_index].size() == 2U;

    // as commuter has value means new addition term must be added
    sorted_terms.push_back(vector_type<Operator<OperatorInfo>>());
    auto &new_term = sorted_terms.back();
    if (all_numbers and (no_other_terms or (not all_one))) {
      // numbers commute so send straight to the front. if they are one don't
      // bother
      new_term.insert(new_term.end(), mul_term.begin(), mul_term.end());
    }
    // insert all terms before the 2 non commuting terms here
    new_term.insert(new_term.end(), sorted_terms[term_index].begin(),
                    sorted_terms[term_index].begin() + i);
    if (not all_numbers) {
      for (const auto &op : mul_term) {
        new_term.push_back(op);
      }
    }
    // insert all terms after the 2 non commuting terms
    new_term.insert(new_term.end(), sorted_terms[term_index].begin() + i + 2,
                    sorted_terms[term_index].end());
  }
}

// also used in evaluate
template <class OperatorInfo, typename Commutator>
bool bubble_pass(
    const std::size_t term_index,
    vector_type<vector_type<Operator<OperatorInfo>>> &sorted_terms,
    Commutator commute,
    const SortUsing sortUsing) {
  bool swap_performed = false;
  int sign = 1;
  for (std::size_t i = 0; i < sorted_terms[term_index].size() - 1;
       ++i) { // should this really be using term
    if (sorted_terms[term_index][i] > sorted_terms[term_index][i + 1]) {
      const auto comutator =
          commute(sorted_terms[term_index][i], sorted_terms[term_index][i + 1]);
      add_terms_from_comutator(sorted_terms, comutator, term_index, i);
      std::swap(sorted_terms[term_index][i], sorted_terms[term_index][i + 1]);
      if (sortUsing == SortUsing::ANTICOMMUTATORS) {
        if (!(sorted_terms[term_index][i].is_number() or
              sorted_terms[term_index][i + 1].is_number())) {
          sign = sign * -1;
        }
      }
      swap_performed = true;
    }
  }
  if (sortUsing == SortUsing::ANTICOMMUTATORS and sign == -1) {
    sorted_terms[term_index].insert(sorted_terms[term_index].begin(),
                                    Operator<OperatorInfo>(-1));
  }
  return swap_performed;
}
template <class OperatorInfo, typename Commutator>
static Expression<OperatorInfo> sort_multiply_term(
    const vector_type<Operator<OperatorInfo>> &term,
    Commutator commute,
    const SortUsing sortUsing) {
  Expression<OperatorInfo> sorted_terms(
      vector_type<vector_type<Operator<OperatorInfo>>>(1));
  sorted_terms.expression[0] = term;
  for (std::size_t i = 0; i < sorted_terms.expression.size(); ++i) {
    // when this loop is entered the size will be one but every non zero
    // commutator will add extra terms to sorted terms
    if (sorted_terms.expression[i].empty()) {
      continue;
    }
    while (bubble_pass(i, sorted_terms.expression, commute, sortUsing))
      ;
  }
  return sorted_terms;
}

template <class OperatorInfo, typename Commutator>
Expression<OperatorInfo>
sort(const Expression<OperatorInfo> &expr,
     Commutator commute,
     const SortUsing sortUsing = SortUsing::COMMUTATORS) {
  spdlog::debug("Sorting exprssion begin");
  // addition is assumed to always be comutative
  Expression<OperatorInfo> sorted_expression;
  for (std::size_t add_index = 0; add_index < expr.expression.size();
       ++add_index) {
    const auto sorted_terms =
        sort_multiply_term(expr.expression[add_index], commute, sortUsing);
    sorted_expression.expression.insert(sorted_expression.expression.end(),
                                        sorted_terms.expression.begin(),
                                        sorted_terms.expression.end());
  }
  spdlog::debug("Sorting expression end");
  return sorted_expression;
}

// end sorting zone
//----------------------------------------------------------------------------------------------------------------------

} // namespace operators
