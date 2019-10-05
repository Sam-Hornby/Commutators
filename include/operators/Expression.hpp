#ifndef _expression_hpp_
#define _expression_hpp_

#include <vector>
#include <map>
#include <iostream>
#include "Operator.hpp"
#include <algorithm>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <sstream>
#include <boost/container/small_vector.hpp>

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

template <class T>
using vector_type = boost::container::small_vector<T, 1>;

template <class OperatorInfo>
class Expression {
public:
  vector_type<vector_type<Operator<OperatorInfo>>> expression;  // terms in inner vectors are considered multiplied, outer added
  void print(std::ostream & out = std::cout, const bool add_newline = true) const;   // print the expression
  Expression sort(std::function<Expression(const Operator<OperatorInfo> &, const Operator<OperatorInfo> &)> commute,
                  const SortUsing s = SortUsing::COMMUTATORS) const; // order expresion
  Expression simplify_numbers() const;  // combine numbers and remove zeros
  Expression simplify_multiplications() const;  // simplify zeros and ones
  // substitute a sequence of multiplications for return value of subst function, empty optional denotes no substitution
  Expression performMultiplicationSubstitutions(
                  std::function<bool(typename vector_type<Operator<OperatorInfo>>::iterator,
                                     vector_type<Operator<OperatorInfo>> &)> subst) const;
  Expression evaluate(std::function<Expression(const Operator<OperatorInfo> &,
                      const Operator<OperatorInfo> &)> commute,
                      std::function<bool(typename vector_type<Operator<OperatorInfo>>::iterator,
                                         vector_type<Operator<OperatorInfo>> &)> subst,
                      const SortUsing s = SortUsing::COMMUTATORS) const;
  Expression interleaved_evaluate(
      std::function<Expression(const Operator<OperatorInfo> &, const Operator<OperatorInfo> &)> commute,
      std::function<bool(typename vector_type<Operator<OperatorInfo>>::iterator,
                         vector_type<Operator<OperatorInfo>> &)> subst,
      const SortUsing s = SortUsing::COMMUTATORS) const;
  Expression(vector_type<vector_type<Operator<OperatorInfo>>> expression) : expression(expression) {};
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
  vector_type<Operator<OperatorInfo>> m;
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
void Expression<OperatorInfo>::print(std::ostream& out, const bool add_newline) const {

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
        out << expression[i][j].name() << " * ";
      } else {
        out << expression[i][j].name();
      }
    }
    out << ")";
  }
  if (add_newline) {
    out << "\n";
  }
}

// end printing zone
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// sorting zone
//----------------------------------------------------------------------------------------------------------------------

template <class OperatorInfo>
static void add_terms_from_comutator(vector_type<vector_type<Operator<OperatorInfo>>> &sorted_terms,
                                     const Expression<OperatorInfo> & com,
                                     const std::size_t term_index,
                                     const unsigned i) {
  for (const auto &mul_term : com.expression) {
    if (std::any_of(mul_term.begin(), mul_term.end(),
                    [&](const Operator<OperatorInfo> &op) {
                      return op.is_number() and op.value() == ComplexNumber(0);
                    })) {
      continue;
    }
    const bool all_numbers = std::all_of(mul_term.begin(), mul_term.end(),
                                        [&](const Operator<OperatorInfo> &op) {
          return op.is_number();
        });
    const bool all_one = std::all_of(mul_term.begin(), mul_term.end(),
                                        [&](const Operator<OperatorInfo> &op) {
          return op.is_number() and op.value() == ComplexNumber(1.0);
        });
    // as commuter has value means new addition term must be added
    sorted_terms.push_back(vector_type<Operator<OperatorInfo>>());
    auto & new_term = sorted_terms.back();
    if (all_numbers and (not all_one)) {
      // numbers commute so send straight to the front. if they are one don't bother
      new_term.insert(new_term.end(), mul_term.begin(), mul_term.end());
    }
    // insert all terms before the 2 non commuting terms here
    new_term.insert(new_term.end(), sorted_terms[term_index].begin(), sorted_terms[term_index].begin() + i);
    if (not all_numbers) {
      for (const auto &op : mul_term) {
        new_term.push_back(op);
      }
    }
    // insert all terms after the 2 non commuting terms
    new_term.insert(new_term.end(), sorted_terms[term_index].begin() + i + 2, sorted_terms[term_index].end());
  }
}

