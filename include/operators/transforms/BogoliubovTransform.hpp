// To perform the bogoliubov transform
#pragma once

#include "Expression.hpp"
#include "ImaginaryNumber.hpp"
#include "Transform.hpp"
#include <algorithm>
#include <numeric>

namespace operators {
/*
enum class OperatorType {
  BOSON,
  FERMION,
};

// c_0! = (coeff_a * b_0!) + (coeff_b * b_1)
// c_1! = (coeff_a * b_1!) +/- (coeff_b * b_0)

template <class InfoA>
bool isSecondOp(const Operator<InfoA> & op) {
  // Will give a default but suspect for most custom classes, an overload
  // for that template instance will be needed.  Same for the create function
  std::abort();
}

template <>
bool isSecondOp(const Operator<Fock1DInfo> & op) {
  return op.info().x_coordinate < 0;
}

template <class InfoA>
Operator<InfoA> createBogOp(Operator<InfoA> op, const bool swap) {
  std::abort();
  // TODO work out nice way for this
}

template <>
Operator<Fock1DInfo> createBogOp(Operator<Fock1DInfo> op, const bool swap) {
  assert(isFockOpType(op.info().type));
  assert(op.info().x_coordinate != 0);
  if (op.info().symbol != 'b') {
    op.info().symbol = 'b';
  } else {
    op.info().symbol = 'c';
  }
  if (swap) {
    op.info().x_coordinate = (op.info().x_coordinate *= -1);
    op.info().type = op.info().type == Type::CREATION_OPERATOR ?
                                         Type::ANIHILATION_OPERATOR : Type::CREATION_OPERATOR;
  }
  return op;
}

template <class InfoA>
Expression<InfoA>
bogoliubov_transform(const Expression<InfoA> & input,
                     const OperatorType type) {
  // maybe better off finding coefficents first then substituing
  auto coeff_a = named_number<InfoA>('u');
  auto coeff_b = named_number<InfoA>('v');
  auto result = transform_expression<InfoA, InfoA>(input, [&] (const Operator<InfoA> & op) {
    if (op.is_number() or isVectorType(op.info().type)) {
      return Expression<InfoA>({{op}});
    }
    if (not isFockOpType(op.info().type)) {
      throw std::logic_error("Unexpected type");
    }
    const auto sign_term = [&] () {
      if (type == OperatorType::FERMION and isSecondOp<InfoA>(op)) {
        return number<InfoA>(-1);
      }
      return number<InfoA>(1);
    }();
    return (coeff_a * createBogOp<InfoA>(op, false)) +
             (sign_term * coeff_b * createBogOp<InfoA>(op, true));
  });

  result = result.simplify_numbers();
  // std::sort on the operators, then try and factorise
  return result;
}
*/

namespace {

// A bogoliubov transform only works on second order terms, we can skip all other terms
template <class InfoA>
struct BogExpressionPair {
  Expression<InfoA> second_order_terms;
  Expression<InfoA> other_terms;
};

struct DefaultBogTransformFunctions {
  template <class T>
  static unsigned get_order(const Operator<T> &) { return 1U; }
  
  // give a few example groupings for certain infos
  static unsigned get_group(const Operator<Fock1DInfo> &op) {
    return std::abs(op.info().x_coordinate); 
  }
};

template <class InfoA, class TransformFunctions>
static unsigned get_order(const Operator<InfoA> & A) {
  if (A.is_number()) { return 0U; }
  return TransformFunctions::get_order(A);
} 

template <class InfoA, class TransformFunctions>
static unsigned get_order(const vector_type<Operator<InfoA>> & term) {
  return std::accumulate(term.begin(), term.end(), 0U, [] (unsigned acc, const Operator<InfoA> & A) {
    return acc + get_order<InfoA, TransformFunctions>(A); 
  });
}

template <class InfoA, class TransformFunctions>
static BogExpressionPair<InfoA> seperate_second_order_terms(const Expression<InfoA> & input) {
  BogExpressionPair<InfoA> result;
  for (const auto & mul_term : input.expression) {
    const unsigned order = get_order<InfoA, TransformFunctions>(mul_term);
    if (order == 2) {
      result.second_order_terms.expression.emplace_back(mul_term);
    } else {
      result.other_terms.expression.emplace_back(mul_term); 
    }
  }
  return result;
}

template <class InfoA>
struct SeperatedTerms {
  // Terms that belong to more than a single group and so can't be transformed
  Expression<InfoA> multi_group_terms;
  std::vector<Expression<InfoA>> grouped_terms;
};


template <class InfoA, class TransformFunctions>
static boost::optional<unsigned> get_grouping(const vector_type<Operator<InfoA>> &term) {
  boost::optional<unsigned> result;
  for (const auto &op : term) {
    if (op.is_number()) {
      continue;
    }
    const unsigned group = TransformFunctions::get_group(op);
    if (!result) {
      result = group;
    } else {
      if (result.get() != group) {
        return boost::none;
      }
    }
  }
  return result;
}

template <class InfoA, class TransformFunctions>
static SeperatedTerms<InfoA>
group_terms(const Expression<InfoA> &second_order_terms) {
  SeperatedTerms<InfoA> result;
  std::unordered_map<unsigned, Expression<InfoA>> grouped_terms;
  for (const auto & mul_term : second_order_terms.expression) {
    auto grouping = get_grouping<InfoA, TransformFunctions>(mul_term);
    if (!grouping) {
      // add to the multi group terms and continue
      result.multi_group_terms.expression.emplace_back(mul_term);
      continue;
    }
    auto &terms = grouped_terms[grouping.get()];
    terms.expression.emplace_back(mul_term);
  } 
  for (auto &entry : grouped_terms) {
    result.grouped_terms.emplace_back(std::move(entry.second));
  }
  return result;
}

template <class InfoA>
static Expression<InfoA>
create_final_expression(BogExpressionPair<InfoA> &pair, SeperatedTerms<InfoA> &groups) {
  auto result = std::move(pair.other_terms);
  result = result + groups.multi_group_terms;
  for (auto &expr : groups.grouped_terms) {
    result = result + expr; // TODO don't use this + loop as O(n**2), just insert back
  } 
  return result;
}

} // namespace

template <class InfoA, class TransformFunctions = DefaultBogTransformFunctions>
Expression<InfoA>
bogoliubov_transform(const Expression<InfoA> & input) {
  spdlog::info("bogoliubov_transform start");
  spdlog::debug("Bogoliubov Input: {}", input.print(false));
  auto expressions = seperate_second_order_terms<InfoA, TransformFunctions>(input); 
  spdlog::debug("Non second order terms: {}", expressions.other_terms.print(false));
  spdlog::debug("Second order terms: {}", expressions.second_order_terms.print(false));
  spdlog::info("Pair up second order terms");
  auto groups = group_terms<InfoA, TransformFunctions>(expressions.second_order_terms);   
  if (!groups.multi_group_terms.expression.empty()) {
    spdlog::warn("Some terms cannot be grouped {}", groups.multi_group_terms.print(false));
  }
  for (const auto &expr : groups.grouped_terms) {
    spdlog::debug("Grouped term: {}", expr.print(false));
  }
  spdlog::info("Try to make substitutions");


  return create_final_expression(expressions, groups);
}
//template <class InfoA>
//Expression<InfoA>
//bogoliubov_transform(const Expression<InfoA> & input,
//                    const OperatorType) {
//  return input;
//}


}
