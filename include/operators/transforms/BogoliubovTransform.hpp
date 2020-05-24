// To perform the bogoliubov transform
#pragma once

#include "Expression.hpp"
#include "ImaginaryNumber.hpp"
#include "Transform.hpp"
#include "Comutators.hpp"
#include "Ordering.hpp"
#include <algorithm>
#include <numeric>

namespace operators {

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
  Expression<InfoA> non_transformable_terms;
  std::vector<Expression<InfoA>> transformable_terms;
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
      result.non_transformable_terms.expression.emplace_back(mul_term);
      continue;
    }
    auto &terms = grouped_terms[grouping.get()];
    terms.expression.emplace_back(mul_term);
  } 
  for (auto &entry : grouped_terms) {
    result.transformable_terms.emplace_back(std::move(entry.second));
  }
  return result;
}

template <class InfoA>
static Expression<InfoA>
create_final_expression(BogExpressionPair<InfoA> &pair, SeperatedTerms<InfoA> &groups) {
  auto result = std::move(pair.other_terms);
  result = result + groups.non_transformable_terms;
  for (auto &expr : groups.transformable_terms) {
    result = result + expr; // TODO don't use this + loop as O(n**2), just insert back
  } 
  return result;
}



template <class InfoA>
Expression<InfoA> error_commutator(const Operator<InfoA> & A, const Operator<InfoA> &B) {
  return commute_numbers<InfoA>(A, B,
           [] (const Operator<InfoA> &, const Operator<InfoA> &) -> Expression<InfoA> {
    throw std::logic_error("Shouldn't be moving operators");
  });
}

enum class TermType {
  NONE,
  DIAGONOL_TERM,
  CROSS_TERM,
  CONSTANT,
};



// TODO pile
//  - make sure not just that only right bumber of creation / anihilation but also they 
//    have the right id's eg 0 or 1.
//  - make sure if 2 terms the same ie 5C0!C0 + C0!C0 they get combined
//  - add way to also allow state vectors at start and finsh
//  - move convert op to expression to utils

template <class InfoA>
static TermType get_term_type(const vector_type<InfoA> &term) {
  unsigned creationCount = 0;
  unsigned anihilationCount = 0;
  bool containsAnihilation = false;
  for (const auto &op : term) {
    if (op.is_number()) {continue;}
    if (op.info().type == Type::CREATION_OPERATOR) {
      creationCount += 1;
    } else if (op.info().type == Type::ANIHILATION_OPERATOR) {
      anihilationCount += 1;
    } else {
      return TermType::NONE;
    }
  }
  if (creationCount == 1 and anihilationCount == 1) {
    return TermType::DIAGONOL_TERM;
  }
  if ((creationCount == 2 and anihilationCount == 0) or
      (creationCount == 0 and anihilationCount == 2)) {
    return TermType::CROSS_TERM;
  }
  if (creationCount == 0 and anihilationCount == 0) {
    return TermType::CONSTANT;
  } 
  return TermType::NONE;
}

template <class InfoA>
static Expression<InfoA> convert_op_to_expression(Operator<InfoA> op) {
  Expression<InfoA> result;
  result.expression.emplace_back(vector_type<Operator<InfoA>>(1, std::move(op)));
  return result;
}

template <class InfoA>
static Expression<InfoA> get_constant(const vector_type<Operator<InfoA>> &term) {
  if (!term.front().is_number()) {
    return {{{number<InfoA>(1.0)}}};
  } 
  Expression<InfoA> result = convert_op_to_expression(term.front());
  for (unsigned i = 1U; i < term.size(); ++i) {
    if (!term.front().is_number()) {
      break; // we sorted the terms before this step so all numbers should be a front
    }
    result.expression[0].emplace_back(term[i]);
  }
  return result;
}

template <class InfoA>
static bool can_transform(const Expression<InfoA> & input) {
  boost::optional<Expression<InfoA>> E_constant;
  boost::optional<Expression<InfoA>> Y_constant;
  unsigned E_count = 0;
  unsigned Y_count = 0;
  for (const auto &term : input.expression) {
    if (term.empty()) {continue;}
    auto constant = get_constant(term);
    const TermType type = get_term_type(term); 
    spdlog::trace("Term type = {}", type);
    spdlog::trace("Contstant term = {}", constant.print(false));
    if (type == TermType::NONE) {
      return false;
    }
    if (type == TermType::CONSTANT) {
      continue;
    }
    auto & old_constant = type == TermType::DIAGONOL_TERM ? E_constant : Y_constant;
    if (old_constant) {
      if (old_constant.get() != constant) {
        return false;
      }
    } else {
      old_constant = constant;
    }
    if (type == TermType::DIAGONOL_TERM) {
      ++E_count;
    } else {
      ++Y_count;
    }
  }
  if (E_count != 2 or Y_count != 2) {
    return false;
  }
  return true; 
}

// At this stage we are getting to the stage where we know we are going to make
// transformation. Transform will look like 
// E (c_0! c_0 + c_1! c_1) + Y (c_0! c_1! + c_1 c_0) --->
// e (b_0! b_0 + b_1! b_0) + K
// Constant e and K are determined by E, Y and whether the transforms are on fermions
// or bosons
template <class InfoA, class TransformFunctions>
static SeperatedTerms<InfoA> can_transform(SeperatedTerms<InfoA> input) {
  for (auto &term : input.transformable_terms) {
    term = numbers_first(term);
    term = term.sort(error_commutator<InfoA>);
    // simplify numbers as going to rely on numbers being at start of each term
    term = term.simplify_numbers();
  } 
  std::vector<Expression<InfoA>> new_transformable_terms;
  for (auto &term : input.transformable_terms) {
    if (can_transform<InfoA>(term)) {
      new_transformable_terms.emplace_back(std::move(term));
    } else {
      input.non_transformable_terms = input.non_transformable_terms + term;
    }
  }
  input.transformable_terms = std::move(new_transformable_terms);
  return input;
}

template <class InfoA, class TransformFunctions>
static Expression<InfoA> make_transformation(Expression<InfoA> input) {
  return input; 
}

template <class InfoA, class TransformFunctions>
static std::vector<Expression<InfoA>>
make_transformation(std::vector<Expression<InfoA>> grouped_terms) {
  for (auto &expr : grouped_terms) {
    expr = make_transformation<InfoA, TransformFunctions>(std::move(expr));
  }
  return grouped_terms;
}

template <class InfoA>
static void log_groups(const SeperatedTerms<InfoA> &groups) {
  if (!groups.non_transformable_terms.expression.empty()) {
    spdlog::warn("Some terms cannot be transformed {}", groups.non_transformable_terms.print(false));
  }
  for (const auto &expr : groups.transformable_terms) {
    spdlog::debug("Transformable term: {}", expr.print(false));
  }
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
  log_groups(groups);
  spdlog::info("Check transformable");
  groups = can_transform<InfoA, TransformFunctions>(std::move(groups));
  log_groups(groups);
  spdlog::info("Try to make substitutions");
  groups.transformable_terms =
      make_transformation<InfoA, TransformFunctions>(std::move(groups.transformable_terms));

  return create_final_expression(expressions, groups);
}

}
