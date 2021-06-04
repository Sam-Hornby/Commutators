// To perform the bogoliubov transform
#pragma once

#include <Expression/Expression.hpp>
#include <Numbers/ImaginaryNumber.hpp>
#include <Comutators/Comutators.hpp>
#include <Expression/Ordering.hpp>
#include <Expression/Sort.hpp>
#include <Expression/Simplify.hpp>
#include <algorithm>
#include <numeric>
#include <array>
#include <Numbers/CompositeNumberExpressions.hpp>
#include <transforms/BogoliubovTransformFunctionsBase.hpp>
#include <transforms/BogoliubovTransformFunctions.hpp>
#include <Utils/Utils.hpp>

namespace operators {

namespace {

// A bogoliubov transform only works on second order terms, we can skip all other terms
template <class InfoA>
struct BogExpressionPair {
  Expression<InfoA> second_order_terms;
  Expression<InfoA> other_terms;
};

template <class InfoA>
static unsigned get_order(const Operator<InfoA> & A,
                          const BogTransformFunctionsBase<InfoA> &transformFunctions) {
  if (A.is_number()) { return 0U; }
  return transformFunctions.get_order(A);
} 

template <class InfoA>
static unsigned get_order(const vector_type<Operator<InfoA>> & term,
                          const BogTransformFunctionsBase<InfoA> &transformFunctions) {
  return std::accumulate(term.begin(), term.end(), 0U, [&] (unsigned acc, const Operator<InfoA> & A) {
    return acc + get_order<InfoA>(A, transformFunctions); 
  });
}

template <class InfoA>
static BogExpressionPair<InfoA> seperate_second_order_terms(
      const Expression<InfoA> & input,
      const BogTransformFunctionsBase<InfoA> &transformFunctions) {
  BogExpressionPair<InfoA> result;
  for (const auto & mul_term : input.expression) {
    const unsigned order = get_order<InfoA>(mul_term, transformFunctions);
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


template <class InfoA>
static boost::optional<unsigned> get_grouping(
         const vector_type<Operator<InfoA>> &term,
         const BogTransformFunctionsBase<InfoA> &transformFunctions) {
  boost::optional<unsigned> result;
  for (const auto &op : term) {
    if (op.is_number()) {
      continue;
    }
    const auto group = transformFunctions.get_group(op.info());
    if (!group) {
      return boost::none;
    }
    if (!result) {
      result = group.get();
    } else {
      if (result.get() != group.get()) {
        return boost::none;
      }
    }
  }
  return result;
}

template <class InfoA>
static SeperatedTerms<InfoA>
group_terms(const Expression<InfoA> &second_order_terms,
            const BogTransformFunctionsBase<InfoA> &transformFunctions) {
  SeperatedTerms<InfoA> result;
  std::unordered_map<unsigned, Expression<InfoA>> grouped_terms;
  for (const auto & mul_term : second_order_terms.expression) {
    auto grouping = get_grouping<InfoA>(mul_term, transformFunctions);
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
create_final_expression(BogExpressionPair<InfoA> &pair,
                        SeperatedTerms<InfoA> &groups,
                        const Expression<InfoA> &input) {
  if (groups.transformable_terms.empty()) {
    return input;
  }
  auto result = std::move(pair.other_terms);
  result = result + groups.non_transformable_terms;
  for (auto &expr : groups.transformable_terms) {
    result = result + expr; // TODO don't use this + loop as O(n**2), just insert back
  } 
  return simplify_numbers(result);
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

std::vector<std::string> term_type_names = {
  "NONE", "DIAGONOL_TERM", "CROSS_TERM", "CONSTANT",
};


// TODO pile
//  - make sure not just that only right bumber of creation / anihilation but also they 
//    have the right id's eg 0 or 1.
//  - make sure if 2 terms the same ie 5C0!C0 + C0!C0 they get combined
//  - add way to also allow state vectors at start and finsh
//  - move convert op to expression to utils

template <class InfoA>
static TermType get_term_type(const vector_type<Operator<InfoA>> &term,
        const BogTransformFunctionsBase<InfoA> &transformFunctions) {
  unsigned creationCount = 0;
  unsigned anihilationCount = 0;
  bool containsAnihilation = false;
  for (const auto &op : term) {
    if (op.is_number()) {continue;}
    if (transformFunctions.get_type(op.info()) == Type::CREATION_OPERATOR) {
      creationCount += 1;
    } else if (transformFunctions.get_type(op.info())
                       == Type::ANIHILATION_OPERATOR) {
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
    if (!term[i].is_number()) {
      break; // we sorted the terms before this step so all numbers should be a front
    }
    result.expression[0].emplace_back(term[i]);
  }
  return result;
}


template <class InfoA>
struct BogConstants {
  static const Expression<InfoA> zero_expression;
  Expression<InfoA> E_constant = zero_expression;
  Expression<InfoA> Y_constant = zero_expression;
  Expression<InfoA> constants = zero_expression;
};

template <class InfoA>
struct TransformedBogConstants {
  Expression<InfoA> e_constant = BogConstants<InfoA>::zero_expression;
  Expression<InfoA> K_constant = BogConstants<InfoA>::zero_expression;
};

template <class InfoA>
const Expression<InfoA> BogConstants<InfoA>::zero_expression = {{{Operator<InfoA>(0)}}};

template <class InfoA>
static BogConstants<InfoA> get_bog_constants(const Expression<InfoA> &input,
         const BogTransformFunctionsBase<InfoA> &transformFunctions) {
  BogConstants<InfoA> result;
  for (const auto &term : input.expression) {
    auto constant = get_constant(term);
    auto term_type = get_term_type<InfoA>(term, transformFunctions);
    assert(term_type != TermType::NONE);
    if (term_type == TermType::DIAGONOL_TERM and result.E_constant == result.zero_expression) {
      result.E_constant = std::move(constant);
    } else if (term_type == TermType::CROSS_TERM and result.Y_constant == result.zero_expression) {
      result.Y_constant = std::move(constant);
    } else if (term_type == TermType::CONSTANT) {
      result.constants = result.constants + constant;
    }
  }
  return result;
}


template <class InfoA>
static OperatorPairs<InfoA> get_bog_operators(const Expression<InfoA> &exp,
                   const BogTransformFunctionsBase<InfoA> &transformFunctions) {
  
  OperatorPairs<InfoA> result;
  std::array<bool, 4> already_found = {false, false, false, false};
  for (const auto &term : exp.expression) {
    for (const auto &op : term) {
      if (std::all_of(already_found.begin(), already_found.end(), [] (bool a) {return a;})) {
        break;
      }
      if (op.is_number()) {continue;}
      const unsigned index = transformFunctions.get_index(op.info());
      if (transformFunctions.get_type(op.info()) == Type::CREATION_OPERATOR) {
        result.creation_ops[index] = op;
      } else if (transformFunctions.get_type(op.info())
                          == Type::ANIHILATION_OPERATOR) {
        result.anihilation_ops[index] = op;
      } else {
        std::abort();
      }
    }
  }
  assert(std::all_of(already_found.begin(), already_found.end(), [] (bool a) {return a}));
  return result;
}

template <class InfoA>
static OperatorPairs<InfoA> get_transformed_operators(
          const Expression<InfoA> &exp,
          const BogTransformFunctionsBase<InfoA> &transformFunctions) {
  return transformFunctions.transform_ops(
      get_bog_operators<InfoA>(exp, transformFunctions));
}


template <class InfoA>
static bool expressions_equivalent(Expression<InfoA> A, Expression<InfoA> B) {
  // Don't care about ordering values so set all fixed number
  A = numbers_first(simplify_numbers(A));
  B = numbers_first(simplify_numbers(B));

  const auto ops_less_than = [] (const vector_type<Operator<InfoA>> &a,
                                 const vector_type<Operator<InfoA>> &b) {
    if (a.size() != b.size()) {
      return a.size() < b.size();
    }
    for (std::size_t i = 0; i < a.size(); ++i) {
      if (a[i] != b[i]) {
        return a[i].data < b[i].data;
      }
    }
    return false;
  };
  // Addition is assumed to always be comutative so change order of additions
  // to match terms
  std::sort(A.expression.begin(), A.expression.end(), ops_less_than);
  std::sort(B.expression.begin(), B.expression.end(), ops_less_than);

  return A == B;
}

template <class InfoA>
static bool expression_matches(const Expression<InfoA> &input,
                               OperatorPairs<InfoA> op_pair,
                               BogConstants<InfoA> constants) {
  auto expected =
      (constants.E_constant *
        ((op_pair.creation_ops[0] * op_pair.anihilation_ops[0])
       + (op_pair.creation_ops[1] * op_pair.anihilation_ops[1])))
    + (constants.Y_constant *
        ((op_pair.creation_ops[0] * op_pair.creation_ops[1])
       + (op_pair.anihilation_ops[1] * op_pair.anihilation_ops[0])))
  + constants.constants;
  return expressions_equivalent(input, std::move(expected));
}

template <class InfoA>
static bool can_transform(
      const Expression<InfoA> & input,
      const BogTransformFunctionsBase<InfoA> &transformFunctions) {
  boost::optional<Expression<InfoA>> E_constant;
  boost::optional<Expression<InfoA>> Y_constant;
  unsigned E_count = 0;
  unsigned Y_count = 0;
  for (const auto &term : input.expression) {
    if (term.empty()) {continue;}
    auto constant = get_constant(term);
    const auto type = get_term_type<InfoA>(term, transformFunctions); 
    spdlog::trace("Term type = {}", term_type_names[static_cast<unsigned>(type)]);
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

  return expression_matches<InfoA>(
                input,
                get_bog_operators<InfoA>(input, transformFunctions),
                get_bog_constants<InfoA>(input, transformFunctions)); 
}

// At this stage we are getting to the stage where we know we are going to make
// transformation. Transform will look like 
// E (c_0! c_0 + c_1! c_1) + Y (c_0! c_1! + c_1 c_0) --->
// e (b_0! b_0 + b_1! b_0) + K
// Constant e and K are determined by E, Y and whether the transforms are on fermions
// or bosons
template <class InfoA>
static SeperatedTerms<InfoA> can_transform(SeperatedTerms<InfoA> input,
               const BogTransformFunctionsBase<InfoA> &transformFunctions) {
  for (auto &term : input.transformable_terms) {
    term = numbers_first(term);
    term = sort<InfoA>(term, error_commutator<InfoA>);
    // simplify numbers as going to rely on numbers being at start of each term
    term = simplify_numbers(term);
  } 
  std::vector<Expression<InfoA>> new_transformable_terms;
  for (auto &term : input.transformable_terms) {
    const bool transformable = can_transform<InfoA>(term, transformFunctions);
    spdlog::trace("Term: {} transformable {}", term.name(), transformable); 
    if (transformable) {
      new_transformable_terms.emplace_back(std::move(term));
    } else {
      input.non_transformable_terms = input.non_transformable_terms + term;
    }
  }
  input.transformable_terms = std::move(new_transformable_terms);
  return input;
}



// Bosons e = squareroot(E^2 - Y^2)    K = e - E
// Fermions e = squareroot(E^2 + Y^2)  K = E - e
template <class InfoA>
TransformedBogConstants<InfoA> get_transformed_constants(BogConstants<InfoA> &input,
                                                         const bool fermions) {
  TransformedBogConstants<InfoA> result;
  auto E_constant = ExpressionExpr::create(
                        convertNumbersToEmptyInfo<InfoA>(std::move(input.E_constant)));
  auto Y_constant = ExpressionExpr::create(
                        convertNumbersToEmptyInfo<InfoA>(std::move(input.Y_constant)));
  auto constants = ExpressionExpr::create(
                        convertNumbersToEmptyInfo<InfoA>(std::move(input.constants)));
  boost::optional<CompositeNumber> K_constant;
  if (fermions) {
    auto e_constant =
             SquareRootExpr::create(
               AddExpr::create(
                 SquareExpr::create(E_constant),
                 SquareExpr::create(Y_constant)));
    result.e_constant = {{{Operator<InfoA>(e_constant)}}};
    K_constant = SubExpr::create(E_constant, e_constant);
  } else {
    auto e_constant =
             SquareRootExpr::create(
               SubExpr::create(
                 SquareExpr::create(E_constant),
                 SquareExpr::create(Y_constant)));
    result.e_constant = {{{Operator<InfoA>(e_constant)}}};
    K_constant = SubExpr::create(e_constant, E_constant);
  }
  // add on any constant terms to K constant
  assert(K_constant);
  if (input.constants.expression == BogConstants<InfoA>::zero_expression.expression) {
    result.K_constant = {{{Operator<InfoA>(K_constant.get())}}};
  } else {
    result.K_constant = {{{Operator<InfoA>(
        AddExpr::create(constants, K_constant.get()))}}};
  }
  return result;
}



template <class InfoA>
static Expression<InfoA>
create_new_expression(OperatorPairs<InfoA> bog_ops,
                      TransformedBogConstants<InfoA> constants) {
  Expression<InfoA> result = (constants.e_constant *
                              ((bog_ops.creation_ops[0] * bog_ops.anihilation_ops[0]) +
                               (bog_ops.creation_ops[1] * bog_ops.anihilation_ops[1]))) +
                              constants.K_constant;
  
  return result;
}

template <class InfoA>
static Expression<InfoA> make_transformation(
               Expression<InfoA> input,
               const BogTransformFunctionsBase<InfoA> &transformFunctions) {
  auto constants = get_bog_constants<InfoA>(input, transformFunctions); 
  auto transformed_constants = get_transformed_constants<InfoA>(
             constants, transformFunctions.get_op_type() == OperatorType::FERMION);
  auto transformed_ops = get_transformed_operators<InfoA>(
      input, transformFunctions);
  return create_new_expression<InfoA>(std::move(transformed_ops),
                                      std::move(transformed_constants));
}

template <class InfoA>
static std::vector<Expression<InfoA>>
make_transformation(std::vector<Expression<InfoA>> grouped_terms,
                    const BogTransformFunctionsBase<InfoA> &transformFunctions) {
  for (auto &expr : grouped_terms) {
    expr = make_transformation<InfoA>(std::move(expr), transformFunctions);
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

template <class Info>
using OpPairType = std::pair<Operator<Info>, Operator<Info>>;

template <class Info>
OpPairType<Info> find_op_pair(
      const vector_type<Operator<Info>> & term) {
  boost::optional<Operator<Info>> first, second;
  for (const auto &op : term) {
    if (op.is_number()) {
      continue;
    }
    if (first && second) {std::abort();}
    if (first) {
      second = op;
    } else {
      first = op;
    }
  }
  return std::make_pair(first.get(), second.get());
}

template <class Info>
void maybeInsertInfosAndGroup(std::map<Info, boost::optional<GroupAndIndex>> &result,
                              const OpPairType<Info> &op_pair) {
  
  const std::array all_terms{op_pair.first.info(),
                       op_pair.second.info(),
                       hermition_conjugate(op_pair.first).info(),
                       hermition_conjugate(op_pair.second).info()};

  if (all_terms[0] == all_terms[1] or all_terms[0] == all_terms[3]) {
    // not a cross term
    return;
  }
  const std::array its {result.find(all_terms[0]),
                        result.find(all_terms[1]),
                        result.find(all_terms[2]),
                        result.find(all_terms[3])};
  if (std::all_of(its.begin(), its.end(), [&](const auto &it) {return it == result.end();})) {
    unsigned group = result.size();
    bool index = false;
    for (const auto &term : all_terms) {
      result.emplace(term, boost::optional<GroupAndIndex>({group, index}));
      index = index xor true;
    } 
    return;
  }
  if (std::any_of(its.begin(), its.end(), [&] (const auto &it) {
       return it == result.end() or
              (not static_cast<bool>(it->second)) or
              it->second->group != its[0]->second->group;
     })) {
    
    // set all to none
    for (const auto &term : all_terms) {
      result[term] = boost::none; 
    }
  }
}


template <class Info>
std::map<Info, GroupAndIndex> create_group_map(
      const Expression<Info> &input,
      const BogTransformFunctionsBase<Info> &transformFunctions) {
  auto expressions = seperate_second_order_terms<Info>(input, transformFunctions);
  expressions.second_order_terms =
        simplify_numbers(sort<Info>(
              numbers_first(expressions.second_order_terms),
              error_commutator<Info>)); 
  std::map<Info, boost::optional<GroupAndIndex>> provisionalResult;
  for (const auto &term : expressions.second_order_terms.expression) {
    const auto op_pair = find_op_pair(term);
    // find cross terms, ignore non cross terms
    // when find cross term insert both ops and the hermitian conj of both into map with
    // same group. If already in map dunno

    maybeInsertInfosAndGroup(provisionalResult, op_pair);
  }
  std::map<Info, GroupAndIndex> result;
  for (auto &item : provisionalResult) {
    if (item.second) {
      // maybe instead of not inserting should just seperate out all these so they have
      // a unique group each. means won't fail if can't transform but won't try to transform
      result.emplace(std::move(item.first), std::move(item.second.get()));
    }
  }
  return result;
}

 } // namespace

template <class InfoA>
Expression<InfoA>
bogoliubov_transform(const Expression<InfoA> & input,
                     const BogTransformFunctionsBase<InfoA> &transformFunctions,
                     const bool recurse = true) {
  spdlog::info("bogoliubov_transform start: {}", recurse);
  spdlog::debug("Bogoliubov Input: {}", input.print(false));
  auto expressions = seperate_second_order_terms<InfoA>(input, transformFunctions); // done 
  spdlog::debug("Non second order terms: {}", expressions.other_terms.print(false));
  spdlog::debug("Second order terms: {}", expressions.second_order_terms.print(false));
  spdlog::info("Pair up second order terms");
  auto groups = group_terms<InfoA>(expressions.second_order_terms, transformFunctions);  // done 
  log_groups(groups);
  spdlog::info("Check transformable");
  groups = can_transform<InfoA>(std::move(groups), transformFunctions); // get term type todo pile
  log_groups(groups);
  spdlog::info("Try to make substitutions");
  // todo: get new constants from old constants; implement create new term function taking transformed constants
  groups.transformable_terms =
      make_transformation<InfoA>(std::move(groups.transformable_terms), transformFunctions);

  auto result = create_final_expression(expressions, groups, input);
  if (recurse) {
    return bogoliubov_transform(result, SwapIndices<InfoA>(transformFunctions), false);
  }
  return result;
}

template <class InfoA>
Expression<InfoA>
bogoliubov_transform(const Expression<InfoA> & input,
                     const OperatorType op_type) {
  spdlog::info("Default bogoliubov transform start");
  DefaultBogTransformFunctions<InfoA> transformFunctions(op_type);
  FindGroupsTransformFunctions<InfoA>
      complete_transform_functions(transformFunctions,
                                   create_group_map<InfoA>(input, transformFunctions));
  spdlog::info("Create group map with {} entries", transformFunctions.groupMap.size());
  return bogoliubov_transform<InfoA>(input, complete_transform_functions);
}

}
