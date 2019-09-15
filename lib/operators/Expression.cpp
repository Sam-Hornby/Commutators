
#include "Expression.hpp"
/*#include <algorithm>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <sstream>


namespace operators {

//----------------------------------------------------------------------------------------------------------------------
// operator overloads
//----------------------------------------------------------------------------------------------------------------------


Expression operator+(const Expression & A, const Operator & B) {
  Expression exp = A;
  std::vector<Operator> m;
  m.push_back(B);
  exp.expression.push_back(std::move(m));
  return exp;
}

Expression operator+(const Operator & A, const Expression & B) {
  Expression exp({{A}});
  return exp + B;
}

Expression operator*(const Expression & A, const Operator & B) {
  Expression exp = A;
  for (std::size_t i = 0; i < A.expression.size(); ++i) {
    exp.expression[i].push_back(B);
  }
  return exp;
}

Expression operator*(const Operator & A, const Expression & B) {
  Expression exp({{A}});
  return exp * B;
}

Expression operator+(const Expression & A, const Expression & B) {
  Expression exp = A;
  exp.expression.insert(exp.expression.end(), B.expression.begin(), B.expression.end());
  return exp;
}

Expression operator*(const Expression & A, const Expression & B) {
  Expression exp;
  for (std::size_t i = 0; i < B.expression.size(); ++i) {
    exp = exp + A;
  }
  for (std::size_t i = 0; i < B.expression.size(); ++i) {
    for (std::size_t j = 0; j < A.expression.size(); ++j) {
      const auto index = i * A.expression.size() + j;
      exp.expression[index].insert(exp.expression[index].end(), B.expression[i].begin(), B.expression[i].end());
    }
  }
  return exp;
}

//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------
// printing zone
//----------------------------------------------------------------------------------------------------------------------

void Expression::print(std::ostream& out) const {

  for (std::size_t i = 0; i < expression.size(); ++i) {
    if (expression[i].empty()) {
      continue;
    }
    if (i != 0) {
      out << " + ";
    }
    out << "(";
    for (std::size_t j = 0; j < expression[i].size(); ++j) {
      if (j != (expression[i].size() - 1)) {
        out << expression[i][j].name << " * ";
      } else {
        out << expression[i][j].name;
      }
    }
    out << ")";
  }
  out << "\n";
}

// end printing zone
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// sorting zone
//----------------------------------------------------------------------------------------------------------------------

static void add_terms_from_comutator(std::vector<std::vector<Operator>> &sorted_terms,
                                     const Expression & com,
                                     const std::size_t term_index,
                                     const unsigned i) {
  for (const auto &mul_term : com.expression) {
    if (std::any_of(mul_term.begin(), mul_term.end(),
                    [&](const Operator &op) {
                      return op.is_number() and op.value.get() == 0;
                    })) {
      continue;
    }
    // as commuter has value means new addition term must be added
    sorted_terms.push_back(std::vector<Operator>());
    auto & new_term = sorted_terms.back();
    // insert all terms before the 2 non commuting terms here
    new_term.insert(new_term.end(), sorted_terms[term_index].begin(), sorted_terms[term_index].begin() + i);
    for (const auto &op : mul_term) {
      new_term.push_back(op);
    }
    // insert all terms after the 2 non commuting terms
    new_term.insert(new_term.end(), sorted_terms[term_index].begin() + i + 2, sorted_terms[term_index].end());
  }
}

static bool bubble_pass(const std::size_t term_index,
                        std::vector<std::vector<Operator>> & sorted_terms,
                        std::function<Expression(const Operator &, const Operator &)> commute,
                        const SortUsing sortUsing) {
  bool swap_performed = false;
  int sign = 1;
  for (std::size_t i = 0; i < sorted_terms[term_index].size() - 1; ++i) { // should this really be using term
    if (sorted_terms[term_index][i] > sorted_terms[term_index][i+1]) {
      const auto comutator = commute(sorted_terms[term_index][i], sorted_terms[term_index][i+1]);
      add_terms_from_comutator(sorted_terms, comutator, term_index, i);
      std::swap(sorted_terms[term_index][i], sorted_terms[term_index][i+1]);
      if (sortUsing == SortUsing::ANTICOMMUTATORS) {
        if (!(sorted_terms[term_index][i].is_number() or sorted_terms[term_index][i+1].is_number())) {
          sign = sign * -1;
        }
      }
      swap_performed = true;
    }
  }
  if (sortUsing == SortUsing::ANTICOMMUTATORS and sign == -1) {
    sorted_terms[term_index].insert(sorted_terms[term_index].begin(), Operator(-1));
  }
  return swap_performed;
}

static Expression sort_multiply_term(const std::vector<Operator> & term,
                                          std::function<Expression(const Operator &, const Operator &)> commute,
                                          const SortUsing sortUsing) {
  Expression sorted_terms(std::vector<std::vector<Operator>>(1));
  sorted_terms.expression[0] = term;
  for (std::size_t i = 0; i < sorted_terms.expression.size(); ++i) {
    // when this loop is entered the size will be one but every non zero commutator will add extra terms to sorted terms
    if (sorted_terms.expression[i].empty()) {
      continue;
    }
    while(bubble_pass(i, sorted_terms.expression, commute, sortUsing));
  }
  return sorted_terms;
}

Expression Expression::sort(std::function<Expression(const Operator &, const Operator &)> commute,
                            const SortUsing sortUsing) const {
  // addition is assumed to always be comutative
  Expression sorted_expression;
  for (std::size_t add_index = 0; add_index < expression.size(); ++add_index) {
    const auto sorted_terms = sort_multiply_term(expression[add_index], commute, sortUsing);
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
      simplified[0].value = simplified[0].value.get() * op.value.get();
    } else {
      simplified.push_back(op);
    }
  }
  if (simplified[0].is_number() and simplified[0].value.get() == 0) {
    simplified.clear(); // if zero return empty vector
  }
  if (simplified[0].is_number() and simplified[0].value.get() == 1 and simplified.size() != 1) {
    simplified.erase(simplified.begin());
  }
  if (simplified[0].is_number()) {
    simplified[0].name = std::to_string(simplified[0].value.get()); // correct name for number
  }
  return simplified;
}

static bool expression_operators_match(const std::vector<Operator> & A, const std::vector<Operator> & B) {
  // assumes that simplifying multiplies has moved all numbers to first position
  if (A.empty() or B.empty()) {
    return false;
  }
  unsigned size_A = A.size();
  unsigned start_index_A = static_cast<unsigned>(A[0].is_number());
  unsigned size_B = B.size();
  unsigned start_index_B = static_cast<unsigned>(B[0].is_number());
  if ((size_A - start_index_A) != (size_B - start_index_B)) {
    return false;
  }
  for (unsigned i = 0; i < (size_A - start_index_A); ++i) {
    if (A[i + start_index_A] != B[i + start_index_B]) {
      return false;
    }
  }
  return true;
}

static std::vector<Operator> combine_expressions(const std::vector<Operator> & A, const std::vector<Operator> & B) {
  std::vector<Operator> C;
  C.reserve(A.size() + 1);
  Operator number;
  if ((!A[0].is_number()) and (!B[0].is_number())) {
    number = Operator(2);
  } else if ((A[0].is_number()) && (!B[0].is_number())) {
    number = Operator(A[0].value.get() + 1);
  } else if ((!A[0].is_number()) && B[0].is_number()) {
    number = Operator(B[0].value.get() + 1);
  } else {
    number = Operator(A[0].value.get() + B[0].value.get());
  }
  C.push_back(number);
  unsigned start_index = static_cast<unsigned>(A[0].is_number());
  C.insert(C.end(), A.begin() + start_index, A.end());
  return C;
}

static void simplify_additions(Expression & exp) {
  for (std::size_t i = 0; i < exp.expression.size(); ++i) {
    for (std::size_t j = i + 1; j < exp.expression.size(); ++j) {
      if (expression_operators_match(exp.expression[i], exp.expression[j])) {
        exp.expression[i] = combine_expressions(exp.expression[i], exp.expression[j]);
        exp.expression[j].clear();
      }
    }
  }
}

static void removeEmptyVectors(Expression &exp) {
  for (std::size_t add_index = 0; add_index < exp.expression.size();) {
    if (exp.expression[add_index].empty()) {
      // remove empty vectors
      exp.expression.erase(exp.expression.begin() + add_index);
    } else {
      ++add_index;
    }
  }
}

Expression Expression::simplify_numbers() const {
  Expression simplified;
  simplified.expression.resize(expression.size());
  for (std::size_t add_index = 0; add_index < expression.size(); ++add_index) {
    simplified.expression[add_index] = simplify_multiply(expression[add_index]);
  }
  simplify_additions(simplified);
  removeEmptyVectors(simplified);
  return simplified;
}

// end numbers zone
//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------
// perform substitutions zone
//----------------------------------------------------------------------------------------------------------------------

static bool bubbleSubs(std::vector<Operator> &exp,
                       std::function<bool(std::vector<Operator>::iterator,
                                          std::vector<Operator> &)> subst) {
  bool madeSub = false;
  for (auto it = exp.begin(); it != exp.end(); ++it) {
    if (subst(it, exp)) {
      madeSub = true;
    }
  }
  return madeSub;
}

static std::vector<Operator>
performMulSubs(const std::vector<Operator> & term,
              std::function<bool(std::vector<Operator>::iterator,
                                 std::vector<Operator> &)> subst) {
  std::vector<Operator> exp = term;
  while(bubbleSubs(exp, subst));
  return exp;
}

Expression Expression::performMultiplicationSubstitutions(
                  std::function<bool(std::vector<Operator>::iterator,
                                     std::vector<Operator> &)> subst) const {
  Expression final_exp(std::vector<std::vector<Operator>>(expression.size()));
  for (unsigned i = 0; i < expression.size(); ++i) {
    final_exp.expression[i] = performMulSubs(expression[i], subst);
  }
  return final_exp;
}


//----------------------------------------------------------------------------------------------------------------------

static void log_expression(spdlog::logger & logger, const Expression & exp, const std::string & prefix) {
  std::stringstream loggingStream;
  exp.print(loggingStream);
  logger.info("After step {}: {}",prefix, loggingStream.str());
}

Expression Expression::evaluate(std::function<Expression(const Operator &, const Operator &)> commute,
                            std::function<bool(std::vector<Operator>::iterator, std::vector<Operator> &)> subst) const {
  // simplify numbers is much faster than the other functions and can make the the other functions run faster so call
  // it frequently

  // Set up logging(defaults to off)
  //-------------------------------------------------------------------------------
  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  console_sink->set_level(spdlog::level::info);
  console_sink->set_pattern("%^\033[33m[evaluate] [%l%$] %v%$");
  spdlog::logger logger("Operators", console_sink);
  logger.set_level(spdlog::level::off);
  //-------------------------------------------------------------------------------

  log_expression(logger, *this, "Start");
  auto exp = simplify_numbers();
  log_expression(logger, exp, "Simplify numbers 1");
  exp = exp.sort(commute);
  log_expression(logger, exp, "Sort");
  exp = exp.simplify_numbers();
  log_expression(logger, exp, "Simplify numbers 2");
  exp = exp.performMultiplicationSubstitutions(subst);
  log_expression(logger, exp, "Perform subs");
  exp = exp.simplify_numbers();
  log_expression(logger, exp, "Simplify numbers 3");
  return exp;
}


} // end name space operators
*/
