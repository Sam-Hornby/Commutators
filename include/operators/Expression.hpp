#ifndef _expression_hpp_
#define _expression_hpp_

#include <vector>
#include <iostream>
#include "Operator.hpp"
#include <algorithm>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <sstream>

namespace operators {

enum class SortUsing {
  COMMUTATORS,     // the commute function passed to sort returns the commutator of the 2 operators
  ANTICOMMUTATORS, // the commute function passed to sort returns the anticommutator of the 2 operators
};

// [A, B] = AB - BA
// BA = AB - [A, B]
// AB = [A, B] + BA
// {A, B} = AB + BA
// AB = -BA + {A, B}

template <class OperatorInfo>
class Expression {
public:
  std::vector<std::vector<Operator<OperatorInfo>>> expression;  // terms in inner vectors are considered multiplied, outer added
  void print(std::ostream & out = std::cout) const;   // print the expression
  Expression sort(std::function<Expression(const Operator<OperatorInfo> &, const Operator<OperatorInfo> &)> commute,
                  const SortUsing s = SortUsing::COMMUTATORS) const; // order expresion
  Expression simplify_numbers() const;  // combine numbers and remove zeros
  // substitute a sequence of multiplications for return value of subst function, empty optional denotes no substitution
  Expression performMultiplicationSubstitutions(
                  std::function<bool(typename std::vector<Operator<OperatorInfo>>::iterator,
                                     std::vector<Operator<OperatorInfo>> &)> subst) const;
  Expression evaluate(std::function<Expression(const Operator<OperatorInfo> &,
                      const Operator<OperatorInfo> &)> commute,
                      std::function<bool(typename std::vector<Operator<OperatorInfo>>::iterator,
                      std::vector<Operator<OperatorInfo>> &)> subst) const;
  Expression(std::vector<std::vector<Operator<OperatorInfo>>> expression) : expression(expression) {};
  Expression() = default;

};

template <class OperatorInfo>
Expression<OperatorInfo> operator+(const Operator<OperatorInfo> & A, const Operator<OperatorInfo> & B);
template <class OperatorInfo>
Expression<OperatorInfo> operator*(const Operator<OperatorInfo> & A, const Operator<OperatorInfo> & B);

template <class OperatorInfo>
Expression<OperatorInfo> operator+(const Expression<OperatorInfo> & A, const Operator<OperatorInfo> & B);
template <class OperatorInfo>
Expression<OperatorInfo> operator*(const Expression<OperatorInfo> & A, const Operator<OperatorInfo> & B);

template <class OperatorInfo>
Expression<OperatorInfo> operator+(const Operator<OperatorInfo> & A, const Expression<OperatorInfo> & B);
template <class OperatorInfo>
Expression<OperatorInfo> operator*(const Operator<OperatorInfo> & A, const Expression<OperatorInfo> & B);

template <class OperatorInfo>
Expression<OperatorInfo> operator+(const Expression<OperatorInfo> & A, const Expression<OperatorInfo> & B);
template <class OperatorInfo>
Expression<OperatorInfo> operator*(const Expression<OperatorInfo> & A, const Expression<OperatorInfo> & B);


//******************************************************************
// All definitions below
//******************************************************************
//----------------------------------------------------------------------------------------------------------------------
// operator overloads
//----------------------------------------------------------------------------------------------------------------------

template <class OperatorInfo>
Expression<OperatorInfo> operator+(const Operator<OperatorInfo> & A, const Operator<OperatorInfo> & B) {
  Expression<OperatorInfo> exp;
  exp.expression.resize(2);
  exp.expression[0] = {A};
  exp.expression[1] = {B};
  return exp;
}

template <class OperatorInfo>
Expression<OperatorInfo> operator*(const Operator<OperatorInfo> & A, const Operator<OperatorInfo> & B) {
  Expression<OperatorInfo> exp;
  exp.expression.resize(1);
  exp.expression[0].resize(2);
  exp.expression[0][0] = A;
  exp.expression[0][1] = B;
  return exp;
}

template <class OperatorInfo>
Expression<OperatorInfo> operator+(const Expression<OperatorInfo> & A, const Operator<OperatorInfo> & B) {
  Expression<OperatorInfo> exp = A;
  std::vector<Operator<OperatorInfo>> m;
  m.push_back(B);
  exp.expression.push_back(std::move(m));
  return exp;
}

template <class OperatorInfo>
Expression<OperatorInfo> operator+(const Operator<OperatorInfo> & A, const Expression<OperatorInfo> & B) {
  Expression<OperatorInfo> exp({{A}});
  return exp + B;
}

template <class OperatorInfo>
Expression<OperatorInfo> operator*(const Expression<OperatorInfo> & A, const Operator<OperatorInfo> & B) {
  Expression<OperatorInfo> exp = A;
  for (std::size_t i = 0; i < A.expression.size(); ++i) {
    exp.expression[i].push_back(B);
  }
  return exp;
}

template <class OperatorInfo>
Expression<OperatorInfo> operator*(const Operator<OperatorInfo> & A, const Expression<OperatorInfo> & B) {
  Expression<OperatorInfo> exp({{A}});
  return exp * B;
}

template <class OperatorInfo>
Expression<OperatorInfo> operator+(const Expression<OperatorInfo> & A, const Expression<OperatorInfo> & B) {
  Expression<OperatorInfo> exp = A;
  exp.expression.insert(exp.expression.end(), B.expression.begin(), B.expression.end());
  return exp;
}

template <class OperatorInfo>
Expression<OperatorInfo> operator*(const Expression<OperatorInfo> & A, const Expression<OperatorInfo> & B) {
  Expression<OperatorInfo> exp;
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

template <class OperatorInfo>
void Expression<OperatorInfo>::print(std::ostream& out) const {

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

template <class OperatorInfo>
static void add_terms_from_comutator(std::vector<std::vector<Operator<OperatorInfo>>> &sorted_terms,
                                     const Expression<OperatorInfo> & com,
                                     const std::size_t term_index,
                                     const unsigned i) {
  for (const auto &mul_term : com.expression) {
    if (std::any_of(mul_term.begin(), mul_term.end(),
                    [&](const Operator<OperatorInfo> &op) {
                      return op.is_number() and op.value.get() == 0;
                    })) {
      continue;
    }
    // as commuter has value means new addition term must be added
    sorted_terms.push_back(std::vector<Operator<OperatorInfo>>());
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

template <class OperatorInfo>
static bool
bubble_pass(const std::size_t term_index,
            std::vector<std::vector<Operator<OperatorInfo>>> & sorted_terms,
            std::function<Expression<OperatorInfo>(const Operator<OperatorInfo> &, const Operator<OperatorInfo> &)> commute,
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
    sorted_terms[term_index].insert(sorted_terms[term_index].begin(), Operator<OperatorInfo>(-1));
  }
  return swap_performed;
}
template <class OperatorInfo>
static Expression<OperatorInfo>
sort_multiply_term(const std::vector<Operator<OperatorInfo>> & term,
                  std::function<Expression<OperatorInfo>(const Operator<OperatorInfo> &, const Operator<OperatorInfo> &)> commute,
                  const SortUsing sortUsing) {
  Expression<OperatorInfo> sorted_terms(std::vector<std::vector<Operator<OperatorInfo>>>(1));
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

template <class OperatorInfo>
Expression<OperatorInfo>
Expression<OperatorInfo>::sort(std::function<Expression<OperatorInfo>(const Operator<OperatorInfo> &,
                               const Operator<OperatorInfo> &)> commute,
                               const SortUsing sortUsing) const {
  // addition is assumed to always be comutative
  Expression<OperatorInfo> sorted_expression;
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

template <class OperatorInfo>
static std::vector<Operator<OperatorInfo>>
simplify_multiply(const std::vector<Operator<OperatorInfo>> & mul_term) {
  std::vector<Operator<OperatorInfo>> simplified;
  simplified.push_back(Operator<OperatorInfo>(1));
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

template <class OperatorInfo>
static bool expression_operators_match(const std::vector<Operator<OperatorInfo>> & A,
                                       const std::vector<Operator<OperatorInfo>> & B) {
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

template <class OperatorInfo>
static std::vector<Operator<OperatorInfo>>
combine_expressions(const std::vector<Operator<OperatorInfo>> & A, const std::vector<Operator<OperatorInfo>> & B) {
  std::vector<Operator<OperatorInfo>> C;
  C.reserve(A.size() + 1);
  Operator<OperatorInfo> number;
  if ((!A[0].is_number()) and (!B[0].is_number())) {
    number = Operator<OperatorInfo>(2);
  } else if ((A[0].is_number()) && (!B[0].is_number())) {
    number = Operator<OperatorInfo>(A[0].value.get() + 1);
  } else if ((!A[0].is_number()) && B[0].is_number()) {
    number = Operator<OperatorInfo>(B[0].value.get() + 1);
  } else {
    number = Operator<OperatorInfo>(A[0].value.get() + B[0].value.get());
  }
  C.push_back(number);
  unsigned start_index = static_cast<unsigned>(A[0].is_number());
  C.insert(C.end(), A.begin() + start_index, A.end());
  return C;
}

template <class OperatorInfo>
static void simplify_additions(Expression<OperatorInfo> & exp) {
  for (std::size_t i = 0; i < exp.expression.size(); ++i) {
    for (std::size_t j = i + 1; j < exp.expression.size(); ++j) {
      if (expression_operators_match(exp.expression[i], exp.expression[j])) {
        exp.expression[i] = combine_expressions(exp.expression[i], exp.expression[j]);
        exp.expression[j].clear();
      }
    }
  }
}

template <class OperatorInfo>
static void removeEmptyVectors(Expression<OperatorInfo> &exp) {
  for (std::size_t add_index = 0; add_index < exp.expression.size();) {
    if (exp.expression[add_index].empty()) {
      // remove empty vectors
      exp.expression.erase(exp.expression.begin() + add_index);
    } else {
      ++add_index;
    }
  }
}

template <class OperatorInfo>
Expression<OperatorInfo> Expression<OperatorInfo>::simplify_numbers() const {
  Expression<OperatorInfo> simplified;
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

template <class OperatorInfo>
static bool bubbleSubs(std::vector<Operator<OperatorInfo>> &exp,
                       std::function<bool(typename std::vector<Operator<OperatorInfo>>::iterator,
                                          std::vector<Operator<OperatorInfo>> &)> subst) {
  bool madeSub = false;
  for (auto it = exp.begin(); it != exp.end(); ++it) {
    if (subst(it, exp)) {
      madeSub = true;
    }
  }
  return madeSub;
}

template <class OperatorInfo>
static std::vector<Operator<OperatorInfo>>
performMulSubs(const std::vector<Operator<OperatorInfo>> & term,
              std::function<bool(typename std::vector<Operator<OperatorInfo>>::iterator,
                                 std::vector<Operator<OperatorInfo>> &)> subst) {
  std::vector<Operator<OperatorInfo>> exp = term;
  while(bubbleSubs(exp, subst));
  return exp;
}

template <class OperatorInfo>
Expression<OperatorInfo> Expression<OperatorInfo>::performMultiplicationSubstitutions(
                  std::function<bool(std::vector<Operator<OperatorInfo>>::iterator,
                                     std::vector<Operator<OperatorInfo>> &)> subst) const {
  Expression<OperatorInfo> final_exp(std::vector<std::vector<Operator<OperatorInfo>>>(expression.size()));
  for (unsigned i = 0; i < expression.size(); ++i) {
    final_exp.expression[i] = performMulSubs(expression[i], subst);
  }
  return final_exp;
}


//----------------------------------------------------------------------------------------------------------------------

template <class OperatorInfo>
static void log_expression(spdlog::logger & logger, const Expression<OperatorInfo> & exp, const std::string & prefix) {
  std::stringstream loggingStream;
  exp.print(loggingStream);
  logger.info("After step {}: {}",prefix, loggingStream.str());
}

template <class OperatorInfo>
Expression<OperatorInfo>
Expression<OperatorInfo>::evaluate(
        std::function<Expression<OperatorInfo>(const Operator<OperatorInfo> &, const Operator<OperatorInfo> &)> commute,
        std::function<bool(std::vector<Operator<OperatorInfo>>::iterator,
                           std::vector<Operator<OperatorInfo>> &)> subst) const {
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


} // end namespace operators

#endif // header guard
