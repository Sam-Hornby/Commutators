#pragma once

#include <Expression/Operator.hpp>
#include <Utils/Utils.hpp>
#include <vector>
#include <boost/container/small_vector.hpp>
#include <spdlog/spdlog.h>
#include <Expression/Expression.hpp>

namespace operators {

// substitution functions take an iterator and the vector, the it is where to start looking for pattern from and the
// vector gives the end of the sequence and if subsituting in a longer expression then for insertions.
// If they find a pattern they must replace terms with new terms and insert any extra.
// If they find a pattern they must also return true, otherwise return false


// When replacement expression is smaller than what is being substituted is useful to set excess operators to 1 and then
// after the substitution phase call simplify numbers to remove them
template <class OperatorInfo>
void set_to_one(typename vector_type<Operator<OperatorInfo>>::iterator it, const std::size_t n) {
  for (std::size_t i = 0; i < n; ++i) {
    *(it + i) = Operator<OperatorInfo>(1.0);
  }
}

template <class Info>
void assertInRange(typename vector_type<Operator<Info>>::iterator start,
                   vector_type<Operator<Info>> & exp) {
  assert(start >= exp.begin());
  assert(start < exp.end());
}

// if every state in the expression is orthognal (and you are not generating any new ones) can zero all inner products
// between different states and set to one inner product of identical states. Assumes that the value of the operator
// info is used to show the state
template <class OperatorInfo>
bool all_states_orthognal(typename vector_type<Operator<OperatorInfo>>::iterator start,
                          vector_type<Operator<OperatorInfo>> & exp) {
  assert(start != exp.end());
  assertInRange<OperatorInfo>(start, exp);
  if (is_hc_state_vector(*start)) {
    const auto & info = start->info();
    if ((start + 1) != exp.end() and is_state_vector(*(start + 1))) {
      if ((start + 1)->info().match(info)) {
        *start = Operator<OperatorInfo>(1);
        set_to_one<OperatorInfo>(start + 1, 1);
      } else {
        *start = Operator<OperatorInfo>(0);
        set_to_one<OperatorInfo>(start + 1, 1);
      }
      return true;
    }
  }
  return false;
}

// assumes the the value of operator_info for the vacuum state and its hc is 0
template <class OperatorInfo>
bool anihilate_vacuum(typename vector_type<Operator<OperatorInfo>>::iterator start,
                      vector_type<Operator<OperatorInfo>> & exp) {
  assertInRange<OperatorInfo>(start, exp);
  if (start + 1 == exp.end()) {
    return false;
  }
  assertInRange<OperatorInfo>(start + 1, exp);
  if (start->is_number() or (start + 1)->is_number()) {
    return false;
  }
  bool anihilate_hc = start->info().isHCVacuumState() and is_creation_op(*(start + 1));
  bool anihilate_st = is_anihilation_op(*start) and (start + 1)->info().isVacuumState();
  if (anihilate_hc or anihilate_st) {
    *start = Operator<OperatorInfo>(0);
    set_to_one<OperatorInfo>(start + 1, 1);
    return true;
  }
  return false;
}

template <class OperatorInfo>
bool no_subs(typename vector_type<Operator<OperatorInfo>>::iterator, vector_type<Operator<OperatorInfo>> &) {
  return false;
}

template <class OperatorInfo>
bool fermion_dual_occupation(typename vector_type<Operator<OperatorInfo>>::iterator start,
                             vector_type<Operator<OperatorInfo>> & exp) {
  // for fermions a * a = 0 and a! * a! = 0 (from fact anti comutator is zero)
  assertInRange<OperatorInfo>(start, exp);
  if (start + 1 == exp.end()) {
    return false;
  }
  assertInRange<OperatorInfo>(start + 1, exp);
  if (start->is_number() or (start + 1)->is_number()) {
    return false;
  }
  bool ani = is_anihilation_op<OperatorInfo>(*start) and is_anihilation_op<OperatorInfo>(*(start + 1));
  bool cre = is_creation_op<OperatorInfo>(*start) and is_creation_op<OperatorInfo>(*(start + 1));
  if ((ani or cre) and start->info().match((start + 1)->info())) {
    *start = Operator<OperatorInfo>(0);
    set_to_one<OperatorInfo>(start + 1, 1);
    return true;
  }
  return false;
}

template <class Info>
using SubstIt = typename vector_type<Operator<Info>>::iterator;
template <class Info>
using SubstVec = vector_type<Operator<Info>>&;

template <class Info>
bool default_boson_subs(SubstIt<Info> start, SubstVec<Info> exp) {
  return anihilate_vacuum<Fock1DInfo>(start, exp) or
         all_states_orthognal<Fock1DInfo>(start, exp);
}

template <class Info>
bool default_fermion_subs(SubstIt<Info> start, SubstVec<Info> exp) {
  return anihilate_vacuum<Fock1DInfo>(start, exp) or
         all_states_orthognal<Fock1DInfo>(start, exp) or
         fermion_dual_occupation<Fock1DInfo>(start, exp);
}

} // end name space
