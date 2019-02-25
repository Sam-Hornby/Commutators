
//#include "Operator.hpp"
#include "Expression.hpp"
#include <algorithm>

namespace operators {

//----------------------------------------------------------------------------------------------------------------------
// printing zone
//----------------------------------------------------------------------------------------------------------------------

void Expression::print(std::ostream& out) const {
  if (expression.empty()) {
    out << "\n";
    return;
  }
  for (std::size_t i = 0; i < expression.size(); ++i) {
    out << "(";
    if (expression[i].empty()) {
      out << ") + ";
      continue;
    }
    for (std::size_t j = 0; j < expression[i].size(); ++j) {
      if (j != (expression[i].size() - 1)) {
        out << expression[i][j].name << " * ";
      } else {
        out << expression[i][j].name;
      }
    }
    if (i != (expression.size() - 1)) {
      out << ") + ";
    } else {
      out << ")\n";
    }
  }
}

// end printing zone
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// sorting zone
//----------------------------------------------------------------------------------------------------------------------

static void add_terms_from_comutator(std::vector<std::vector<Operator>> &sorted_terms,
                                     const Expression & com,
                                     const std::vector<Operator> & terms_in_question,
                                     const unsigned i) {
  for (const auto &mul_term : com.expression) {
    if (std::any_of(mul_term.begin(), mul_term.end(),
                    [&](const Operator &op) {
                      return op.is_number() && op.value.get() == 0;
                    })) {
      continue;
    }
    // as commuter has value means new addition term must be added
    sorted_terms.push_back(std::vector<Operator>());
    auto & new_term = sorted_terms.back();
    // insert all terms before the 2 non commuting terms here
    new_term.insert(new_term.end(), terms_in_question.begin(), terms_in_question.begin() + i);
    for (const auto &op : mul_term) {
      new_term.push_back(op);
    }
    // insert all terms after the 2 non commuting terms
    new_term.insert(new_term.end(), terms_in_question.begin() + i + 2, terms_in_question.end());
  }
}

static bool bubble_pass(std::vector<Operator> & terms_in_question,
                        std::vector<std::vector<Operator>> & sorted_terms,
                        std::function<Expression(const Operator &, const Operator &)> commute) {
  bool swap_performed = false;
  for (std::size_t i = 0; i < terms_in_question.size() - 1; ++i) { // should this really be using term
    if (terms_in_question[i] > terms_in_question[i+1]) {
      const auto comutator = commute(terms_in_question[i], terms_in_question[i+1]);
      add_terms_from_comutator(sorted_terms, comutator, terms_in_question, i);
      std::swap(sorted_terms[0][i], sorted_terms[0][i+1]);
      swap_performed = true;
    }
  }
  return swap_performed;
}

Expression Expression::sort_multiply_term(const std::vector<Operator> & term,
                                          std::function<Expression(const Operator &, const Operator &)> commute) const {
  Expression sorted_terms(std::vector<std::vector<Operator>>(1));
  sorted_terms.expression[0] = term;
  for (std::size_t i = 0; i < sorted_terms.expression.size(); ++i) {
    // when this loop is entered the size will be one but every non zero commutator will add extra terms to sorted terms
    if (sorted_terms.expression[i].empty()) {
      continue;
    }
    while(bubble_pass(sorted_terms.expression[i], sorted_terms.expression, commute));
  }
  return sorted_terms;
}

Expression Expression::sort(std::function<Expression(const Operator &, const Operator &)> commute) const {
  // addition is assumed to always be comutative
  Expression sorted_expression;
  for (std::size_t add_index = 0; add_index < expression.size(); ++add_index) {
    const auto sorted_terms = sort_multiply_term(expression[add_index], commute);
    sorted_expression.expression.insert(sorted_expression.expression.end(),
                                        sorted_terms.expression.begin(),
                                        sorted_terms.expression.end());
  }
  return sorted_expression;
}

// end sorting zone
//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------
// simplify numbers zone
//----------------------------------------------------------------------------------------------------------------------

static std::vector<Operator> simplify_multiply(const std::vector<Operator> & mul_term) {
  std::vector<Operator> simplified;
  simplified.push_back(Operator(1));
  for (const auto & op : mul_term) {
    if (op.is_number()) {
      *(simplified[0].value) *= op.value.get();
    } else {
      simplified.push_back(op);
    }
  }
  if (simplified[0].is_number() && simplified[0].value.get() == 0) {
    simplified.clear(); // if zero return empty vector
  }
  return simplified;
}

Expression Expression::simplify_numbers() const {
  Expression simplified;
  simplified.expression.resize(expression.size());
  for (std::size_t add_index = 0; add_index < expression.size(); ++add_index) {
    simplified.expression[add_index] = simplify_multiply(expression[add_index]);
  }
  for (std::size_t add_index = 0; add_index < expression.size(); ++add_index) {
    if (simplified.expression[add_index].empty()) {
      simplified.expression.erase(simplified.expression.begin() + add_index);
    }
  }
  return simplified;
}

// end numbers zone
//----------------------------------------------------------------------------------------------------------------------


} // end name space operators
