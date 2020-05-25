#pragma once

#include "Expression.hpp"

namespace operators {

//----------------------------------------------------------------------------------------------------------------------
// simplify numbers zone
//----------------------------------------------------------------------------------------------------------------------
namespace {

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
    if (op.is_evaluated_number()) {
      simplified[0].data = simplified[0].value() * op.value();
    } else {
      simplified.push_back(op);
    }
  }
  spdlog::trace("Simply first term");
  if (simplified[0].is_evaluated_number() and simplified[0].value() == ComplexNumber(0)) {
    simplified.clear(); // if zero return empty vector
    return simplified;
  }
  if (simplified[0].is_evaluated_number() and simplified[0].value() == ComplexNumber(1) and simplified.size() != 1) {
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
  unsigned start_index_A = static_cast<unsigned>(A[0].is_evaluated_number());
  unsigned size_B = B.size();
  unsigned start_index_B = static_cast<unsigned>(B[0].is_evaluated_number());
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
  if ((!A[0].is_evaluated_number()) and (!B[0].is_evaluated_number())) {
    number = Operator<OperatorInfo>(2);
  } else if ((A[0].is_evaluated_number()) && (!B[0].is_evaluated_number())) {
    number = Operator<OperatorInfo>(A[0].value() + 1);
  } else if ((!A[0].is_evaluated_number()) && B[0].is_evaluated_number()) {
    number = Operator<OperatorInfo>(B[0].value() + 1);
  } else {
    number = Operator<OperatorInfo>(A[0].value() + B[0].value());
  }
  C.push_back(number);
  unsigned start_index = static_cast<unsigned>(A[0].is_evaluated_number());
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
    return full.size() - static_cast<unsigned>(full[0].is_evaluated_number());
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
      if (op_A.data.which() != op_B.data.which()) {
        return op_A.data.which() < op_B.data.which();
      }
      if (op_A.is_number()) {
        // assuming no evaluated numbers here
        const auto tie_A = std::make_pair(op_A.order, op_A.named_number());
        const auto tie_B = std::make_pair(op_B.order, op_B.named_number());
        if (tie_A != tie_B) {
          return tie_A < tie_B;
        }
        continue;
      }
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
  spdlog::trace("simplify additions begin");
  std::map<ComparisonStruct<OperatorInfo>, unsigned> unique_mul_terms;
  for (std::size_t i = 0; i < exp.expression.size(); ++i) {
    ComparisonStruct<OperatorInfo> comp(exp.expression[i]);
    const auto it = unique_mul_terms.emplace(comp, i);
    if (not it.second) {
      exp.expression[it.first->second] = combine_expressions(exp.expression[it.first->second], exp.expression[i]);
      exp.expression[i].clear();
    }
  }
  spdlog::trace("simplify additions end");
}

template <class OperatorInfo>
static void removeEmptyVectors(Expression<OperatorInfo> &exp) {
  spdlog::trace("remove empty vectors begin");
  Expression<OperatorInfo> new_exp;
  for (std::size_t add_index = 0; add_index < exp.expression.size(); ++add_index) {
    if (!exp.expression[add_index].empty()) {
      new_exp.expression.emplace_back(std::move(exp.expression[add_index]));
    }
  }
  exp = std::move(new_exp);
  spdlog::trace("remove empty vectors end");
}

} // namespace

template <class OperatorInfo>
Expression<OperatorInfo> simplify_multiplications(const Expression<OperatorInfo> &expr) {
  spdlog::trace("simplify multiplications begin");
  Expression<OperatorInfo> simplified;
  simplified.expression.resize(expr.expression.size());

  for (std::size_t add_index = 0; add_index < expr.expression.size(); ++add_index) {
    simplified.expression[add_index] = simplify_multiply(expr.expression[add_index]);
  }
  spdlog::trace("simplify multiplications end");
  removeEmptyVectors(simplified);
  return simplified;
}

template <class OperatorInfo>
Expression<OperatorInfo> simplify_numbers(const Expression<OperatorInfo> &expr) {
  Expression<OperatorInfo> simplified = simplify_multiplications(expr);
  simplify_additions(simplified);
  removeEmptyVectors(simplified);
  return simplified;
}

} // namespace operators

// end numbers zone
//----------------------------------------------------------------------------------------------------------------------