template <class OperatorInfo>
static bool
bubble_pass(const std::size_t term_index,
            vector_type<vector_type<Operator<OperatorInfo>>> & sorted_terms,
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
sort_multiply_term(const vector_type<Operator<OperatorInfo>> & term,
                  std::function<Expression<OperatorInfo>(const Operator<OperatorInfo> &, const Operator<OperatorInfo> &)> commute,
                  const SortUsing sortUsing) {
  Expression<OperatorInfo> sorted_terms(vector_type<vector_type<Operator<OperatorInfo>>>(1));
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
static vector_type<Operator<OperatorInfo>>
simplify_multiply(const vector_type<Operator<OperatorInfo>> & mul_term) {
  spdlog::trace("simplify single multiply begin");
  if (mul_term.empty()) {
    return mul_term;
  }
  vector_type<Operator<OperatorInfo>> simplified;
  simplified.push_back(Operator<OperatorInfo>(1));
  for (const auto & op : mul_term) {
    spdlog::trace("iterating ops");
    if (op.is_number()) {
      simplified[0].data = simplified[0].value() * op.value();
    } else {
      simplified.push_back(op);
    }
  }
  spdlog::trace("Simply first term");
  if (simplified[0].is_number() and simplified[0].value() == ComplexNumber(0)) {
    simplified.clear(); // if zero return empty vector
    return simplified;
  }
  if (simplified[0].is_number() and simplified[0].value() == ComplexNumber(1) and simplified.size() != 1) {
    simplified.erase(simplified.begin());
  }
  spdlog::trace("simplify single multiply end");
  return simplified;
}

template <class OperatorInfo>
static bool expression_operators_match(const vector_type<Operator<OperatorInfo>> & A,
                                       const vector_type<Operator<OperatorInfo>> & B) {
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
static vector_type<Operator<OperatorInfo>>
combine_expressions(const vector_type<Operator<OperatorInfo>> & A, const vector_type<Operator<OperatorInfo>> & B) {
  vector_type<Operator<OperatorInfo>> C;
  C.reserve(A.size() + 1);
  Operator<OperatorInfo> number;
  if ((!A[0].is_number()) and (!B[0].is_number())) {
    number = Operator<OperatorInfo>(2);
  } else if ((A[0].is_number()) && (!B[0].is_number())) {
    number = Operator<OperatorInfo>(A[0].value() + 1);
  } else if ((!A[0].is_number()) && B[0].is_number()) {
    number = Operator<OperatorInfo>(B[0].value() + 1);
  } else {
    number = Operator<OperatorInfo>(A[0].value() + B[0].value());
  }
  C.push_back(number);
  unsigned start_index = static_cast<unsigned>(A[0].is_number());
  C.insert(C.end(), A.begin() + start_index, A.end());
  return C;
}

template <class OperatorInfo>
struct ComparisonStruct {
  const vector_type<Operator<OperatorInfo>> &full;
  ComparisonStruct() = default;
  ComparisonStruct(const vector_type<Operator<OperatorInfo>> &full) : full(full) {}

  std::size_t num_operators() const {
    // uses fact that simpilifying multiply terms has moved numbers to start
    if (full.empty()) {
      return 0U;
    }
    return full.size() - static_cast<unsigned>(full[0].is_number());
  }

  bool operator<(ComparisonStruct other) const {
    if (num_operators() != other.num_operators()) {
      return num_operators() < other.num_operators();
    }
    if (full.empty()) {
      // if both empty return false
      return false;
    }
    const unsigned first_A = full.size() - num_operators();
    const unsigned first_B = other.full.size() - other.num_operators();
    for (unsigned i = 0; i < num_operators(); ++i) {
      const auto & op_A = full.at(first_A + i);
      const auto & op_B = other.full.at(first_B + i);
      const auto tie_A = std::tie(op_A.order, op_A.info());
      const auto tie_B = std::tie(op_B.order, op_B.info());
      if (tie_A != tie_B) {
        return tie_A < tie_B;
      }
    }
    return false;
  }
};

template <class OperatorInfo>
static void simplify_additions(Expression<OperatorInfo> & exp) {
  spdlog::debug("simplify additions begin");
  std::map<ComparisonStruct<OperatorInfo>, unsigned> unique_mul_terms;
  for (std::size_t i = 0; i < exp.expression.size(); ++i) {
    ComparisonStruct<OperatorInfo> comp(exp.expression[i]);
    const auto it = unique_mul_terms.emplace(comp, i);
    if (not it.second) {
      exp.expression[it.first->second] = combine_expressions(exp.expression[it.first->second], exp.expression[i]);
      exp.expression[i].clear();
    }
  }
  spdlog::debug("simplify additions end");
}

template <class OperatorInfo>
static void removeEmptyVectors(Expression<OperatorInfo> &exp) {
  spdlog::debug("remove empty vectors begin");
  Expression<OperatorInfo> new_exp;
  for (std::size_t add_index = 0; add_index < exp.expression.size(); ++add_index) {
    if (!exp.expression[add_index].empty()) {
      new_exp.expression.emplace_back(std::move(exp.expression[add_index]));
    }
  }
  exp = std::move(new_exp);
  spdlog::debug("remove empty vectors end");
}
template <class OperatorInfo>
Expression<OperatorInfo> Expression<OperatorInfo>::simplify_multiplications() const {
  spdlog::debug("simplify multiplications begin");
  Expression<OperatorInfo> simplified;
  simplified.expression.resize(expression.size());

  for (std::size_t add_index = 0; add_index < expression.size(); ++add_index) {
    simplified.expression[add_index] = simplify_multiply(expression[add_index]);
  }
  spdlog::debug("simplify multiplications end");
  removeEmptyVectors(simplified);
  return simplified;
}

template <class OperatorInfo>
Expression<OperatorInfo> Expression<OperatorInfo>::simplify_numbers() const {
  Expression<OperatorInfo> simplified = simplify_multiplications();
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
static bool bubbleSubs(vector_type<Operator<OperatorInfo>> &exp,
                       std::function<bool(typename vector_type<Operator<OperatorInfo>>::iterator,
                                          vector_type<Operator<OperatorInfo>> &)> subst) {
  spdlog::trace("bubbleSubs begin");
  bool madeSub = false;
  for (auto it = exp.begin(); it != exp.end(); ++it) {
    if (subst(it, exp)) {
      madeSub = true;
    }
  }
  spdlog::trace("bubbleSubs begin");
  return madeSub;
}

template <class OperatorInfo>
static vector_type<Operator<OperatorInfo>>
performMulSubs(const vector_type<Operator<OperatorInfo>> & term,
              std::function<bool(typename vector_type<Operator<OperatorInfo>>::iterator,
                                 vector_type<Operator<OperatorInfo>> &)> subst) {
  spdlog::trace("performMulSubs begin");
  vector_type<Operator<OperatorInfo>> exp = term;
  while(bubbleSubs(exp, subst));
  spdlog::trace("performMulSubs end");
  return exp;
}

template <class OperatorInfo>
Expression<OperatorInfo> Expression<OperatorInfo>::performMultiplicationSubstitutions(
                  std::function<bool(typename vector_type<Operator<OperatorInfo>>::iterator,
                                     vector_type<Operator<OperatorInfo>> &)> subst) const {
  spdlog::debug("performMultiplicationSubstitutions begin");
  Expression<OperatorInfo> final_exp(vector_type<vector_type<Operator<OperatorInfo>>>(expression.size()));
  for (unsigned i = 0; i < expression.size(); ++i) {
    final_exp.expression[i] = performMulSubs(expression[i], subst);
  }
  spdlog::debug("performMultiplicationSubstitutions end");
  return final_exp;
}


//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// all at once zone
//----------------------------------------------------------------------------------------------------------------------
template <class OperatorInfo>
static Expression<OperatorInfo>
interleaved_sort_sub_simple(Expression<OperatorInfo> exp,
      std::function<Expression<OperatorInfo>(const Operator<OperatorInfo> &, const Operator<OperatorInfo> &)> commute,
      std::function<bool(typename vector_type<Operator<OperatorInfo>>::iterator,
                         vector_type<Operator<OperatorInfo>> &)> subst,
      const SortUsing s) {

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
    exp = exp.performMultiplicationSubstitutions(subst);
    exp = exp.simplify_multiplications();
    spdlog::info("Size of expression {}", exp.expression.size());
  }
  return exp;
}

template <class OperatorInfo>
Expression<OperatorInfo> Expression<OperatorInfo>::interleaved_evaluate(
      std::function<Expression<OperatorInfo>(const Operator<OperatorInfo> &, const Operator<OperatorInfo> &)> commute,
      std::function<bool(typename vector_type<Operator<OperatorInfo>>::iterator,
                         vector_type<Operator<OperatorInfo>> &)> subst,
      const SortUsing s) const {

   return interleaved_sort_sub_simple(*this, commute, subst, s);
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
        std::function<bool(typename vector_type<Operator<OperatorInfo>>::iterator,
                           vector_type<Operator<OperatorInfo>> &)> subst,
        const SortUsing s) const {
  // simplify numbers is much faster than the other functions and can make the the other functions run faster so call
  // it frequently

  // Set up logging(defaults to off)
  //-------------------------------------------------------------------------------
  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  console_sink->set_level(spdlog::level::debug);
  console_sink->set_pattern("%^\033[33m[evaluate] [%l%$] [%H:%M:%S] %v%$");
  spdlog::logger logger("Operators", console_sink);
  logger.set_level(spdlog::level::debug);
  //-------------------------------------------------------------------------------

  logger.critical("Start evaluation: Simplift numbers");
  log_expression(logger, *this, "Start");
  auto exp = simplify_numbers();
  log_expression(logger, exp, "Simplify numbers 1");
  logger.critical("Interleaved evaluate");
  exp = exp.interleaved_evaluate(commute, subst, s);
  logger.critical("Simplify multiplications");
  exp = exp.simplify_numbers();
  log_expression(logger, exp, "Simplify numbers 2");
  logger.critical("End");
  return exp;
}


} // end namespace operators

#endif // header guard
